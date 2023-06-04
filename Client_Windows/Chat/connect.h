#ifndef CONNECT_H
#define CONNECT_H
#include <QMainWindow>
#include <QString>
#include <QBoxLayout>
#include <QList>
#include <QtNetwork>
#include <QButtonGroup>
#include <QAbstractButton>
#include <QPushButton>
#include <vector>
#include <QMessageBox>
#include <QStyle>
#include <QTimer>
#include <QLineEdit>
#include <QThread>
//#include <QDesktopWidget>

#include <string>
#include <iostream>
#include "command.h"
#include "ui_menu.h"
#include "menu.h"



class Dialog_window;

QT_BEGIN_NAMESPACE
namespace Ui { class Connect; }
QT_END_NAMESPACE

class Connect : public QMainWindow
{
    Q_OBJECT

public:
    Connect(Menu *menu_window, std::string server_ip, int port,QWidget *parent = nullptr);
    void start();
    ~Connect();
private:
    Dialog_window* dialog_window;
    QString buffer;
    Menu *menu_window;
    Ui::Connect *ui;
    QTimer *timer_conn;
    QMovie *gif_animation;      //анимация при подключении
    int connect_cnt;
    QTcpSocket* main_socket ;
    void make_all_slot_connections();
    void first_connection_failed();
    void select_window(QMainWindow* pos_window);
    void friends_list_show(QString data);
    void friends_requests_show(QString data);
    void communication_request_from_server(QString data);
    void decoding_server_response(Command cmd);
    bool recv_command(Command &command);
    void send_command(Command &command);

private slots:
    void timer_connect();
    void server_connection_error_handler();
    void register_slot();
    void send_auth_data_slot();
    void certain_btn_pressed_slot(int id);
    void server_listener();

public slots:

    void connect_interface_for_menu_slot(char action,QString data = "");
    void exit_session_slot();

signals:
    void menu_interface_signal(char action, QMainWindow *window = nullptr,QString data = "");

};

class Dialog_window : public QDialog {
    Q_OBJECT

public:
    Dialog_window( QWidget* parent);

    int get_state();
    void set_buttons_for_friends_list(std::vector <QString> &friend_logins, std::vector <bool> &friend_status);
    void set_buttons_for_friends_requests(std::vector <QString> &friend_logins);
    void set_buttons_for_communication_request_from_server(QString &user_login);
    void set_buttons_for_friend_request_answer(QString &user_login);
    void show_error(QString error);
    void set_text(QString text);
    std::string get_friend_login();
    void set_make_request_mode();
    void clear_buttons();
    QButtonGroup* get_buttons();
    ~Dialog_window();

private:
    QButtonGroup* btns = nullptr;
    QList <QPushButton*> button_list;
    Connect* connector;
    QLabel* label;
    QBoxLayout* layout = nullptr;
    QLineEdit* friend_login_pole = nullptr;
    int state = 0;      //0 - список друзей, 1 - список запросов на дружбы


private slots:
    void btn_pressed(QAbstractButton* btn);
signals:
    void certain_btn_pressed_signal(int id);
};

#endif // MENU_H
