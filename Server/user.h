#ifndef USER_H_INCLUDED
#define USER_H_INCLUDED
#include"internal_classes.h"
#include<string>

class User
{public:
    User(int socket);       //конструктор
    int get_usr_socket();       //получить сокет
    void log_off();         //выйти из аккаунта-стереть данные аккаунта
    int get_db_id();        //получить id из базы данных
    bool get_privilege();       //получить привилегии
    std::string get_login();        //получить логин
    bool get_authentication_status();       //получить статус аутентификации
    void change_authentication_status(bool new_status);     //изменить статус аутентификации
    std::string get_correspondent_login();           //получить логин партнера
    void change_to_response_waiting_mode(std::string corr_login);
    void change_to_dialogue_mode(std::string corr_login = "");
    void change_to_menu_mode();
    char get_status();      //получить статус
    void set_db_data(int db_id, std::string login, bool privilege);     //загрузить в пользователя данные из базы
private:
    std::string login = "";     //логин
    int db_id = -1;     //id из базы
    bool is_authenticated = false;      //статус аутентификации
    char status = in_menu;      //статус на сервере
    std::string correspondent_login = "";      //логин партнера
    int usr_socket = 0;     //сокет пользователя
    bool privilege = 0;     //привилегии
};

#endif // USER_H_INCLUDED
