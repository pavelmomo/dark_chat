#include <iostream>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string>
#include <unistd.h>
#include <stdio.h>
#include <vector>
#include <string.h>
#include <string>
#include <thread>
#include <ctime>
#include <signal.h>
#include <cstdlib>
#include <arpa/inet.h>
#include "command.h"
using namespace std;
#define SERVER_IP "127.0.0.1"



/*
void stress_test()
{
    std::time_t result = std::time(nullptr);
    auto start = std::chrono::system_clock::now();

    //unsigned int start_point = result;
    vector <Client> clients;
    for (int i = 0;i<2001;++i)
    {
        Client buf;
        buf.conn_to_server();
        if(buf.conn_to_server())
        {
            cout<<"Ошибка подключения!";
            break;
        }
        //char name[10];
        //sprintf(name,"%d",i);
        //buf.set_login(name);
        //buf.get_in_room();
        clients.push_back(buf);
        //cout<<i<<endl;

    }
    cout<<"Все клиенты подключены! Начинается установка логина"<<endl;

    for (int i = 0;i<2001;++i)
    {

        std::string login = std::to_string(i);
        clients[i].set_login(login);

    }
    cout<<"Логины установлены, вход в комнату"<<endl;
    //sleep(1);

    for (int i = 0;i<2001;++i)
    {

        clients[i].get_in_room();

    }

    for (int i = 0;i<2001;++i)
    {

        close(clients[i].main_socket);

    }
    cout<<"Вход в комнату осущствлён!"<<endl;
    auto end = std::chrono::system_clock::now();
    auto diff = std::chrono::duration_cast<std::chrono::seconds>(end - start).count();
    std::cout<<"Время работы :"<<diff<<std::endl;

}*/



enum User_moves
{
    authentication = 1,
    registration,
    show_friends

};
class Client
{public:
    int main_socket;
    struct sockaddr_in addr;
    bool is_authorized = 0;
    Client()
    {
        this->set_up_client();
    }
    void set_up_client()
    {
        this->main_socket = socket(AF_INET,SOCK_STREAM,0);
        if (main_socket < 0)
        {
            cout<<"Socket make error"<<endl;
        }
        addr.sin_family = AF_INET;
        addr.sin_port = htons(9999);
        //addr.sin_addr.s_addr = INADDR_ANY;
        addr.sin_addr.s_addr = inet_addr(SERVER_IP);
    }
    bool conn_to_server()
    {

        if(connect(this->main_socket,(sockaddr*)&addr,sizeof(this->addr)) < 0 )
        {
            cout<<"Connect error"<<endl;
            return 0;
        }
        return 0;
    }

    void send_command(Command &command)
    {
        char command_mas [command.get_command_size()] = {32};
        command.transform_command(command_mas);
        send(this->main_socket,&command_mas,sizeof(command_mas),MSG_NOSIGNAL);
    }

    void recv_command(Command &command)
    {
        int bytes_read;
        command.clear();
        char command_mas [command.get_command_size()] = {32};
        bytes_read = recv(main_socket,&command_mas,sizeof(command_mas),0);
        command.set_command(command_mas,main_socket);
    }
    void get_user_list()        //
    {
       /* Command cmd;
        cmd.action = show_active_users;
        send(this->main_socket,&cmd,sizeof(cmd),NULL);
        recv(this->main_socket,&cmd,sizeof(cmd),NULL);
        cout<<"Сервер:\n"<<cmd.data<<endl;*/
    }
    void switch_authorized(int mov)
    {
        switch (mov)
            {
                case 0:
                    cout<<"Действия:\n2 - получить список друзей\n3 - авторизация\n4 - регистрация\n5 - выход"<<endl;
                    break;
                case 2:
                    get_friends_list();
                    break;
                case 3:
                    authorization();
                    break;
                case 4:
                    //A.get_rooms_list();
                    break;

            }

    }
    void switch_non_authorized(int mov)
    {
        switch (mov)
            {
                case 0:
                    cout<<"Действия:\n3 - авторизация\n4 - регистрация\n5 - выход"<<endl;
                    break;
                case 3:
                    authorization();
                    break;
                case 4:
                    //A.get_rooms_list();
                    break;

            }

    }
    void authorization()        //
    {
        Command cmd;
        cmd.set_action(authentication);
        string login = "";
        string password = "";
        cout<<"Введите Ваш логин:"<<endl;
        cin>>login;
        cout<<"Введите Ваш пароль:"<<endl;
        cin>>password;
        if(login.size() > 15 || password.size() > 15)
        {
            cout<<"Некорректный ввод."<<endl;
            return;
        }
        while(login.size() < 15)
        {
            login += " ";
        }
        while(password.size() < 15)
        {
            password += " ";
        }
        std::cout<<login.size()<<" ."<<login<<".  ."<<password<<". "<<password.size()<<std::endl;
        cmd.set_data(login + password);
        send_command(cmd);
        recv_command(cmd);
        if (cmd.get_action())
        {
            cout<<"Авторизация прошла успешно!!!"<<endl;
            is_authorized = 1;
        }
        else
        {
            cout<<"Ошибка авторизации!!"<<endl;
        }

    }

    void get_friends_list()        //
    {
        Command cmd;
        cmd.set_action(show_friends);
        send_command(cmd);
        recv_command(cmd);
        if (cmd.get_action())
        {
            cout<<cmd.get_data()<<endl;
        }
        else
        {
            cout<<"Ошибка!!"<<endl;
        }

    }
    void cli()
    {
        int mov;
        while(1)
        {
            cout<<"Введите действие (0 - справка): "<<endl;
            cin>>mov;
            if(mov == 5)
            {
                break;
            }

            if (is_authorized)
            {
                switch_authorized(mov);
            }
            else
            {
                switch_non_authorized(mov);
            }
        }

    }

    class server_listen
    {public:
        int fd;
        server_listen(int serv)
        {
            this->fd = serv;

        }
        void listen()
        {
            Command cm;
            while(1)
            {

                recv(fd,&cm,sizeof(cm),NULL);
                if(cm.get_action() == 7)
                {
                    break;
                }
                cout<<cm.get_data()<<endl;
            }
        }
    };
};

int main()
{
    Client A;
    int mov;
    if (!A.conn_to_server())
    {
        cout<<"Подключение к серверу успешно выполнено!"<<endl;
        A.cli();


    }

    return 0;
}
