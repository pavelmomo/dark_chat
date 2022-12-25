#ifndef MENU_H
#define MENU_H
#include <QMainWindow>
#include <QStyle>
#include <QDesktopWidget>
#include <QMessageBox>

QT_BEGIN_NAMESPACE
namespace Ui { class Menu; }
QT_END_NAMESPACE

class Menu : public QMainWindow
{
    Q_OBJECT

public:
    Menu(QWidget *parent = nullptr);
    void select_window(QMainWindow* pos_window);

    ~Menu();

private:
    Ui::Menu *ui;

public slots:
    void connect_slot();
    void set_chat_dialogue_mode();
    void print_message(QString);
private slots:
    void pre_send_message();
    void exit_chat();
signals:
    void get_user_list();
    void get_chat();
    void quit_from_chat();
    void send_message(QString);
};
#endif // MENU_H
