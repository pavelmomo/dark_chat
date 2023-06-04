#ifndef SERVER_WORK_H
#define SERVER_WORK_H

#include <iostream>
#include <map>
#include <vector>
#include <thread>
#include <condition_variable>
#include <sys/socket.h>
#include "internal_classes.h"
#include "user.h"
#include "queue_safe.h"
#include "user_work.h"
#include "command.h"

class Server_Work
{
private:
    std::map <int,User> *users_online;      //map пользователей, находящихся на сервере (сокет,User)
    std::map <std::string,int> *login_to_socket;        //map пользователей, находящихся на сервере (логин,сокет)
    queue_safe <Command> *commands;     //очередь комманд
    unsigned int work_threads_num;      //количество потоков обработки
    std::atomic<unsigned int> max_q_size;       //максимальная длина очереди
    std::mutex decode_mutex;        //мьютекс, защищающий массивы пользователей
    User_work *user_worker;         //указатель на User_Worker
public:
    Server_Work(queue_safe <Command> *commands,unsigned int work_threads_num);      //конструктор
    ~Server_Work();         //деструктор
    void Start_Work();      //запуск потоков обработки
private:
    void internal_decoding(Command internal_command);       //обработка служебных команд      
    void work_thread();         //основная функция, запускается в разных потоках

};

#endif
