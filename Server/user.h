#ifndef USER_H_INCLUDED
#define USER_H_INCLUDED
#include"internal_classes.h"
#include<string>

class User
{public:
    User(int socket);       //�����������
    int get_usr_socket();       //�������� �����
    void log_off();         //����� �� ��������-������� ������ ��������
    int get_db_id();        //�������� id �� ���� ������
    bool get_privilege();       //�������� ����������
    std::string get_login();        //�������� �����
    bool get_authentication_status();       //�������� ������ ��������������
    void change_authentication_status(bool new_status);     //�������� ������ ��������������
    std::string get_correspondent_login();           //�������� ����� ��������
    void change_to_response_waiting_mode(std::string corr_login);
    void change_to_dialogue_mode(std::string corr_login = "");
    void change_to_menu_mode();
    char get_status();      //�������� ������
    void set_db_data(int db_id, std::string login, bool privilege);     //��������� � ������������ ������ �� ����
private:
    std::string login = "";     //�����
    int db_id = -1;     //id �� ����
    bool is_authenticated = false;      //������ ��������������
    char status = in_menu;      //������ �� �������
    std::string correspondent_login = "";      //����� ��������
    int usr_socket = 0;     //����� ������������
    bool privilege = 0;     //����������
};

#endif // USER_H_INCLUDED
