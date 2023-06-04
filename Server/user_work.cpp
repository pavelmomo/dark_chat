#include "user_work.h"


User_work::User_work(std::map <int,User> *users, queue_safe <Command> *commands,std::mutex *data_mutex, std::map <std::string,int> *login_to_socket)
{
    default_access_list = new std::set <char> {1,2,3,4,5,6,7,8,9,10,11,12};
    in_dialogue_access_list = new std::set <char> {6,7};
    in_wait_access_list = new std::set <char> {7};
    db_handler = new Database_Handle;
    this->users = users;
    this->login_to_socket = login_to_socket;
    this->data_mutex = data_mutex;
    this->commands = commands;

}

void User_work::send_command(Command &command,int destination)
{
    char command_mas [command.get_command_size()] = {32};
    command.transform_command(command_mas);
    send(destination,&command_mas,command.get_command_size(),MSG_NOSIGNAL);
}


bool User_work::authorization(Command &command)
{
    data_mutex->lock();
    std::map <int,User> ::iterator user_iterator;      //проверка на наличие пользователя
    user_iterator = users->find(command.get_from());
    if (user_iterator == users->end())
    {
        data_mutex->unlock();
        return 0;
    }
    User current_user = user_iterator->second;
    data_mutex->unlock();

    if (current_user.get_authentication_status() == 0 && command.get_action() > User_moves::registration)     //запрос на обслуживание неавторизованного пользователя
    {
        return 0;
    }
    if(current_user.get_privilege() == 0 && default_access_list->find(command.get_action()) == default_access_list->end())      //недопустимые команды
    {
        return 0;       //недопустимые обычному пользователю команды
    }
    if (current_user.get_status() == in_dialogue && in_dialogue_access_list->find(command.get_action()) == in_dialogue_access_list->end())     //проверка на доступность комманды в режиме диалога
    {
        return 0;
    }
    if (current_user.get_status() == in_wait && in_wait_access_list->find(command.get_action()) == in_wait_access_list->end())     //проверка на доступность комманды в режиме диалога
    {
        return 0;
    }

    return 1;
}


void User_work::authentication(Command &command)
{
    int user_from = command.get_from();
    char *data_mas = command.get_data_massive();
    std::string login = "";
    std::string password = "";

    for (int i = 0; i < 15; ++i)
    {
        login.push_back(data_mas[i]);
        password.push_back(data_mas[i+15]);
    }

    command.clear();
    data_mutex->lock();
    if (login_to_socket->find(login) != login_to_socket->end())
    {
        //std::cout<<"Пользователь уже в сети."<<std::endl;
        data_mutex->unlock();
        command.set_action(authentication_error);
        send_command(command,user_from);
        return;
    }
    data_mutex->unlock();
    pqxx::result Result;
    bool res = db_handler->Search_login(login,Result);
    pqxx::result::iterator iter = Result.begin();

    if (Result.size() > 0 && res == 1 && iter[2].as<std::string>() == password)     //успешная авторизация
    {
        //std::cout<<"Авторизация прошла успешно!"<<std::endl;
        data_mutex->lock();
        std::map <int,User> ::iterator user_iter = upload_user_iter(user_from);
        if (user_iter != users->end())
        {
            user_iter->second.change_authentication_status(true);
            user_iter->second.set_db_data(iter[0].as<int>(),login, iter[3].as<bool>());
            login_to_socket->emplace(std::make_pair(login,user_iter->first));
            data_mutex->unlock();
            command.set_action(authentication_successful);
            send_command(command,user_from);
            return;

        }
        data_mutex->unlock();

    }

    command.set_action(authentication_error);   // ошибка авторизации
    send_command(command,user_from);
}


void User_work::get_friends_list(Command &command)
{
    int user_from = command.get_from();
    pqxx::result Result;
    data_mutex->lock();
    std::map <int,User> ::iterator user_iter = upload_user_iter(user_from);
    if (user_iter == users->end())
    {
        //std::cout<<"Произошла ошибка при отправке списка друзей"<<std::endl;
        data_mutex->unlock();
        command.clear();
        command.set_action(get_friends_list_error);
        send_command(command,user_from);
        return;
    }

    User current_user = user_iter->second;
    data_mutex->unlock();
    bool res = db_handler->Get_Friends_List(current_user.get_db_id(),Result);
    if (res != 0)
    {
        std::string buf = "";
        data_mutex->lock();
        for (pqxx::result::iterator it = Result.begin(); it!= Result.end(); ++it)       //добавление статуса пользователя
        {
            if (login_to_socket->find(it[0].as<std::string>()) != login_to_socket->end())
            {
                buf += "+" + it[0].as<std::string>();
            }
            else
            {
                buf += " " + it[0].as<std::string>();
            }
        }
        data_mutex->unlock();
        command.clear();
        command.set_data(buf);
        command.set_action(get_friends_list_successful);
        send_command(command,user_from);
    }
    else
    {
        command.clear();
        command.set_action(get_friends_list_error);
        send_command(command,user_from);
    }
}

void User_work::get_friends_requests(Command &command)
{
    int user_from = command.get_from();
    pqxx::result Result;
    data_mutex->lock();
    std::map <int,User> ::iterator user_iter = upload_user_iter(user_from);
    if (user_iter == users->end())
    {
        data_mutex->unlock();
        command.clear();
        command.set_action(get_friends_requests_error);
        send_command(command,user_from);
        return;
    }
    User current_user = user_iter->second;
    data_mutex->unlock();
    bool res = db_handler->Get_Friendship_Requests(current_user.get_db_id(),Result);
    if (res != 0)
    {
        std::string buf = "";
        for (pqxx::result::iterator it = Result.begin(); it!= Result.end(); ++it)
        {
            buf += it[0].as<std::string>();
        }
        command.clear();
        command.set_data(buf);
        command.set_action(get_friends_requests_successful);
        send_command(command,user_from);
    }
    else
    {
        command.clear();
        command.set_action(get_friends_requests_error);
        send_command(command,user_from);
    }
}


void User_work::communication_request_processing(Command &command)
{
    int user_from = command.get_from();
    std::string reciever_login = command.get_data();
    while (reciever_login.size() < 15)
    {
        reciever_login += " ";
    }
    command.clear();
    data_mutex->lock();
    std::map <int,User> ::iterator sender_iter = upload_user_iter(user_from);
    if (sender_iter == users->end() )       //проверка отправителя в массиве пользователей
    {
        data_mutex->unlock();
        return;
    }

    std::map <int,User> ::iterator reciever_iter = upload_user_iter(reciever_login);
    if (reciever_iter == users->end() )
    {
        //std::cout<<"Пользователь получатель не находится на сервере"<<std::endl;
        data_mutex->unlock();
        command.set_action(dialog_closed);
        send_command(command,user_from);
        return;
    }

    User sender = sender_iter->second;
    User reciever = reciever_iter->second;
    if (reciever.get_status() != in_menu)       //проверка статуса получателя
    {
        data_mutex->unlock();
        command.set_action(dialog_closed);
        send_command(command,user_from);
        return;
    }
    sender_iter->second.change_to_response_waiting_mode(reciever_login);            ////
    data_mutex->unlock();
    pqxx::result Result;
    bool res = db_handler->Check_friends(sender.get_db_id(),reciever.get_db_id(),Result);       //запрос в бд на проверку дружбы
    pqxx::result::iterator it = Result.begin();
    if(res == 0 || it[0].as<bool>() == false)
    {
        std::cout<<"Получатель не является другом"<<std::endl;
        data_mutex->lock();
        sender_iter->second.change_to_menu_mode();
        data_mutex->unlock();
        command.set_action(session_establishment_error);
        send_command(command,user_from);
        return;
    }
    data_mutex->lock();

    sender_iter = upload_user_iter(sender.get_login());
    if (sender_iter == users->end() )
    {
        data_mutex->unlock();
        return;
    }
    reciever_iter = upload_user_iter(reciever.get_login());
    if (reciever_iter == users->end() )
    {
        sender_iter->second.change_to_menu_mode();
        data_mutex->unlock();
        command.set_action(dialog_closed);
        send_command(command,user_from);
        return;
    }

    data_mutex->unlock();
    command.set_data(sender.get_login());
    command.set_action(communication_request_from_server);
    send_command(command,reciever.get_usr_socket());

}

std::map <int,User>::iterator User_work::upload_user_iter(int fd)
{
    std::map <int,User> ::iterator iter = users->find(fd);
    if (iter == users->end())
    {
        return users->end();
    }
    return iter;
}

std::map <int,User>::iterator User_work::upload_user_iter(std::string login)
{
    std::map<std::string,int>::iterator user_login_iter = login_to_socket->find(login);
    if (user_login_iter == login_to_socket->end())
    {
        return users->end();
    }
    std::map <int,User> ::iterator user_iter = users->find(user_login_iter->second);
    if (user_iter == users->end())
    {
        return users->end();
    }
    return user_iter;
}

void User_work::communication_response_processing(Command &command)
{
    int user_from = command.get_from();
    std::string response_sender_answer = command.get_data();
    command.clear();
    char answer = response_sender_answer[0];
    response_sender_answer.erase(response_sender_answer.begin());
    while (response_sender_answer.size() < 15)
    {
        response_sender_answer += " ";
    }
    data_mutex->lock();
    std::map <int,User> ::iterator sender_iter = upload_user_iter(user_from);
    if (sender_iter == users->end())
    {
        data_mutex->unlock();
        return;
    }
    std::map <int,User> ::iterator reciever_iter = upload_user_iter(response_sender_answer);
    if (reciever_iter == users->end())
    {
        data_mutex->unlock();
        command.set_action(dialog_closed);
        send_command(command,user_from);
        return;
    }
    User sender = sender_iter->second;
    User reciever = reciever_iter->second;

    if (reciever.get_status() != in_wait || reciever.get_correspondent_login() != sender.get_login())
    {
        data_mutex->unlock();
        command.set_action(session_establishment_error);
        send_command(command,user_from);
        return;
    }
    else if (answer == '1')
    {
        sender_iter->second.change_to_dialogue_mode(reciever.get_login());                     ////////////////////////////////////////
        reciever_iter->second.change_to_dialogue_mode();
        data_mutex->unlock();
        command.set_action(session_accepted);
        send_command(command,reciever.get_usr_socket());
        send_command(command,user_from);

    }
    else
    {
        reciever_iter->second.change_to_menu_mode();
        data_mutex->unlock();
        command.set_action(session_refused);
        send_command(command,reciever.get_usr_socket());
    }


}

void User_work::send_message(Command &command)
{
    int user_from = command.get_from();
    data_mutex->lock();
    std::map <int,User> ::iterator user_iter = upload_user_iter(user_from);
    if(user_iter == users->end())
    {
        data_mutex->unlock();
        return;
    }

    if (user_iter->second.get_status() != in_dialogue)
    {
        data_mutex->unlock();
        command.clear();
        command.set_action(0);
        send_command(command,user_from);
        return;
    }
    std::string data = user_iter->second.get_login();
    for (int i = 14; i > -1; --i)
    {
        if (data[i] == ' ')
        {
            data.pop_back();
        }
        else
        {
            break;
        }
    }

    data = data + " : " + command.get_data();
    std::map <int,User> ::iterator corr_iter = upload_user_iter(user_iter->second.get_correspondent_login());
    if (corr_iter == users->end())
    {
        data_mutex->unlock();
        return;
    }
    int corr_fd = corr_iter->second.get_usr_socket();
    data_mutex->unlock();
    command.clear();
    command.set_action(new_message);
    command.set_data(data);
    send_command(command,corr_fd);
    send_command(command,user_from);


}

void User_work::exit_dialog(Command &command)
{
    int user_from = command.get_from();
    command.clear();
    data_mutex->lock();
    std::map <int,User> ::iterator user_iter = upload_user_iter(user_from);
    if(user_iter == users->end())
    {
        data_mutex->unlock();
        return;
    }
    if (user_iter->second.get_status() == in_wait)
    {
        user_iter->second.change_to_menu_mode();
    }
    else if (user_iter->second.get_status() == in_dialogue)
    {
        command.set_action(dialog_closed);
        std::map <int,User> ::iterator corr_iter = upload_user_iter(user_iter->second.get_correspondent_login());
        user_iter->second.change_to_menu_mode();
        if (corr_iter != users->end())
        {
            int correspondent_fd = corr_iter->second.get_usr_socket();
            corr_iter->second.change_to_menu_mode();
            data_mutex->unlock();
            send_command(command,correspondent_fd);
            return;
        }
    }
    data_mutex->unlock();

}


void User_work::accept_friendship(Command &command)
{
    int user_from = command.get_from();
    pqxx::result Result;
    data_mutex->lock();
    std::map <int,User> ::iterator user_iter = upload_user_iter(user_from);

    if (user_iter == users->end())
    {
        data_mutex->unlock();
        return;
    }
    User current_user = user_iter->second;
    data_mutex->unlock();
    bool res = db_handler->Accept_Friendship(command.get_data(),current_user.get_db_id(),Result);
    pqxx::result::iterator it = Result.begin();

    if (Result.size() > 0 && res != 0 && it[0].as<bool>() == true)
    {
        command.clear();
        command.set_action(friendship_accept_successful);
        send_command(command,user_from);
    }
    else
    {
        command.clear();
        command.set_action(friendship_accept_error);
        send_command(command,user_from);
    }

}

void User_work::discard_friendship(Command &command)
{
    int user_from = command.get_from();
    pqxx::result Result;
    data_mutex->lock();
    std::map <int,User> ::iterator user_iter = upload_user_iter(user_from);

    if (user_iter == users->end())
    {
        data_mutex->unlock();
        return;
    }
    User current_user = user_iter->second;
    data_mutex->unlock();
    bool res = db_handler->Discard_Friendship(command.get_data(),current_user.get_db_id(),Result);
    pqxx::result::iterator it = Result.begin();

    if (Result.size() > 0 && res != 0 && it[0].as<bool>() == true)
    {
        command.clear();
        command.set_action(friendship_discard_successful);
        send_command(command,user_from);
    }
    else
    {
        command.clear();
        command.set_action(friendship_discard_error);
        send_command(command,user_from);
    }

}

void User_work::make_friend_request(Command &command)
{
    int user_from = command.get_from();
    pqxx::result Result;
    data_mutex->lock();
    std::map <int,User> ::iterator user_iter = upload_user_iter(user_from);
    if (user_iter == users->end())
    {
        data_mutex->unlock();
        return;
    }
    User current_user = user_iter->second;
    data_mutex->unlock();
    bool res = db_handler->Make_Friend_Request(current_user.get_db_id(),command.get_data(),Result);
    pqxx::result::iterator it = Result.begin();
    if (Result.size() > 0 && res != 0 && it[0].as<bool>() == true)
    {
        command.clear();
        command.set_action(making_friend_request_successful);
        send_command(command,user_from);
    }
    else
    {
        command.clear();
        command.set_action(making_friend_request_error);
        send_command(command,user_from);
    }


}

void User_work::leave_from_account(Command &command)
{
    int user_from = command.get_from();
    data_mutex->lock();
    std::map <int,User> ::iterator user_iter = upload_user_iter(user_from);
    if (user_iter == users->end())
    {
        data_mutex->unlock();
        return;
    }
    if (login_to_socket->find(user_iter->second.get_login()) != login_to_socket->end())
    {
        login_to_socket->erase(user_iter->second.get_login());
    }

    user_iter->second.log_off();
    data_mutex->unlock();

}


void User_work::registration(Command &command)
{
    int user_from = command.get_from();
    char *data_mas = command.get_data_massive();
    data_mutex->lock();
    std::map <int,User> ::iterator user_iter = upload_user_iter(user_from);
    if (user_iter == users->end())
    {
        data_mutex->unlock();
        return;
    }
    if (user_iter->second.get_authentication_status() == true)
    {
        data_mutex->unlock();
        return;
    }
    data_mutex->unlock();
    std::string login = "";
    std::string password = "";
    for (int i = 0; i < 15; ++i)
    {
        login.push_back(data_mas[i]);
        password.push_back(data_mas[i+15]);
    }
    command.clear();
    pqxx::result Result;
    bool res = db_handler->Registration(login,password,Result);
    pqxx::result::iterator iter = Result.begin();
    if (Result.size() > 0 && res == 1 && iter[0].as<bool>() == true)     //успешная регистрация
    {
        command.set_action(registration_successful);
        send_command(command,user_from);
    }
    else
    {
        command.set_action(registration_error);
        send_command(command,user_from);
    }
}
void User_work::user_decoding(Command user_command)
{

    if (authorization(user_command) == 0)
    {
        user_command.set_action(delete_user);
        commands->push(user_command);
        close(user_command.get_from());
        return;
    }

    switch(user_command.get_action())
    {
        case User_moves::leave_from_account :     //выход из диалога
        {
            this->leave_from_account(user_command);
            break;
        }
        case User_moves::registration :     //регистрация нового пользователя
        {
            this->registration(user_command);
            break;
        }
        case User_moves::authentication :     //аутенфикация
        {
            this->authentication(user_command);
            break;
        }
        case User_moves::get_friends_list :     //вывод активных пользователей
        {
            this->get_friends_list(user_command);
            break;
        }
        case User_moves::get_friends_requests :     //вывод активных пользователей
        {
            this->get_friends_requests(user_command);
            break;
        }
        case User_moves::accept_friendship :     //отправка сообщения
        {
            this->accept_friendship(user_command);
            break;
        }
        case User_moves::discard_friendship :     //отправка сообщения
        {
            this->discard_friendship(user_command);
            break;
        }
        case User_moves::communication_request :     //запрос на переписку
        {
            this->communication_request_processing(user_command);
            break;
        }
        case User_moves::communication_response :     //ответ на запрос переписки
        {
            this->communication_response_processing(user_command);
            break;
        }
        case User_moves::send_message :     //отправка сообщения
        {
            this->send_message(user_command);
            break;
        }
        case User_moves::make_friend_request :     //отправка сообщения
        {
            this->make_friend_request(user_command);
            break;
        }
        case User_moves::exit_dialog :     //выход из диалога
        {
            this->exit_dialog(user_command);
            break;
        }

    }
}
