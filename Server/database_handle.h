#ifndef DATABASE_HANDLE_H_INCLUDED
#define DATABASE_HANDLE_H_INCLUDED
#include <pqxx/pqxx>
#include <mutex>
#include <iostream>

class Database_Handle
{private:
    static inline std::mutex db_mutex;      //������� ��� ������ ��������� � ����
    std::string db_name = "Dark_Chat";      //��� ��
    std::string db_user = "postgres";       //��� ������������ ��� �����������
    std::string db_password = "2103";       //������
    std::string db_ip = "127.0.0.1";        //IP
    std::string db_port = "5432";           //����
    bool Nontransaction_Request(std::string request, pqxx::result &result_of_request);      //����������� ��� �������
    bool Transaction_Request(std::string request, pqxx::result &result_of_request);     //������
    pqxx::connection *Connection;       //��������� �� ������ connection
public:
    Database_Handle();
    ~Database_Handle();
    bool Search_login(std::string login, pqxx::result &result_of_request);      //����� �� ������
    bool Accept_Friendship(std::string friendship_sender_login, int friendship_reciever_id, pqxx::result &result_of_request);       //������� ������
    bool Discard_Friendship(std::string friendship_sender_login, int friendship_reciever_id, pqxx::result &result_of_request);      //��������� ������
    bool Registration(std::string login,std::string password, pqxx::result &result_of_request);     //�����������
    bool Make_Friend_Request( int friendship_sender_id, std::string friendship_reciever_login, pqxx::result &result_of_request);    //������ ������
    bool Get_Friendship_Requests(int id, pqxx::result &result_of_request);      //�������� ������ ��������
    bool Get_Friends_List(int id, pqxx::result &result_of_request);     //�������� ������ ������
    bool Check_friends(int first_id,int second_id, pqxx::result &result_of_request);        //��������� ������� � �������
    bool is_active = 0;     //������ �����������

};

#endif // DATABASE_HANDLE_H_INCLUDED
