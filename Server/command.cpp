#include "command.h"

void Command::clear()
{
    action = 0;
    from = 0;
    memset(data,' ',sizeof(data));
}

bool Command::set_command(char *command_mas, int recv_from)
{
    from = recv_from;
    action = command_mas[command_size - 1];
    strncpy(data,command_mas,command_size - 1);
    return 0;
}

bool Command::transform_command(char *output_mas)
{
    strncpy(output_mas,data,command_size - 1);
    output_mas[command_size - 1] = action;
    return 0;

}

unsigned int Command::get_command_size()
{
    return command_size;
}

void Command::set_action(char action)
{
    this->action = action;
}

char Command::get_action()
{
    return action;
}

void Command::set_from(int from)
{
    this->from = from;
}

int Command::get_from()
{
    return from;
}

void Command::set_data(std::string data)
{
    strncpy(this->data,data.c_str(),sizeof(this->data));
}

std::string Command::get_data()
{
    std::string data_string (data);
    return data_string;

}

char* Command::get_data_massive()
{
    return data;
}
