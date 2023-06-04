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

    Database_Handle *db_handler;        //��������� �� Database_Handle ������
    std::map <int,User> *users;      //map �������������, ����������� �� ������� (�����,User)
    std::map <std::string,int> *login_to_socket;             //map �������������, ����������� �� ������� (�����,�����)
    std::set <char> *default_access_list;       //����������� ������ ��������� ��������
    std::set <char> *in_wait_access_list;       //������ ��������� �������� � ��������� ��������
    std::set <char> *in_dialogue_access_list;       //������ ��������� �������� � �������
    std::mutex *data_mutex;                 //�������, ���������� ������� �������������
    queue_safe <Command> *commands;         //������� ������
    std::map <int,User> ::iterator upload_user_iter(int fd);        //��������� ��������� ������������ � ���������
    std::map <int,User>::iterator upload_user_iter(std::string login);        //��������� ��������� ������������ � ���������
    bool authorization(Command &command);       //�������� ����������
    void get_friends_list(Command &command);        //��������� ������ ������
    void accept_friendship(Command &command);       //������� ������
    void discard_friendship(Command &command);      //��������� ������
    void make_friend_request(Command &command);     //��������� ������ ������
    void registration(Command &command);        //�����������
    void leave_from_account(Command &command);      //����� �� ��������
    void get_friends_requests(Command &command);        //�������� ������ �������� � ������
    void send_message(Command &command);        //��������� ���������
    void exit_dialog(Command &command);     //�������� ������
    void communication_request_processing(Command &command);        //�������� ������� �� ���������
    void communication_response_processing(Command &command);       //�������� ��������� ������� �� ���������
    void authentication(Command &command);      //���� � �������
    void send_command(Command &command,int destination);        //�������� ��������
public:
    User_work(std::map <int,User> *users, queue_safe <Command> *commands,std::mutex *data_mutex, std::map <std::string,int> *login_to_socket);//�����������
    void user_decoding(Command user_command);       //��������� �������, ������� �������
};


#endif // USER_WORK_H_INCLUDED
