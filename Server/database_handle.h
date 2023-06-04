#ifndef DATABASE_HANDLE_H_INCLUDED
#define DATABASE_HANDLE_H_INCLUDED
#include <pqxx/pqxx>
#include <mutex>
#include <iostream>

class Database_Handle
{private:
    static inline std::mutex db_mutex;      //мьютекс для защиты обращения к базе
    std::string db_name = "Dark_Chat";      //имя бд
    std::string db_user = "postgres";       //имя пользователя для подключения
    std::string db_password = "2103";       //пароль
    std::string db_ip = "127.0.0.1";        //IP
    std::string db_port = "5432";           //порт
    bool Nontransaction_Request(std::string request, pqxx::result &result_of_request);      //облегченный вид запроса
    bool Transaction_Request(std::string request, pqxx::result &result_of_request);     //запрос
    pqxx::connection *Connection;       //указатель на объект connection
public:
    Database_Handle();
    ~Database_Handle();
    bool Search_login(std::string login, pqxx::result &result_of_request);      //поиск по логину
    bool Accept_Friendship(std::string friendship_sender_login, int friendship_reciever_id, pqxx::result &result_of_request);       //принять дружбу
    bool Discard_Friendship(std::string friendship_sender_login, int friendship_reciever_id, pqxx::result &result_of_request);      //отклонить дружбу
    bool Registration(std::string login,std::string password, pqxx::result &result_of_request);     //регистрация
    bool Make_Friend_Request( int friendship_sender_id, std::string friendship_reciever_login, pqxx::result &result_of_request);    //запрос дружбы
    bool Get_Friendship_Requests(int id, pqxx::result &result_of_request);      //получить список запросов
    bool Get_Friends_List(int id, pqxx::result &result_of_request);     //получить список друзей
    bool Check_friends(int first_id,int second_id, pqxx::result &result_of_request);        //проверить наличие в друзьях
    bool is_active = 0;     //статус подключения

};

#endif // DATABASE_HANDLE_H_INCLUDED
