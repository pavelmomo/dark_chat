#include <iostream>
#include <sys/types.h>
//#include <sys/socket.h>
//#include <netinet/in.h>
#include <string>
//#include <unistd.h>
//#include <stdio.h>
#include <string.h>
#include <thread>
#include <signal.h>
//#include <cstdlib>
#include <arpa/inet.h>
using namespace std;
#define SERVER_IP "192.168.0.105"
class Command
{public:
    //0-создание user,1-регистрация,2-авторизация,3-вывести список активных п.
    int action = 0;
    int from = 0;
    char data[500];

};

class Client
{public:
    int main_socket;
    struct sockaddr_in addr;
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
        addr.sin_port = htons(1234);
        addr.sin_addr.s_addr = INADDR_ANY;
        //addr.sin_addr.s_addr = inet_addr(SERVER_IP);

    }
    bool conn_to_server()
    {

        if(connect(this->main_socket,(sockaddr*)&addr,sizeof(this->addr)) < 0 )
        {
            cout<<"Error Connect!"<<endl;
            return 1;
        }
        cout<<"Connect!"<<endl;
        return 0;
    }
    void set_login()
    {
        Command cmd;
        cmd.action = 1;
        send(this->main_socket,&cmd,sizeof(cmd),NULL);
        recv(this->main_socket,&cmd,sizeof(cmd),NULL);
        cout<<"Сервер:\n"<<cmd.data<<endl;
    }
    void get_user_list()
    {
        Command cmd;
        cmd.action = 2;
        send(this->main_socket,&cmd,sizeof(cmd),NULL);
        recv(this->main_socket,&cmd,sizeof(cmd),NULL);
        cout<<"Сервер:\n"<<cmd.data<<endl;
    }
    void create_room()
    {
        Command cmd;
        cmd.action = 3;
        cout<<"Введите название новой комнаты: "<<endl;
        scanf("%s",&cmd.data);
        send(this->main_socket,&cmd,sizeof(cmd),NULL);
        recv(this->main_socket,&cmd,sizeof(cmd),NULL);
        cout<<"Сервер:\n"<<cmd.data<<endl;
    }
    void get_rooms_list()
    {
        Command cmd;
        cmd.action = 4;
        send(this->main_socket,&cmd,sizeof(cmd),NULL);
        recv(this->main_socket,&cmd,sizeof(cmd),NULL);
        cout<<"Сервер:\n"<<cmd.data<<endl;
    }
    void get_in_room()
    {
        Command cmd;
        cmd.action = 5;
        cout<<"\nВведите id комнаты: "<<endl;
        scanf("%s",&cmd.data);
        send(this->main_socket,&cmd,sizeof(cmd),NULL);
        recv(this->main_socket,&cmd,sizeof(cmd),NULL);
        cout<<"Сервер:\n"<<cmd.data<<endl;
        if(!cmd.action)
        {
            return;
        }
        server_listen listener(this->main_socket);      //запуск второго потока для приёма сообщений
        thread thr(&server_listen::listen,listener);
        thr.detach();
        //sleep(2);
        system("clear");
        cout<<"Чат (для выхода используйте ^q):"<<endl;
        getchar();      //очистка буфера
        cmd.action = 6;
        char quit[] = "^q\n";
        char join_msg[] = "Присоединился\0";
        strcpy(cmd.data,join_msg);
        send(this->main_socket,&cmd,sizeof(cmd),NULL);      //вывод строки-сигнала
        while(strcmp(quit,cmd.data))
        {
            cmd.data[0] = '\0';
            fgets(cmd.data,500,stdin);
            if (!strncmp(quit,cmd.data,3))      //выход из комнаты
            {
                cmd.action = 7;
            }
            send(this->main_socket,&cmd,sizeof(cmd),NULL);
        }
        system("clear");

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
                if(cm.action == 7)
                {
                    break;
                }
                cout<<cm.data<<endl;
            }
        }
    };
};


