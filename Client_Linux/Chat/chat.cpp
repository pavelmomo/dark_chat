#include "chat.h"
#include "ui_chat.h"

Chat::Chat(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::Chat)
{
    ui->setupUi(this);
}
void Chat::select_window(QMainWindow* pos_window)
{
    this->setGeometry(pos_window->geometry());
    this->show();
    pos_window->close();
}
Chat::~Chat()
{
    delete ui;
}

