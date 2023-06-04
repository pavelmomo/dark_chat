#include "server_handle.h"


Server_Handle::Server_Handle()
{
    this->commands = new queue_safe <Command>;
    this->shut_threads_fd = eventfd(0,EFD_SEMAPHORE);
}
Server_Handle::~Server_Handle()
{
    safe_cout.lock();
    std::cout<<"Вызов деструктора"<<std::endl;
    safe_cout.unlock();
    eventfd_write(shut_threads_fd,1);      //выкючение потоков
    delete [] handle_epoll_threads;
    while(this->commands->size() > 0)
    {
        this->commands->pop();
    }
    close(this->main_socket);

}
bool Server_Handle::Set_up_server(uint16_t port, unsigned int size_of_connection_queue, unsigned int max_events_count,unsigned int number_of_threads)
{
    if (number_of_threads < 1 )
    {
        std::cout<<"Too many threads"<<std::endl;
        return 1;
    }
    this->max_events_count = max_events_count;
    this->number_of_threads = number_of_threads;
    this->main_socket = socket(AF_INET,SOCK_STREAM,0);        //создание серверного сокета
    if (main_socket < 0)
    {
        std::cout<<"Socket make error"<<std::endl;
        return 1;
    }
    /*
    int nonBlocking = 1;
    if (fcntl( this->main_socket, F_SETFL, O_NONBLOCK, nonBlocking ) == -1 )       //перевод сокета в неблокирующий режим
    {
        std::cout<<"Set non-blocking mode failed!"<<std::endl;
        return 1;
    }
    */
    main_socket_info.sin_family = AF_INET;
    main_socket_info.sin_port = htons(port);
    main_socket_info.sin_addr.s_addr = INADDR_ANY;
    if (bind(main_socket, (struct sockaddr *)&main_socket_info,sizeof(main_socket_info)) < 0)   //именование сокета
    {
        std::cout<<"Socket bind error"<<std::endl;
        return 1;
    }
    listen(main_socket,size_of_connection_queue);     //прослушивание серверного сокета

    this->handle_epoll_threads = new Thread_handle[this->number_of_threads];        //массив, хранящий данные о потоках
    epoll_event event;
    event.events = EPOLLIN;
    for (unsigned int i = 0 ; i < this->number_of_threads ; ++i)
    {
        handle_epoll_threads[i].epoll_fd = epoll_create1(0);      //создание экземпляра epoll для каждого потка
        handle_epoll_threads[i].number = i;
        if (handle_epoll_threads[i].epoll_fd == -1)
        {
            std::cout<<"Epoll_create error"<<std::endl;
            return 1;
        }
        handle_epoll_threads[i].usage = 0;
        event.data.fd = shut_threads_fd;
        if(epoll_ctl(handle_epoll_threads[i].epoll_fd,EPOLL_CTL_ADD,shut_threads_fd,&event) == -1)      //добавление off дескриптора
        {
            std::cout<<"Epoll_ctl shut_fd error "<<errno<<std::endl;
            return 1;
        }
    }
    event.data.fd = main_socket;
    if(epoll_ctl(handle_epoll_threads[0].epoll_fd,EPOLL_CTL_ADD,main_socket,&event) == -1)      //добавление серверного сокета в epoll 0/1 потока
    {
        std::cout<<"Epoll_ctl main_fd error "<<errno<<std::endl;
        return 1;
    }
    handle_epoll_threads[0].usage = 500;
    Worker = new Server_Work(commands,number_of_threads);
    return 0;
}

void Server_Handle::handle_thread(int current_epoll_fd,unsigned int thread_number)
{
    epoll_event *events = new epoll_event[max_events_count];
    epoll_event event;
    event.events = EPOLLIN;     //событие-ввод
    unsigned char minimal_use_number;
    int bytes_read;
    unsigned int event_count;
    sockaddr accept_info;
    linger so_linger;
    so_linger.l_onoff = 1;
    so_linger.l_linger = 0;
    unsigned int addrlen = sizeof(accept_info);
    int accept_socket;
    Command cmd;
    char command_massive[cmd.get_command_size()];
    int command_massive_size = sizeof(command_massive);
    while (true)
    {
        event_count= epoll_wait(current_epoll_fd,events,this->max_events_count,-1);
        for(unsigned int i = 0; i<event_count; ++i)
        {
            cmd.clear();
            memset(command_massive,' ',command_massive_size);
            if (events[i].data.fd == this->main_socket)     //событие на серверном сокете
            {
                accept_socket = accept(this->main_socket,&accept_info,&addrlen);
                if (accept_socket < 0)          //очередь пуста
                {
                    continue;
                }
                setsockopt(accept_socket,SOL_SOCKET,SO_LINGER,(char*)&so_linger,sizeof(so_linger));     //добавление параметра SO_LINGER сокету
                minimal_use_number = this->get_minimal_use_thread_number();
                event.data.fd = accept_socket;
                if(epoll_ctl(this->handle_epoll_threads[minimal_use_number].epoll_fd,EPOLL_CTL_ADD,accept_socket,&event) == -1)
                {
                    this->safe_cout.lock();
                    std::cout<<"Epoll_ctl_ADD error "<<errno<<std::endl;
                    this->safe_cout.unlock();
                    continue;
                }
                this->handle_epoll_threads[minimal_use_number].usage++;
                cmd.set_from(accept_socket);
                cmd.set_action(make_new_user);
                this->commands->push(cmd);
                continue;
            }
            if (events[i].data.fd == this->shut_threads_fd)      //выключение потока
            {
                this->safe_cout.lock();
                std::cout<<"Handle thread "<<thread_number<<" is turned off  a:"<<std::endl;
                this->safe_cout.unlock();
                return;
            }

            bytes_read = recv(events[i].data.fd,&command_massive,command_massive_size,0);
            if( bytes_read < 1 )      //пользователь вышел из чата
            {
                close(events[i].data.fd);
                cmd.set_from(events[i].data.fd);
                cmd.set_action(delete_user);
                this->commands->push(cmd);
                this->handle_epoll_threads[thread_number].usage--;

            }
            else if (bytes_read == command_massive_size)          //обработка всех команд, кроме создания пользователя и отключения от сервера
            {
                cmd.set_command(command_massive,events[i].data.fd);
                if (cmd.get_action() < 0)        //защита
                {
                    continue;
                }
                this->commands->push(cmd);
            }
        }
    }
}

unsigned char Server_Handle::get_minimal_use_thread_number()
{
    unsigned int min_use = this->handle_epoll_threads[0].usage;
    unsigned char min_use_number = 0;
    for(unsigned int i = 1; i < this->number_of_threads; ++i)
    {
        if (this->handle_epoll_threads[i].usage < min_use)
        {
            min_use_number = i;
            min_use = this->handle_epoll_threads[i].usage;
        }
    }
    return min_use_number;
}
bool Server_Handle::Start()        //рабочая часть сервера
{

    Worker->Start_Work();       //запуск обработчика

    for (unsigned int i = 0 ; i < this->number_of_threads ; ++i)           //запуск потоков с epoll
    {
        handle_epoll_threads[i].current_thread = std::thread (&Server_Handle::handle_thread,this,handle_epoll_threads[i].epoll_fd,handle_epoll_threads[i].number);
        handle_epoll_threads[i].current_thread.detach();
    }
    std::cout<<"Server started! "<<std::endl;
    std::string current_move;

    while(true)
    {
        std::cin >> current_move;
    }

	return 0;

}



