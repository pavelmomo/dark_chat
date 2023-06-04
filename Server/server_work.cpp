#include "server_work.h"


Server_Work::Server_Work(queue_safe <Command> *commands,unsigned int work_threads_num)
{
    this->users_online = new std::map<int,User>;
    this->login_to_socket = new std::map <std::string,int>;
    this->work_threads_num = work_threads_num;
    this->commands = commands;
    this->user_worker = new User_work(users_online,commands,&decode_mutex,login_to_socket);
    this->max_q_size = 0;
}
Server_Work::~Server_Work()
{
    for(auto i = users_online->begin(); i != users_online->end(); ++i)
    {
        close((*i).second.get_usr_socket());
    }
    this->users_online->clear();
}
void Server_Work::Start_Work ()
{
    for(int i = 0; i < work_threads_num;++i)
    {
        std::thread thr(&Server_Work::work_thread,this);
        thr.detach();
    }
    return;
}

void Server_Work::work_thread()
{
    int prev = 0;
    Command cmd;
    while(1)
    {
        cmd = commands->pop_with_check_and_wait();

        if (cmd.get_action() < 0)
        {
            internal_decoding(cmd);
        }
        else
        {
            user_worker->user_decoding(cmd);
        }

        cmd.clear();
        //if(users_online->size() % 1000 == 0 && prev != users_online->size())
        //{
           // std::cout<<"Users: "<<users_online->size()<<" Queue: "<<commands->size()<<" Max_queue: "<<max_q_size<<std::endl;
            //prev = users_online->size();
        //}

    }
}
void Server_Work::internal_decoding(Command internal_command)
{
    switch (internal_command.get_action())
    {
        case make_new_user :     //создание нового пользователя
        {
            User new_user(internal_command.get_from());
            decode_mutex.lock();
            users_online->emplace(std::make_pair(internal_command.get_from(),new_user));
            decode_mutex.unlock();
            //std::cout<<"Пользователь добавлен!"<<std::endl;
            break;
        }
        case delete_user :     //удаление пользователя
        {
            std::map <int,User> ::iterator it;
            decode_mutex.lock();
            it = users_online->find(internal_command.get_from());
            if (it == users_online->end())
            {
                decode_mutex.unlock();
                return;
            }
            if (it->second.get_authentication_status() == 1 && login_to_socket->find(it->second.get_login()) != login_to_socket->end())
            {
                login_to_socket->erase(it->second.get_login());
            }

            if (it->second.get_status() == in_dialogue )
            {
                std::map<std::string,int>::iterator correspondent_login_iter = login_to_socket->find(it->second.get_correspondent_login());
                if (correspondent_login_iter != login_to_socket->end())
                {
                    std::map <int,User> ::iterator correspondent_iter = users_online->find(correspondent_login_iter->second);
                    if (correspondent_iter != users_online->end())
                    {
                        correspondent_iter->second.change_to_menu_mode();
                        Command cmd;
                        cmd.clear();
                        cmd.set_action(dialog_closed);
                        char command_mas [cmd.get_command_size()] = {32};
                        cmd.transform_command(command_mas);
                        send(correspondent_iter->first,&command_mas,cmd.get_command_size(),MSG_NOSIGNAL);

                    }
                }

            }

            users_online->erase(it);
            decode_mutex.unlock();
            //std::cout<<"Пользователь удалён!"<<std::endl;
            break;
        }
    }
}



