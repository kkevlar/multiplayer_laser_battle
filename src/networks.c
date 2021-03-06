
// Provided by professor 

// NOT ALLOWED ON WINDOWS
#ifndef _MSC_VER

#include "networks.h"

#include <fcntl.h>
#include <netdb.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <stdbool.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/uio.h>
#include <unistd.h>

#include "gethostbyname6.h"
#include "compat.h"

/* This function sets the server socket.  It lets the system
determine the port number.  The function returns the server
socket number and prints the port number to the screen.  */

int tcpServerSetup(int portNumber)
{
    int server_socket = 0;
    struct sockaddr_in6 server = {0}; /* socket address for local side  */
    socklen_t len = sizeof(server);   /* length of local address        */

    /* create the tcp socket  */
    server_socket = socket(AF_INET6, SOCK_STREAM, 0);
    if (server_socket < 0)
    {
        perror("socket call");
        exit(1);
    }

    server.sin6_family = AF_INET6;
    server.sin6_addr = in6addr_any;  // wild card machine address
    server.sin6_port = htons(portNumber);

    /* bind the name (address) to a port */
    if (bind(server_socket, (struct sockaddr*)&server, sizeof(server)) < 0)
    {
        perror("bind call");
        exit(-1);
    }

    // get the port name and print it out
    if (getsockname(server_socket, (struct sockaddr*)&server, &len) < 0)
    {
        perror("getsockname call");
        exit(-1);
    }

    if (listen(server_socket, LISTEN_BACKLOG) < 0)
    {
        perror("listen call");
        exit(-1);
    }

    printf("Server Port Number %d \n", ntohs(server.sin6_port));

    return server_socket;
}

// This function waits for a client to ask for services.  It returns
// the client socket number.

int tcpAccept(int server_socket, int debugFlag)
{
    struct sockaddr_in6 clientInfo;
    int clientInfoSize = sizeof(clientInfo);
    int client_socket = 0;

    if ((client_socket = accept(server_socket,
                                (struct sockaddr*)&clientInfo,
                                (socklen_t*)&clientInfoSize)) < 0)
    {
        perror("accept call");
        exit(-1);
    }

    if (debugFlag)
    {
        printf("Client accepted.  Client IP: %s Client Port Number: %d\n",
               getIPAddressString(clientInfo.sin6_addr.s6_addr),
               ntohs(clientInfo.sin6_port));
    }

    return (client_socket);
}

bool tcpClientSetup(const char* serverName, const char* port, int debugFlag, CompatSocket* out_compat_sock)
{
    // This is used by the client to connect to a server using TCP

    int socket_num;
    uint8_t* ipAddress = NULL;
    struct sockaddr_in6 server = {0};

    // create the socket
    if ((socket_num = socket(AF_INET6, SOCK_STREAM, 0)) < 0)
    {
        perror("socket call");
        exit(-1);
    }

    if (debugFlag)
    {
        printf("Connecting to server on port number %s\n", port);
    }

    // setup the server structure
    server.sin6_family = AF_INET6;
    server.sin6_port = htons(atoi(port));

    // get the address of the server
    if ((ipAddress = getIPAddress6(serverName, &server)) == NULL)
    {
        exit(-1);
    }

    // printf("server ip address: %s\n", getIPAddressString(ipAddress));
    if (connect(socket_num, (struct sockaddr*)&server, sizeof(server)) < 0)
    {
        perror("connect call");
        exit(-1);
    }

    if (debugFlag)
    {
        printf("Connected to %s IP: %s Port Number: %d\n",
               serverName,
               getIPAddressString(ipAddress),
               atoi(port));
    }

    if(socket_num <= 0)
    {
        return false;
    }

    CompatSocket cs = {socket_num};
    memcpy(out_compat_sock ,& cs, sizeof(CompatSocket));
    return true;
}

#endif