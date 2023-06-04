#ifndef COMMAND_H_INCLUDED
#define COMMAND_H_INCLUDED

#include "string"
#include "string.h"


class Command
{public:

    //0-создание user,1-регистрация,2-авторизация,3-вывести список активных п.
    bool set_command(char *command_mas, int recv_from);
    bool transform_command(char *output_mas);
    void set_action(char action);
    char get_action();
    void set_from(int from);
    int get_from();
    void set_data(std::string data);
    std::string get_data();
    char* get_data_massive();
    void clear();
    static unsigned int get_command_size();
private:
    char action = 0;
    int from = 0;
    char data[500] = {32};
    static inline unsigned int command_size = 501;

};

#endif // COMMAND_H_INCLUDED
