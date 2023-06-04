#include "database_handle.h"

Database_Handle::Database_Handle()
{
    try
    {
        Connection = new pqxx::connection("dbname = " + db_name + " user = "
        + db_user + " password = " + db_password + " hostaddr = " + db_ip + " port = " + db_port);
        if (Connection->is_open())
        {
            std::cout<<"Database opened successfully: " << Connection->dbname() << std::endl;
            is_active = 1;
        }
        else
        {
            std::cout<<"Can not open database!" << std::endl;
            is_active = 0;
        }
    }
    catch(std::exception)
    {
        std::cout<<"Error in database connect!" << std::endl;
        is_active = 0;
    }

}
Database_Handle::~Database_Handle()
{
    Connection->disconnect();
    delete Connection;

}

bool Database_Handle::Nontransaction_Request(std::string request, pqxx::result &result_of_request)
{
    db_mutex.lock();
    bool stat = 0;
    try
    {
        pqxx::nontransaction Transaction_object(*Connection);
        result_of_request = Transaction_object.exec(request);
        Transaction_object.commit();
        stat = 1;
    }
    catch(std::exception)
    {
        std::cout<<"Error in Nontransaction_Request!" << std::endl;
    }
    db_mutex.unlock();
    return stat;

}

bool Database_Handle::Transaction_Request(std::string request, pqxx::result &result_of_request)
{
    db_mutex.lock();
    bool stat = 0;
    try
    {
        pqxx::work Transaction_object(*Connection);
        result_of_request = Transaction_object.exec(request);
        Transaction_object.commit();
        stat = 1;
    }
    catch(std::exception)
    {
        std::cout<<"Error in Transaction_Request!" << std::endl;
    }
    db_mutex.unlock();
    return stat;

}

bool Database_Handle::Search_login(std::string login, pqxx::result &result_of_request)
{
    std::string sql_code = "SELECT * FROM \"Users\" WHERE \"Login\" = '" + login + "'";
    return Nontransaction_Request(sql_code,result_of_request);

}

bool Database_Handle::Get_Friendship_Requests(int id, pqxx::result &result_of_request)
{
    std::string sql_code = "SELECT \"Get_friendship_requests\"(" + std::to_string(id) + ")";
    return Nontransaction_Request(sql_code,result_of_request);

}

bool Database_Handle::Accept_Friendship(std::string friendship_sender_login, int friendship_reciever_id, pqxx::result &result_of_request)
{
    std::string sql_code = "SELECT \"Accept_friendship\"('" + friendship_sender_login + "'," + std::to_string(friendship_reciever_id) + ")";
    return Transaction_Request(sql_code,result_of_request);

}

bool Database_Handle::Discard_Friendship(std::string friendship_sender_login, int friendship_reciever_id, pqxx::result &result_of_request)
{
    std::string sql_code = "SELECT \"Discard_friendship_request\"('" + friendship_sender_login + "'," + std::to_string(friendship_reciever_id) + ")";
    return Transaction_Request(sql_code,result_of_request);

}

bool Database_Handle::Make_Friend_Request( int friendship_sender_id, std::string friendship_reciever_login, pqxx::result &result_of_request)
{
    std::string sql_code = "SELECT \"Make_friendship_request\"(" + std::to_string(friendship_sender_id) + ",'" + friendship_reciever_login + "')";
    return Transaction_Request(sql_code,result_of_request);

}

bool Database_Handle::Get_Friends_List(int id, pqxx::result &result_of_request)
{
    std::string sql_code = "SELECT \"Get_friends_list\"(" + std::to_string(id) + ")";
    return Nontransaction_Request(sql_code,result_of_request);

}

bool Database_Handle::Check_friends(int first_id,int second_id, pqxx::result &result_of_request)
{
    std::string sql_code = "SELECT \"Check_friend\"(" + std::to_string(first_id) + "," + std::to_string(second_id) + ")";
    return Nontransaction_Request(sql_code,result_of_request);

}

bool Database_Handle::Registration(std::string login,std::string password, pqxx::result &result_of_request)
{
    std::string sql_code = "SELECT \"Registration\"('" + login + "','" + password + "')";
    return Transaction_Request(sql_code,result_of_request);

}

