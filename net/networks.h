#ifndef __NETWORKS_H__
#define __NETWORKS_H__
#ifdef __cplusplus
extern "C"
{
#endif

#define LISTEN_BACKLOG 10

    // for the server side
    int tcpServerSetup(int portNumber);
    int tcpAccept(int server_socket, int debugFlag);

    // for the client side
    int tcpClientSetup(const char* serverName, const char* port, int debugFlag);

#ifdef __cplusplus
}
#endif

#endif
