#include "chat.h"
#include "menu.h"
#include "connect.h"
#include <iostream>
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    Menu menu_window;
    Connect connect_window(&menu_window,"127.0.0.1",9999);
    connect_window.start();

    return a.exec();
}
