#ifndef COMMAND_H_INCLUDED
#define COMMAND_H_INCLUDED

#include "string"
#include "string.h"


class Command
{public:

    bool set_command(char *command_mas, int recv_from);       //преобразование из массива char
    bool transform_command(char *output_mas);       //преобразование в массив char
    void set_action(char action);       //установить действие
    char get_action();      //получить действие
    void set_from(int from);        //установить адресат
    int get_from();     //получить адресат
    void set_data(std::string data);        //установить данные 
    std::string get_data();         //получить данные 
    char* get_data_massive();       //получить данные в виде массива char
    void clear();       //очистить команду
    static unsigned int get_command_size();     //получить размер выходного массива
private:
    char action = 0;        //действие
    int from = 0;       //адресат
    char data[500] = {32};      //данные
    const static unsigned int command_size = 501;       //размер выходного массива

};

#endif // COMMAND_H_INCLUDED
