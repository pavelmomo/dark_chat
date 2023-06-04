#ifndef COMMAND_H_INCLUDED
#define COMMAND_H_INCLUDED
#include<iostream>
#include "string.h"

class Command
{public:

    bool set_command(char *command_mas, int recv_from);
    bool transform_command(char *output_mas);
    void set_action(char action);
    char get_action();
    void set_from(int from);
    int get_from();
    void set_data(std::string data);
    std::string get_data();
    char* get_data_massive();
    void clear();
    static unsigned int get_command_size();
private:
    char action = 0;
    int from = 0;
    char data[500] = {32};
    const static unsigned int command_size = 501;

};

enum User_moves
{
    authentication = 1,
    registration,
    get_friends_list,
    communication_request,
    communication_response,
    send_message,
    exit_dialog,
    get_friends_requests,
    accept_friendship,
    discard_friendship,
    make_friend_request,
    leave_from_account

};
enum Server_responses
{
    authentication_successful = 50,
    authentication_error,
    get_friends_list_successful,
    get_friends_list_error,
    get_friends_requests_successful,
    get_friends_requests_error,
    friendship_accept_successful,
    friendship_accept_error,
    friendship_discard_successful,
    friendship_discard_error,
    registration_successful,
    registration_error,
    making_friend_request_successful,
    making_friend_request_error,
    communication_request_from_server,
    session_accepted,
    session_refused,
    session_establishment_error,
    dialog_closed,
    new_message,
    connection_error

};


#endif // COMMAND_H_INCLUDED
