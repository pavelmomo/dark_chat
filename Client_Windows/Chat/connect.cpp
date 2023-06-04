#include "connect.h"
#include "ui_connect.h"
#include <QMovie>

Connect::Connect(Menu *menu_window, std::string server_ip, int port, QWidget *parent)
    : QMainWindow(parent)
{
    ui = new  Ui::Connect;
    this->menu_window = menu_window;
    dialog_window = new Dialog_window(menu_window);
    ui->setupUi(this);
    ui->login_pole->close();
    ui->register_btn->close();
    ui->password_pole->close();
    ui->send_auth_data_btn->close();
    main_socket = new QTcpSocket(this);
    gif_animation = new QMovie(":/img/images/anim.gif");
    ui->anim->setMovie(gif_animation);
    timer_conn = new QTimer();
    timer_conn->setInterval(1000);
    make_all_slot_connections();
    main_socket->connectToHost(server_ip.c_str(),port);


}
void Connect::make_all_slot_connections()
{
    QObject::connect(timer_conn, SIGNAL(timeout()), this, SLOT(timer_connect()));
    QObject::connect(ui->send_auth_data_btn,SIGNAL(released()),this, SLOT(send_auth_data_slot()));
    QObject::connect(ui->register_btn,SIGNAL(released()),this, SLOT(register_slot()));
    QObject::connect(ui->password_pole,SIGNAL(returnPressed()),this, SLOT(send_auth_data_slot()));
    QObject::connect(ui->login_pole,SIGNAL(returnPressed()),this, SLOT(send_auth_data_slot()));


    QObject::connect(menu_window->get_ui()->exit_session,SIGNAL(released()),this,SLOT(exit_session_slot()));
    QObject::connect(main_socket,SIGNAL(readyRead()),this,SLOT(server_listener()));
    QObject::connect(main_socket,SIGNAL(disconnected()),this,SLOT(server_connection_error_handler()));
    QObject::connect(this,SIGNAL(menu_interface_signal(char,QMainWindow*,QString)),menu_window,SLOT(menu_interface_for_connect_slot(char,QMainWindow*,QString)));
    QObject::connect(menu_window,SIGNAL(connect_interface_signal(char, QString)),this,SLOT(connect_interface_for_menu_slot(char, QString)));

    QObject::connect(dialog_window,SIGNAL(certain_btn_pressed_signal(int)), this, SLOT(certain_btn_pressed_slot(int)));         //
}

bool Connect::recv_command(Command &command)
{
    int bytes_read;
    command.clear();
    char command_mas [command.get_command_size()];
    bytes_read = main_socket->read(command_mas,command.get_command_size());
    if (bytes_read != (int)command.get_command_size())
    {
        return 0;
    }
    command.set_command(command_mas,0);
    return 1;
}

void Connect::send_command(Command &command)
{
    char command_mas [command.get_command_size()];
    command.transform_command(command_mas);
    if (main_socket->write(command_mas,sizeof(command_mas)) != command.get_command_size())
    {
        server_connection_error_handler();
    }
}

void Connect::server_listener()
{
    Command cmd;
    if (recv_command(cmd) == 0)
    {
        server_connection_error_handler();
        return;
    }
    decoding_server_response(cmd);

}
void Connect::decoding_server_response(Command cmd)
{
    switch (cmd.get_action())
    {
        case Server_responses::authentication_successful:
        {
            close();
            emit menu_interface_signal(authentication_successful,this);
            break;
        }
        case Server_responses::authentication_error:
        {
            menu_window->set_position(this);
            dialog_window->show_error("Введены неверные данные !");
            ui->login_pole->setText("");
            ui->password_pole->setText("");
            ui->centralwidget->setEnabled(1 );
            break;
        }
        case Server_responses::get_friends_list_successful:
        {
            friends_list_show(QString::fromStdString(cmd.get_data()));
            break;
        }
        case Server_responses::get_friends_requests_successful:
        {
            friends_requests_show(QString::fromStdString(cmd.get_data()));
            break;
        }

        case Server_responses::communication_request_from_server:
        {
            communication_request_from_server(QString::fromStdString(cmd.get_data()));
            break;
        }
        case session_refused:
        {
            dialog_window->show_error("Запрос на сессию был отклонён");
            emit menu_interface_signal(session_refused);
            break;
        }
        case session_accepted:
        {
            emit menu_interface_signal(session_accepted);
            break;
        }
        case dialog_closed:
        {
            dialog_window->show_error("Собеседник покинул диалог");
            emit menu_interface_signal(dialog_closed);
            break;
        }
        case new_message:
        {
            emit menu_interface_signal(new_message,nullptr,QString::fromStdString(cmd.get_data()));
            break;
        }
        case friendship_accept_successful://
        {
            dialog_window->show_error("Пользатель добавлен в друзья");
            break;
        }

        case friendship_discard_successful://
        {
            dialog_window->show_error("Заявка отклонена");
            break;
        }

        case making_friend_request_successful://
        {
            dialog_window->show_error("Заявка отправлена");
            break;
        }
        case making_friend_request_error://
        {
            dialog_window->show_error("Пользователя с указанным логином не существует\n   или он уже находится у Вас в друзьях");
            break;
        }
        case registration_successful://
        {
            menu_window->set_position(this);
            dialog_window->show_error("Регистрация прошла успешно!");
            ui->centralwidget->setEnabled(1);
            register_slot();
            break;
        }
        case registration_error://
        {
            menu_window->set_position(this);
            dialog_window->show_error("Произошла ошибка, возможно данный логин уже занят!");
            ui->centralwidget->setEnabled(1);
            ui->login_pole->clear();
            ui->password_pole->clear();
            break;
        }
        case Server_responses::session_establishment_error:
        {
            dialog_window->show_error("Произошла ошибка соединения");
            emit menu_interface_signal(session_establishment_error);
            break;

        }
        case Server_responses::friendship_accept_error:
        case Server_responses::friendship_discard_error:
        case Server_responses::get_friends_list_error:
        case Server_responses::get_friends_requests_error:
        {
            dialog_window->show_error("Произошла ошибка");
            break;
        }
    }

}
void Connect::select_window(QMainWindow* pos_window)
{
    setGeometry(pos_window->geometry());
    show();
    pos_window->close();
}

void Connect::start()
{
    gif_animation->start();
    timer_conn->start();
    show();
    connect_cnt = 0;
}

void Connect::timer_connect()
{

    if(connect_cnt == 5)      //неудача
    {
        timer_conn->stop();
        first_connection_failed();
    }
    else
    {
        connect_cnt++;
        if (main_socket->state() == QAbstractSocket::ConnectedState)
        {
            timer_conn->stop();
            gif_animation->stop();
            ui->anim->close();
            ui->text_label->setText("Авторизация");
            ui->login_pole->show();
            ui->register_btn->show();
            ui->password_pole->show();
            ui->send_auth_data_btn->show();
            connect_cnt = 0;
        }
    }
}


void Connect::connect_interface_for_menu_slot(char action, QString data)
{
    Command cmd;
    switch (action)
    {
        case User_moves::get_friends_list:
        {
            cmd.set_action(User_moves::get_friends_list);
            send_command(cmd);
            break;
        }
        case User_moves::send_message:
        {
            cmd.set_action(send_message);
            cmd.set_data(data.toStdString());
            send_command(cmd);
            break;
        }
        case User_moves::get_friends_requests:
        {
            cmd.set_action(User_moves::get_friends_requests);
            send_command(cmd);
            break;
        }
        case User_moves::make_friend_request:
        {
            dialog_window->set_make_request_mode();
            dialog_window->show();

            break;
        }
        case User_moves::leave_from_account:
        {
            cmd.set_action(leave_from_account);
            dialog_window->close();
            send_command(cmd);
            menu_window->get_position(this);
            this->show();
            ui->centralwidget->setEnabled(1);
            ui->login_pole->clear();
            ui->password_pole->clear();
            break;
        }

    }
}


void Connect::first_connection_failed()
{                                                               //ошибка подключения
    ui->text_label->setText("Ошибка подключения!");
    gif_animation->stop();
}

void Connect::register_slot()
{
    ui->login_pole->clear();
    ui->password_pole->clear();
    if (ui->register_btn->text() == "Регистрация")
    {
        ui->register_btn->setText("Вход");
        ui->text_label->setText("Регистрация");
        ui->send_auth_data_btn->setText("Зарегистрироваться");
    }
    else if (ui->register_btn->text() == "Вход")
    {
        ui->register_btn->setText("Регистрация");
        ui->text_label->setText("Авторизация");
        ui->send_auth_data_btn->setText("Войти");
    }
}

void Connect::certain_btn_pressed_slot(int id)
{
    Command cmd;
    if (dialog_window->get_state() == 0)        //запрос на переписку
    {
        cmd.set_action(communication_request);
        QString buf = dialog_window->get_buttons()->button(id)->text();
        cmd.set_data(buf.toStdString());
        send_command(cmd);
        menu_window->get_ui()->text->setText("**************** Ожидание ответа ****************\n");
        menu_window->get_ui()->exit_session->show();
        menu_window->get_ui()->leave_from_account->close();
        menu_window->get_ui()->friends_list->setEnabled(0);
        menu_window->get_ui()->make_friend_request->setEnabled(0);
        menu_window->get_ui()->friends_requests->setEnabled(0);

    }
    else if (dialog_window->get_state() == 1)
    {
        buffer = dialog_window->get_buttons()->button(id)->text();
        dialog_window->set_buttons_for_friend_request_answer(buffer);
        dialog_window->show();

    }
    else if (dialog_window->get_state() == 2)
    {
        cmd.set_action(User_moves::make_friend_request);
        cmd.set_data(dialog_window->get_friend_login());
        send_command(cmd);
    }
    else if (dialog_window->get_state() == 3)
    {
        cmd.set_action(communication_response);
        switch (id) {

            case 0:
            {
                buffer = "1" + buffer;
                cmd.set_data(buffer.toStdString());
                send_command(cmd);
                emit connect_interface_for_menu_slot(session_accepted);
                break;
            }
            case 1:
            {
                buffer = "0" + buffer;
                cmd.set_data(buffer.toStdString());
                send_command(cmd);
                break;
            }
        }
    }
    else if (dialog_window->get_state() == 4)
    {
        cmd.set_action(communication_response);
        switch (id) {

            case 0:
            {
                cmd.set_action(accept_friendship);

                cmd.set_data(buffer.toStdString());
                send_command(cmd);
                break;
            }
            case 1:
            {
                cmd.set_action(discard_friendship);
                cmd.set_data(buffer.toStdString());
                send_command(cmd);
                break;
            }
        }
    }
}

void Connect::friends_requests_show(QString data)
{
    if (data.size() == 0)
    {
        dialog_window->show_error("Запросы на дружбу отсутствуют");
        return;
    }
    int empty_points_count = 0;
    std::vector <QString> requests;

    QString buf = "";

    for(int j = 0;j < data.size(); ++j)
    {
        if (data[j] > (QChar)64 && data[j] < (QChar)126 )
        {
            buf += data[j];
        }
        else if (data[j] == ' ')
        {
            if (buf.size() > 0)
            {
                requests.push_back(buf);
                buf = "";
            }
            else
            {
                ++empty_points_count;
                if (empty_points_count > 15)
                {
                    break;
                }
            }
        }
    }

    dialog_window->set_buttons_for_friends_requests(requests);
    dialog_window->show();

}

void Connect::friends_list_show(QString data)
{
    QString buf = "";
    int i = 0;
    int empty_points_count = 0;
    bool flag = 0;
    std::vector <QString> friends;
    std::vector <bool> friend_status;
    if (data.size() == 0)
    {
        dialog_window->show_error("Друзья отсутствуют");
        return;
    }
    while(true)
    {
        for(int j = 0 + 16 * i;j < 16 * (i + 1); ++j)
        {
            if(j == 0 + 16 * i)
            {
                if(data[j] == '+')
                {
                    friend_status.push_back(1);
                }
                else
                {
                    friend_status.push_back(0);
                }
                continue;
            }
            if (data[j] > (QChar)64 && data[j] < (QChar)126 )
            {
                buf += data[j];
            }
            else if (data[j] == ' ')
            {
                ++empty_points_count;
            }
            if(j == data.size() - 1)
            {
                flag = 1;
                break;
            }
        }
        if (empty_points_count > 15)
        {
            break;
        }
        friends.push_back(buf);
        if (flag)
        {
            break;
        }
        ++i;
        empty_points_count = 0;
        buf = "";
    }

    dialog_window->set_buttons_for_friends_list(friends,friend_status);
    dialog_window->show();
}


void Connect::send_auth_data_slot(){     //отправка логина
    Command cmd;
    QString login = ui->login_pole->text();
    QString password = ui->password_pole->text();
    if (ui->send_auth_data_btn->text() == "Войти")
    {
        cmd.set_action(authentication);
    }
    else if (ui->send_auth_data_btn->text() == "Зарегистрироваться")
    {
        cmd.set_action(registration);
        for (int i = 0;i<login.size();++i)
        {
            if (login[i] < (QChar)64 ||  login[i] > (QChar)126)
            {
                dialog_window->show_error("Логин не удовлетворяет требованиям");
                ui->login_pole->clear();
                ui->password_pole->clear();
                return;
            }
        }
        for (int i = 0;i<password.size();++i)
        {
            if (login[i] < (QChar)64 ||  login[i] > (QChar)126)
            {
                dialog_window->show_error("Пароль не удовлетворяет требованиям");
                ui->login_pole->clear();
                ui->password_pole->clear();
                return;
            }
        }
    }

    while(login.size() < 15)
    {
        login += " ";
    }
    while(password.size() < 15)
    {
        password += " ";
    }
    login += password;
    cmd.set_data(login.toStdString());
    send_command(cmd);
    ui->centralwidget->setEnabled(0);
}

void Connect::exit_session_slot()
{
    Command cmd;
    cmd.set_action(exit_dialog);
    send_command(cmd);
}


void Connect::communication_request_from_server(QString data)
{
    Command cmd;
    cmd.set_action(communication_response);
    buffer = data;
    dialog_window->set_buttons_for_communication_request_from_server(buffer);
    dialog_window->show();

}

void Connect::server_connection_error_handler()
{

    main_socket->close();
    menu_window->close();
    QMessageBox msg_box(this);
    msg_box.setText("Потеряно соединение с сервером!\nТребуется перезапуск приложения");
    msg_box.exec();
    QApplication::quit();

}
Connect::~Connect()
{

    main_socket->deleteLater();
    delete ui;
    delete timer_conn;
    delete gif_animation;
}

Dialog_window::Dialog_window(QWidget* parent) : QDialog( parent )   //диалоговое окно
{
    label = new QLabel(" ");
    layout = new QVBoxLayout;
    layout->addWidget(label);

    setLayout(layout);
    btns = new QButtonGroup();

    QObject::connect(btns, SIGNAL(buttonReleased(QAbstractButton*)), this, SLOT(btn_pressed(QAbstractButton*)));

}

QButtonGroup* Dialog_window::get_buttons()
{
    return btns;
}

void Dialog_window::set_text(QString text)
{
    label->setText(text);

}

int Dialog_window::get_state()
{
    return state;
}

std::string Dialog_window::get_friend_login()
{
    return (friend_login_pole->text()).toStdString();
}

void Dialog_window::set_make_request_mode()
{
    clear_buttons();
    set_text("Введите логин пользователя: ");
    state = 2;
    friend_login_pole = new QLineEdit();
    friend_login_pole->setMaxLength(15);
    layout->addWidget(friend_login_pole);
    QPushButton* buf = new QPushButton("Отправить запрос");
    btns->addButton(buf,1);
    layout->addWidget(buf);
    button_list.append(buf);


}
void Dialog_window::set_buttons_for_friends_list(std::vector <QString> &friend_logins, std::vector <bool> &friend_status)
{
    clear_buttons();
    set_text("Список друзей: ");
    QPushButton* buf;
    state = 0;
    for(unsigned int i = 0; i < friend_logins.size(); ++i)
    {
        buf = new QPushButton( friend_logins[i] );
        if (friend_status[i] == 0)
        {
            buf->setEnabled(0);
        }
        button_list.append(buf);
        btns->addButton(buf,i);
        layout->addWidget(buf);
    }

}
void Dialog_window::clear_buttons()
{
    for (auto i = button_list.begin(); i != button_list.end(); ++i)
    {
        layout->removeWidget(*i);
        btns->removeButton(*i);
        delete (*i);
    }
    button_list.clear();
    if (friend_login_pole != nullptr)
    {
        layout->removeWidget(friend_login_pole);
        delete friend_login_pole;
        friend_login_pole = nullptr;
    }


}
void Dialog_window::set_buttons_for_friends_requests(std::vector <QString> &friend_logins)
{
    clear_buttons();
    set_text("Запросы в друзья: ");
    QPushButton* buf;
    state = 1;
    for(unsigned int i = 0; i < friend_logins.size(); i++)
    {
        buf = new QPushButton(friend_logins[i]);
        btns->addButton(buf,i);
        layout->addWidget(buf);
        button_list.append(buf);
    }

}

void Dialog_window::set_buttons_for_communication_request_from_server(QString &user_login)
{
    clear_buttons();
    state = 3;
    set_text("Принять запрос на сессию от " + user_login + " ?");
    QPushButton* buf = new QPushButton("Да");
    btns->addButton(buf,0);
    layout->addWidget(buf);
    button_list.append(buf);
    buf = new QPushButton("Нет");
    btns->addButton(buf,1);
    layout->addWidget(buf);
    button_list.append(buf);

}

void Dialog_window::set_buttons_for_friend_request_answer(QString &user_login)
{
    clear_buttons();
    state = 4;
    set_text("Принять запрос дружбы от " + user_login + " ?");
    QPushButton* buf = new QPushButton("Да");
    btns->addButton(buf,0);
    layout->addWidget(buf);
    button_list.append(buf);
    buf = new QPushButton("Нет");
    btns->addButton(buf,1);
    layout->addWidget(buf);
    button_list.append(buf);

}

void Dialog_window::btn_pressed(QAbstractButton* btn)
{
    close();
    emit certain_btn_pressed_signal(btns->id(btn));

}

void Dialog_window::show_error(QString error)
{
    state = -1;
    close();
    clear_buttons();
    set_text(error);
    QPushButton* buf = new QPushButton("Ок");
    btns->addButton(buf,1);
    layout->addWidget(buf);
    button_list.append(buf);
    show();

}

Dialog_window::~Dialog_window()
{
    button_list.clear();
    delete btns;
    delete layout;
    delete label;
}




