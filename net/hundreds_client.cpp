#include <fcntl.h>
#include <sys/select.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>

#include <iostream>
#include <list>
#include <string>

#include "fromserver_client.h"
#include "handle.h"
#include "keyboard.h"
#include "networks.h"

#define DEBUG_FLAG 1

static __attribute__((warn_unused_result)) std::string checkThenSetupHandle(
    char** argv, int socketNum, int count);
static __attribute__((warn_unused_result)) bool setupHandle(std::string handle,
                                                            int fd);
static void checkArgs(int argc, char* argv[]);

int main(int argc, char* argv[])
{
    int socketNum = -1;  // socket descriptor

    srand(time(NULL));

    checkArgs(argc, argv);

    for (int i = 0; i < 300; i++)
    {
        socketNum = tcpClientSetup(argv[2], argv[3], DEBUG_FLAG);
        if (rand() % 2 == 1)
        {
            std::string myhandle = checkThenSetupHandle(argv, socketNum, i);
        }
        uint8_t buf[256];
        for (size_t j = 0; j < sizeof(buf); j++)
        {
            buf[j] = rand();
        }
        // packetSend(socketNum, buf, sizeof(buf));
    }

    for (;;)
    {
    }

    close(socketNum);

    return 0;
}

static std::string checkThenSetupHandle(char** argv, int socketNum, int mycount)
{
    std::string myhandle = std::string{argv[1]};
    myhandle.append(std::to_string(mycount));
    if (!handleCheckValid(myhandle))
    {
        std::cerr << "Bad handle. Terminating." << std::endl;
        exit(31);
    }

    if (!setupHandle(myhandle, socketNum))
    {
        std::cerr << "Failed to setup handle. Terminating." << std::endl;
        exit(40);
    }

#if DEBUG_FLAG == 1
    std::cerr
        << "Handle proposal phase finished. Now going to enter select state."
        << std::endl;
#endif
    return myhandle;
}

static bool setupHandle(std::string handle, int fd)
{
    using namespace std;

    uint8_t buf[200];
    size_t len;

    if (!clientHandleFillSimplePacket(
            FLAG_CLIENT_HANDLE_PROPOSAL, handle, buf, &len, sizeof(buf)))
    {
        cerr << "Failed to fill the handle proposal packet" << endl;
        return false;
    }

    // TODO does this need to have a return value so that we exit super
    // gracefully on server exit
    packetSend(fd, buf, len);

#if DEBUG_FLAG == 1
    cerr << "Handle proposal submitted to server. Waiting for reply..." << endl;
#endif

    ChatHeader* header = packetFillIncomingUsingDualRecv(fd, buf, len);
#if DEBUG_FLAG == 1
    cerr << "Server reponse!" << endl;
#endif
    if (!header)
    {
        cerr << "Server unexpectedly disconnected while we're proposing the "
                "handle"
             << endl;
        return false;
    }
    if (TO_CHAT_FLAG(header->flag) == FLAG_SERVER_HANDLE_PROPOSAL_OK)
    {
#if DEBUG_FLAG == 1
        cerr << "Handle proposal approved by server." << endl;
#endif
        return true;
    }
    else if (TO_CHAT_FLAG(header->flag) ==
             FLAG_SERVER_HANDLE_PROPOSAL_REJECTION)
    {
        cerr << "Handle already in use: " << handle << endl;
        return false;
    }
    else
    {
        cerr << "Unexpected reply from server during handle exchange" << endl;
        return false;
    }
}

static void checkArgs(int argc, char* argv[])
{
    /* check command line arguments  */
    if (argc != 4)
    {
        printf("usage: %s handleprefix host-name port-number \n", argv[0]);
        exit(1);
    }
}
