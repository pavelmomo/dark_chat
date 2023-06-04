
#ifndef SERVER_HANDLE_H
#define SERVER_HANDLE_H


#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <algorithm>

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/epoll.h>
#include <fcntl.h>

#include <queue>
#include <map>
#include <string>
#include <sys/eventfd.h>

#include <thread>
#include <condition_variable>
#include <mutex>

#include "internal_classes.h"
#include "server_work.h"
#include "queue_safe.h"
#include "command.h"


class Server_Handle
{
private:
    queue_safe <Command> *commands;         //очередь комманд
    Thread_handle *handle_epoll_threads;       //массив с прослушивающими потоками 
    sockaddr_in main_socket_info;       //данные сокета
    unsigned int number_of_threads;     //количество потоков
    unsigned int max_events_count;      //максимальное количество событий
    Server_Work* Worker;        //указатель на Server_Work
    int main_socket;            //основной сокет сервера
    int shut_threads_fd;        //файловый дескриптор для завершения потоков
    static inline std::mutex safe_cout;     //мьютекс для вывода
    void handle_thread(int current_epoll_fd, unsigned int thread_number);       //главная функция, запускается в разных потоках, слушает порт
    unsigned char get_minimal_use_thread_number();      //определяет поток с минимальной нагрузкой
public:
    Server_Handle();
    ~Server_Handle();
    bool Set_up_server(uint16_t port, unsigned int size_of_connection_queue, unsigned int max_events_count,
        unsigned int number_of_threads);//настройка сервера
    bool Start();       //запуск сервера

};

#endif
