#include "user.h"


User::User(int socket)
{
    usr_socket = socket;
}

void User::change_authentication_status(bool new_status)
{
    is_authenticated = new_status;
}

bool User::get_authentication_status()
{
    return is_authenticated;
}

bool User::get_privilege()
{
    return privilege;
}

std::string User::get_correspondent_login()
{
    return correspondent_login;
}

void User::set_db_data(int db_id, std::string login, bool privilege)
{
    this->db_id = db_id;
    this->login = login;
    this->privilege = privilege;
}
void User::change_to_response_waiting_mode(std::string corr_login)
{
    correspondent_login = corr_login;
    status = in_wait;
}

void User::change_to_dialogue_mode(std::string corr_login)
{
    if (corr_login != "")
    {
        status = in_dialogue;
        correspondent_login = corr_login;
    }
    else
    {
        status = in_dialogue;
    }

}

void User::change_to_menu_mode()
{
    correspondent_login = "";
    status = in_menu;
}


int User::get_usr_socket()
{
    return usr_socket;
}

void User::log_off()
{
    login = "";
    db_id = -1;
    is_authenticated = false;
    status = in_menu;
    correspondent_login = "";
    privilege = 0;

}

int User::get_db_id()
{
    return db_id;

}

char User::get_status()
{
    return status;

}

std::string User::get_login()
{
    return login;

}
