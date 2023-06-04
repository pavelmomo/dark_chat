#include <iostream>
#include "server_handle.h"



int main()
{

    Server_Handle *server = new Server_Handle;
    if (!server->Set_up_server(9999, 100000,100000,4));
    {
        server->Start();
    }

    return 0;
}
