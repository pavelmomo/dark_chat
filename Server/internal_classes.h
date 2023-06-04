
#ifndef INTERNAL_CLASSES_H
#define INTERNAL_CLASSES_H
#include <vector>
#include "queue_safe.h"
#include "command.h"
#include <map>
#include <cstring>
#include <atomic>
#include <iostream>
#include <thread>
#include <unistd.h>
#include <sys/socket.h>
#include <cstring>


class Thread_handle
{public:
    unsigned int number;
    int epoll_fd;
    std::thread current_thread;
    std::atomic <unsigned int> usage;

};

enum Internal_moves
{
    delete_user = -2,
    make_new_user
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
enum User_status_list
{
    in_menu = 0,
    in_wait,
    in_dialogue

};
#endif // INTERNAL_CLASSES_H
