#include "chat.h"
#include "menu.h"
#include "connect.h"
#include <iostream>
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    Connect connect_window;
    Menu menu_window;
    Chat chat_window;
    listener* object_listen = connect_window.obj_listen;
    QObject::connect(&connect_window,SIGNAL(connect_successful()),&menu_window,SLOT(connect_slot()));
    QObject::connect(connect_window.obj_listen,SIGNAL(heard_message(QString)),&menu_window,SLOT(print_message(QString)));
    QObject::connect(&connect_window,SIGNAL(set_chat_mode()),&menu_window,SLOT(set_chat_dialogue_mode()));
    QObject::connect(&menu_window,SIGNAL(get_user_list()),&connect_window,SLOT(user_list()));
    QObject::connect(&menu_window,SIGNAL(send_message(QString)),&connect_window,SLOT(send_msg_to_server(QString)));
    QObject::connect(&menu_window,SIGNAL(get_chat()),&connect_window,SLOT(get_in_chat()));
    QObject::connect(&menu_window,SIGNAL(quit_from_chat()),&connect_window,SLOT(quit_chat()));

    connect_window.start();
    //menu_window.show();






    //chat_window.select_window(&menu_window);

    return a.exec();
}
