#include "menu.h"
#include "ui_menu.h"

Menu::Menu(QWidget *parent)
    : QMainWindow(parent)
{
    ui = new  Ui::Menu;
    ui->setupUi(this);
    //this->setGeometry(QStyle::alignedRect(Qt::LeftToRight,Qt::AlignCenter,this->size(),qApp->desktop()->availableGeometry()));
    ui->exit_session->close();
    QObject::connect(this->ui->friends_list,SIGNAL(released()),this,SLOT(friends_list_button_clicked_slot()));
    QObject::connect(this->ui->friends_requests,SIGNAL(released()),this,SLOT(get_friends_requests()));
    QObject::connect(this->ui->leave_from_account,SIGNAL(released()),this,SLOT(leave_from_account()));
    QObject::connect(this->ui->make_friend_request,SIGNAL(released()),this,SLOT(make_friend_request()));
    QObject::connect(this->ui->send_button,SIGNAL(released()),this,SLOT(pre_send_message()));
    QObject::connect(this->ui->send_pole,SIGNAL(returnPressed()),this,SLOT(pre_send_message()));
    QObject::connect(this->ui->exit_session,SIGNAL(released()),this,SLOT(set_menu_mode()));

}
Ui::Menu* Menu::get_ui()
{
    return ui;
}
void Menu::set_position(QMainWindow* pos_window)
{
    int xp1,xp2,yp1,yp2;
    pos_window->geometry().getCoords(&xp1,&yp1,&xp2,&yp2);
    setGeometry(xp1-150,yp1-150,this->geometry().width(),this->geometry().height());
}
void Menu::get_position(QMainWindow* pos_window)
{
    int xp1,xp2,yp1,yp2;
    this->geometry().getCoords(&xp1,&yp1,&xp2,&yp2);
    pos_window->setGeometry(xp1+150,yp1+150,pos_window->geometry().width(),pos_window->geometry().height());
}
void Menu::print_message(QString str)
{
    this->ui->text->QTextEdit::append(str);
}
void Menu::set_menu_mode()
{
    ui->text->clear();
    ui->send_pole->clear();
    ui->send_button->setEnabled(0);
    ui->send_pole->setEnabled(0);
    ui->text_pole->setEnabled(0);
    ui->exit_session->close();
    ui->friends_list->setEnabled(1);
    ui->make_friend_request->setEnabled(1);
    ui->friends_requests->setEnabled(1);
    ui->leave_from_account->show();
}

void Menu::pre_send_message()
{
    QString s = this->ui->send_pole->toPlainText();
    this->ui->send_pole->clear();
    emit connect_interface_signal(User_moves::send_message,s);
}

void Menu::get_friends_requests()
{
    emit connect_interface_signal(User_moves::get_friends_requests);
}

void Menu::leave_from_account()
{
    this->close();
    emit connect_interface_signal(User_moves::leave_from_account);
}

void Menu::make_friend_request()
{
    emit connect_interface_signal(User_moves::make_friend_request);
}

void Menu::set_dialogue_mode()
{
    ui->send_button->setEnabled(1);
    ui->send_pole->setEnabled(1);
    ui->text_pole->setEnabled(1);
    ui->text->setText("*************** Добро пожаловать в чат! ***************\n");
    ui->exit_session->show();
    ui->leave_from_account->close();
    ui->friends_list->setEnabled(0);
    ui->make_friend_request->setEnabled(0);
    ui->friends_requests->setEnabled(0);

}

void Menu::menu_interface_for_connect_slot(char action, QMainWindow *window,QString data)
{
    switch (action)
    {
        case authentication_successful:
            set_position(window);
            show();
            break;
        case session_accepted:
            set_dialogue_mode();
            break;
        case new_message:
            print_message(data);
            break;
        case dialog_closed:
        case session_refused:
        case Server_responses::session_establishment_error:
            set_menu_mode();
            break;
    }

}

void Menu::friends_list_button_clicked_slot()
{
    emit connect_interface_signal(User_moves::get_friends_list);
}
Menu::~Menu()
{
    delete ui;
}

