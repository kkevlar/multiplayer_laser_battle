#pragma once

#include "compat.h"
#include <stdbool.h>

#ifdef __cplusplus
extern "C"
{
#endif

#define LISTEN_BACKLOG 10

    // for the server side
    int tcpServerSetup(int portNumber);
    int tcpAccept(int server_socket, int debugFlag);

    // for the client side
    bool tcpClientSetup(const char* serverName, const char* port, int debugFlag, CompatSocket* out_compat_sock);

#ifdef __cplusplus
}
#endif

