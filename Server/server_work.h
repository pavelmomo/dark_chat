#ifndef SERVER_WORK_H
#define SERVER_WORK_H

#include <iostream>
#include <map>
#include <vector>
#include <thread>
#include <condition_variable>
#include <sys/socket.h>
#include "internal_classes.h"
#include "user.h"
#include "queue_safe.h"
#include "user_work.h"
#include "command.h"

class Server_Work
{
private:
    std::map <int,User> *users_online;      //map �������������, ����������� �� ������� (�����,User)
    std::map <std::string,int> *login_to_socket;        //map �������������, ����������� �� ������� (�����,�����)
    queue_safe <Command> *commands;     //������� �������
    unsigned int work_threads_num;      //���������� ������� ���������
    std::atomic<unsigned int> max_q_size;       //������������ ����� �������
    std::mutex decode_mutex;        //�������, ���������� ������� �������������
    User_work *user_worker;         //��������� �� User_Worker
public:
    Server_Work(queue_safe <Command> *commands,unsigned int work_threads_num);      //�����������
    ~Server_Work();         //����������
    void Start_Work();      //������ ������� ���������
private:
    void internal_decoding(Command internal_command);       //��������� ��������� ������      
    void work_thread();         //�������� �������, ����������� � ������ �������

};

#endif
