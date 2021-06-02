// Server related logic
// Author- Kevin Kellar - 2021

#include <fcntl.h>
#include <stdbool.h>
#include <stdlib.h>
#include <sys/select.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>

#include <iostream>
#include <list>

#include "data_structure.h"
#include "incoming.h"
#include "networks.h"
#include "packet.h"

#define DEBUG_FLAG (0)

static __attribute__((warn_unused_result)) int checkArgs(int argc, char* argv[]);
static __attribute__((warn_unused_result)) int prepareSelectCall(fd_set* fds, int serverSocket);
static void selectSetCheckOnClientSockets(fd_set* fds);
static void selectOnSocketsForever(int serverSocket);

int main(int argc, char* argv[])
{
    int serverSocket = 0;  // socket descriptor for the server socket
    int portNumber = 0;

    portNumber = checkArgs(argc, argv);

    // create the server socket
    serverSocket = tcpServerSetup(portNumber);

    selectOnSocketsForever(serverSocket);

    close(serverSocket);

    return 0;
}

static void selectOnSocketsForever(int serverSocket)
{
    fd_set fds;
    for (;;)
    {
        const int max = prepareSelectCall(&fds, serverSocket);

        if (select(max + 1, &fds, NULL, NULL, NULL) < 0)
        {
            perror("Select owie");
            exit(2);
        }

        if (FD_ISSET(serverSocket, &fds))
        {
            int clientSocket = tcpAccept(serverSocket, DEBUG_FLAG);
            clientFDListAddFd(clientSocket);
        }

        selectSetCheckOnClientSockets(&fds);
    }
}

static __attribute__((warn_unused_result)) int prepareSelectCall(fd_set* fds, int serverSocket)
{
    FD_ZERO(fds);
    FD_SET(serverSocket, fds);
    int max = serverSocket;
    for (const int& sock : clientFdListGetAll())
    {
        if (sock > max)
        {
            max = sock;
        }
        if (sock < 0)
        {
            std::cerr << "Negative socket in fd list." << std::endl;
            exit(23);
        }
        FD_SET(sock, fds);
    }
    return max;
}

static void selectSetCheckOnClientSockets(fd_set* fds)
{
    std::list<int> socksToRemove{};
    for (const int& sock : clientFdListGetAll())
    {
        if (sock >= 0 && FD_ISSET(sock, fds))
        {
            uint8_t buf[20000];
            LibPacketHeader* header = packetFillIncomingUsingDualRecv(sock, buf, sizeof(buf));
            if (!header)
            {
                socksToRemove.push_back(sock);
                std::string handle = clientMapFindByFD(sock);
                if (handle.empty())
                {
                    std::cerr << "Unmapped client terminated its connection..... "
                                 "This is not a bug assuming we just rejected a "
                                 "client's handle. Continuing...."
                              << std::endl;
                }
                else if (!clientMapDeleteHandle(handle))
                {
                    std::cerr << "Failed to delete client by handle after "
                                 "it terminated. Fatal. Terminating."
                              << std::endl;
                    exit(27);
                }
            }
            else
            {
                if (!incomingPacket(header, sock))
                {
                    std::cerr << "Failed to process incoming packet for "
                                 "som reason.... Continuing."
                              << std::endl;
                }
            }
        }
    }

    while (socksToRemove.size() > 0)
    {
        int sock = socksToRemove.front();
        socksToRemove.pop_front();

        std::cerr << "Removing a client which disconnected." << std::endl;

        if (close(sock) < 0)
        {
            std::cerr << "Close failed" << std::endl;
            exit(30);
        }
        if (!clientFDListRemoveFd(sock))
        {
            std::cerr << "Failed to remove the client sock fd" << std::endl;
            exit(25);
        }
    }
}

static int checkArgs(int argc, char* argv[])
{
    // Checks args and returns port number
    int portNumber = 0;

    if (argc > 2)
    {
        fprintf(stderr, "Usage %s [optional port number]\n", argv[0]);
        exit(-1);
    }

    if (argc == 2)
    {
        portNumber = atoi(argv[1]);
    }

    return portNumber;
}

// Server related logic
// Author- Kevin Kellar - 2021