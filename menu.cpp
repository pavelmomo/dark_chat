#include "menu.h"
#include "ui_menu.h"

Menu::Menu(QWidget *parent)
    : QMainWindow(parent)
{
    ui = new  Ui::Menu;
    ui->setupUi(this);
    this->setGeometry(QStyle::alignedRect(Qt::LeftToRight,Qt::AlignCenter,this->size(),qApp->desktop()->availableGeometry()));
    this->ui->quit_chat->close();
    QObject::connect(this->ui->active_users,SIGNAL(released()),this,SIGNAL(get_user_list()));
    QObject::connect(this->ui->get_chat_button,SIGNAL(released()),this,SIGNAL(get_chat()));
    QObject::connect(this->ui->send_button,SIGNAL(released()),this,SLOT(pre_send_message()));
    QObject::connect(this->ui->quit_chat,SIGNAL(released()),this,SLOT(exit_chat()));

}
void Menu::select_window(QMainWindow* pos_window)
{
    this->setGeometry(pos_window->geometry());
    this->show();
    pos_window->close();
}
void Menu::print_message(QString str)
{
    this->ui->text->QTextEdit::append(str);
}
void Menu::exit_chat()
{
    this->ui->text->clear();
    this->ui->send_pole->clear();
    this->ui->send_button->setEnabled(0);
    this->ui->send_pole->setEnabled(0);
    this->ui->text_pole->setEnabled(0);
    this->ui->get_chat_button->show();
    this->ui->quit_chat->close();
    this->ui->active_users->setEnabled(1);
    emit quit_from_chat();
}

void Menu::pre_send_message()
{
    QString s = this->ui->send_pole->toPlainText();
    this->ui->send_pole->clear();
    emit send_message(s);
}
void Menu::set_chat_dialogue_mode()
{
    this->ui->send_button->setEnabled(1);
    this->ui->active_users->setEnabled(0);
    this->ui->send_pole->setEnabled(1);
    this->ui->text_pole->setEnabled(1);
    this->ui->get_chat_button->close();
    this->ui->quit_chat->show();
    this->ui->text->setText("**************Добро пожаловать в чат!*************\n");
}

void Menu::connect_slot()
{
    this->show();
}
Menu::~Menu()
{
    delete ui;
}

