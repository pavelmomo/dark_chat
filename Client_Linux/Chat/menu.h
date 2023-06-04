#ifndef MENU_H
#define MENU_H
#include <QMainWindow>
#include <QStyle>
#include <QDebug>
#include <QDesktopWidget>
#include <QMessageBox>
#include "command.h"

QT_BEGIN_NAMESPACE
namespace Ui { class Menu; }
QT_END_NAMESPACE


class Menu : public QMainWindow
{
    Q_OBJECT

public:
    Menu(QWidget *parent = nullptr);
    Ui::Menu* get_ui();
    void set_position(QMainWindow* pos_window);
    void get_position(QMainWindow* pos_window);
    ~Menu();
private:
    Ui::Menu *ui;
    void print_message(QString);
public slots:
    void menu_interface_for_connect_slot(char action, QMainWindow *window = nullptr,QString data = "");

private slots:
    void pre_send_message();
    void leave_from_account();
    void get_friends_requests();
    void make_friend_request();
    void set_chat_dialogue_mode();
    void set_menu_mode();
    void friends_list_button_clicked_slot();
signals:
    void connect_interface_signal(char action, QString data = "");

};
#endif // MENU_H
