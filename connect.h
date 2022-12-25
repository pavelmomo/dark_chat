#ifndef CONNECT_H
#define CONNECT_H
#include <QMainWindow>
#include <QBoxLayout>
#include <QButtonGroup>
#include <vector>
#include <QMessageBox>
#include <QStyle>
#include <QTimer>
#include <QThread>
#include <QDesktopWidget>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <string>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <thread>
#include <cstdlib>
#include <iostream>
#include <arpa/inet.h>

class listener;
class Dialog_window_chats;

QT_BEGIN_NAMESPACE
namespace Ui { class Connect; }
QT_END_NAMESPACE

class Connect : public QMainWindow
{
    Q_OBJECT

public:
    Connect(QWidget *parent = nullptr);
    listener* obj_listen;
    void select_window(QMainWindow* pos_window);
    void no_connect();
    void start();
    bool set_up_client();
    Dialog_window_chats* dialog_window;
    ~Connect();



private:
    Ui::Connect *ui;
    QTimer *timer;
    QMessageBox *msg;

    QMovie* gif_animation;
    QThread *thread_listen;
    int connect_cnt;
    int main_socket;
    struct sockaddr_in addr;

private slots:
    void timer_connect();
    void button_pressed();
    void onApplied(int id);
public slots:
    void user_list();
    void quit_chat();
    void get_in_chat();
    void send_msg_to_server(QString s);
signals:
    void connect_successful();
    void set_chat_mode();

};


class Dialog_window_chats : public QDialog {
    Q_OBJECT

public:
    Dialog_window_chats( QWidget* parent = nullptr);
    void set_buttons(std::vector <QString> chat_names);
    ~Dialog_window_chats();

private:
    QButtonGroup* btns;
    QLabel* label;
    QBoxLayout* layout;

private slots:
    void btn_pressed(QAbstractButton* btn);
signals:
    void applied(int id);
};

class listener: public QObject
{
    Q_OBJECT
public:
    int fd;
    listener(int sock);
public slots:
    void listen_server();


signals:
    void heard_message(QString);

};

class Command
{public:
    //0-создание user,1-регистрация,2-авторизация,3-вывести список активных п.
    int action = 0;
    int from = 0;
    char data[500];

};
#endif // MENU_H
