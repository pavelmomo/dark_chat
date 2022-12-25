#include "connect.h"
#include "ui_connect.h"
#include <QMovie>

Connect::Connect(QWidget *parent)
    : QMainWindow(parent)
{
    ui = new  Ui::Connect;
    ui->setupUi(this);
    this->setGeometry(QStyle::alignedRect(Qt::LeftToRight,Qt::AlignCenter,this->size(),qApp->desktop()->availableGeometry()));
    this->ui->pole->close();
    this->ui->button->close();
    this->msg = new QMessageBox(this);
    this->gif_animation = new QMovie(":/img/images/anim.gif");
    ui->anim->setMovie(gif_animation);
    this->timer = new QTimer();
    this->timer->setInterval(1000);
    this->set_up_client();
    obj_listen = new listener(this->main_socket);
    this->dialog_window = new Dialog_window_chats(this);
    connect(this->timer, SIGNAL(timeout()), this, SLOT(timer_connect()));
    connect(this->ui->button,SIGNAL(released()),this, SLOT(button_pressed()));
    QObject::connect(this->dialog_window,SIGNAL(applied(int)), this, SLOT(onApplied(int)));
}
void Connect::select_window(QMainWindow* pos_window)
{
    this->setGeometry(pos_window->geometry());
    this->show();
    pos_window->close();
}
void Connect::start()
{
    this->gif_animation->start();
    this->timer->start();
    this->show();
    this->connect_cnt = 0;
}
void Connect::send_msg_to_server(QString s)
{
   Command cm;
   cm.action = 6;
   QByteArray ba = s.toLocal8Bit();
   char *b = ba.data();
   strcpy(cm.data,b);
   send(this->main_socket,&cm,sizeof(cm),NULL);
}
void Connect::timer_connect()
{

    if(this->connect_cnt == 5)      //неудача
    {
        this->timer->stop();
        this->no_connect();
    }
    else
    {
        this->connect_cnt++;

        if(::connect(this->main_socket,(sockaddr*)&this->addr,sizeof(this->addr)) >= 0 )    //успех
        {
            this->timer->stop();
            this->gif_animation->stop();
            this->ui->anim->close();
            this->ui->loading->setText("Введите ваш логин:");
            this->ui->pole->show();
            this->ui->button->show();

        }
    }

}
void Connect::no_connect(){         //ошибка подключение
    this->ui->loading->setText("Ошибка подключения!");
    this->gif_animation->stop();
}

void Connect::get_in_chat(){
    Command cmd;
    cmd.action = 4;
    send(this->main_socket,&cmd,sizeof(cmd),NULL);
    recv(this->main_socket,&cmd,sizeof(cmd),NULL);
    std::vector <QString> chats;
    QString buf;
    for(int i = 0; cmd.data[i] != '\0'; ++i)
    {
        if(cmd.data[i]!='\n')
        {
            buf += cmd.data[i];
        }
        else
        {
            chats.push_back(buf);
            buf.clear();
        }

    }
    this->dialog_window->set_buttons(chats);
    this->dialog_window->exec();

}
void Connect::onApplied(int id)       //вход в чат
{
    Command cmd;
    cmd.action = 5;
    char b[16];
    sprintf (b, "%d", id + 1);      //число в строку
    strcpy(cmd.data,b);
    send(this->main_socket,&cmd,sizeof(cmd),NULL);
    recv(this->main_socket,&cmd,sizeof(cmd),NULL);
    //std::cout<<"Сервер:\n"<<cmd.data<<endl;
    if(!cmd.action)
    {
        return;
    }
    this->thread_listen = new QThread;

    obj_listen->moveToThread(thread_listen);
    QObject::connect(thread_listen,SIGNAL(started()),obj_listen,SLOT(listen_server()));
    thread_listen->start();
    emit set_chat_mode();

}


void Connect::user_list(){      //слот на получение окна со списком пользователей
    Command cmd;
    cmd.action = 2;
    send(this->main_socket,&cmd,sizeof(cmd),NULL);
    recv(this->main_socket,&cmd,sizeof(cmd),NULL);
    QString str(cmd.data);
    str ="Активные пользователи:\n" + str;
    this->msg->setText(str);
    this->msg->show();
}

void Connect::button_pressed(){     //отправка логина
    QByteArray ba = (this->ui->pole->text()).toLocal8Bit();
    if(ba.size()>0 && ba[0] != ' ')
    {
        Command cmd;
        cmd.action = 1;
        char *b = ba.data();
        strncpy(cmd.data,b,20);
        send(this->main_socket,&cmd,sizeof(cmd),NULL);
        recv(this->main_socket,&cmd,sizeof(cmd),NULL);
        this->close();
        emit connect_successful();
    }
}
void Connect::quit_chat()
{
    Command cmd;
    cmd.action = 7;
    this->thread_listen->exit();
    send(this->main_socket,&cmd,sizeof(cmd),NULL);

}
bool Connect::set_up_client()     //настройка сервера
{
    this->main_socket = socket(AF_INET,SOCK_STREAM,0);
    if (main_socket < 0)
    {
        return 1;
    }
    addr.sin_family = AF_INET;
    addr.sin_port = htons(9999);
    addr.sin_addr.s_addr = INADDR_ANY;
    //addr.sin_addr.s_addr = inet_addr(SERVER_IP);
    return 0;
}
Connect::~Connect()
{
    delete ui;
}

Dialog_window_chats::Dialog_window_chats(QWidget* parent) : QDialog( parent )
{        //диалоговое окно с выбором чата
    this->layout = new QVBoxLayout;
    this->label = new QLabel("Активные чаты:");
    this->layout->addWidget(label);
    this->btns = new QButtonGroup();
    connect(btns, SIGNAL(buttonReleased(QAbstractButton*)), this, SLOT(btn_pressed(QAbstractButton*)));
}

void Dialog_window_chats::set_buttons( std::vector <QString> chat_names)
{   this->btns->removeButton()
    QPushButton* buf;
    for(unsigned long i = 0; i < chat_names.size(); ++i)
    {
        buf = new QPushButton( chat_names[i] );
        this->btns->addButton(buf,i);
        this->layout->addWidget( buf );
    }
    setLayout( this->layout );
}
void Dialog_window_chats::btn_pressed(QAbstractButton* btn)
{
    this->close();
    emit applied(this->btns->id(btn));

}
Dialog_window_chats::~Dialog_window_chats()
{

}

listener::listener(int sock)
{
    this->fd = sock;
}

void listener::listen_server()
{
    Command cm;
    while(1)
    {
        //recv(this->fd,&cm,sizeof(cm),NULL);
        std::cout<<recv(this->fd,&cm,sizeof(cm),NULL)<<std::endl;
        if(cm.action == 7)
        {
            break;
        }
        emit heard_message(QString(cm.data));

    }

}
