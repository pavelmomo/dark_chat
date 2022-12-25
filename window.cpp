#include "window.h"


void window::show_on_pos(QMainWindow* pos_window)
{
    this->setGeometry(pos_window->geometry());
    this->show();

}

