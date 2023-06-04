#ifndef USER_WORK_H_INCLUDED
#define USER_WORK_H_INCLUDED

#include "internal_classes.h"
#include "database_handle.h"
#include "user.h"
#include "command.h"
#include <sys/socket.h>
#include <map>
#include <set>
#include <mutex>
#include <vector>
#include <string>
#include <iostream>

class User_work
{private:

    Database_Handle *db_handler;        //указатель на Database_Handle объект
    std::map <int,User> *users;      //map пользователей, находящихся на сервере (сокет,User)
    std::map <std::string,int> *login_to_socket;             //map пользователей, находящихся на сервере (логин,сокет)
    std::set <char> *default_access_list;       //стандартный список доступных действий
    std::set <char> *in_wait_access_list;       //список доступных действий в состоянии ожидания
    std::set <char> *in_dialogue_access_list;       //список доступных действий в диалоге
    std::mutex *data_mutex;                 //мьютекс, защищающий массивы пользователей
    queue_safe <Command> *commands;         //очередь команд
    std::map <int,User> ::iterator upload_user_iter(int fd);        //получение итератора пользователя с проверкой
    std::map <int,User>::iterator upload_user_iter(std::string login);        //получение итератора пользователя с проверкой
    bool authorization(Command &command);       //проверка полномочий
    void get_friends_list(Command &command);        //получение списка друзей
    void accept_friendship(Command &command);       //принять дружбы
    void discard_friendship(Command &command);      //отклонить дружбу
    void make_friend_request(Command &command);     //отправить запрос дружбы
    void registration(Command &command);        //регистрация
    void leave_from_account(Command &command);      //выход из аккаунта
    void get_friends_requests(Command &command);        //получить список запросов в друзья
    void send_message(Command &command);        //отправить сообщение
    void exit_dialog(Command &command);     //покинуть диалог
    void communication_request_processing(Command &command);        //отправка запроса на переписку
    void communication_response_processing(Command &command);       //отправка ответного запроса на переписку
    void authentication(Command &command);      //вход в аккаунт
    void send_command(Command &command,int destination);        //отправка комманды
public:
    User_work(std::map <int,User> *users, queue_safe <Command> *commands,std::mutex *data_mutex, std::map <std::string,int> *login_to_socket);//конструктор
    void user_decoding(Command user_command);       //обработка команды, главная функция
};


#endif // USER_WORK_H_INCLUDED
