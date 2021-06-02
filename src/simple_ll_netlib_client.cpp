// Client library main file for use in online games
// Author- Kevin Kellar - 2021

#include "simple_ll_netlib_client.h"

#include <string.h>

#include <iostream>
#include <list>
#include <string>

#include "fdselect.h"
#include "fromserver_client.h"
#include "handle.h"
#include "message.h"
#include "packet.h"
#include "netcontext.h"

#ifdef _MSC_VER
#include "windows_tcp_client.h"
#else
#include "networks.h"
#endif

#define DEBUG_FLAG 0

static CHECK_RETURN_VAL bool clientProcessServer(NetworksContext* context);
static CHECK_RETURN_VAL std::string checkThenSetupHandle(const char* handle, int socketNum);
static CHECK_RETURN_VAL bool setupHandle(std::string handle, int fd);

CHECK_RETURN_VAL bool publicClientInitialize(const char* handle_AKA_name,
                                                                const char* host_name,
                                                                const char* port_num,
                                                                void* caller_context,
                                                                NetworksCallback callback,
                                                                NetworksHandle** out_handle)
{
    void* socketNum = NULL;

    // THIS IS NEVER FREED LOL
    NetworksHandle* give_to_caller_handle = (NetworksHandle*)malloc(sizeof(NetworksHandle));
    give_to_caller_handle->net_context = (NetworksContext*)malloc(sizeof(NetworksContext));

    /* set up the TCP Client socket  */
#ifdef _MSC_VER
    socketNum = winTcpClientSetup(host_name, port_num);
#else
    socketNum = tcpClientSetup(host_name, port_num, DEBUG_FLAG);
#endif

    if (socketNum <= 0)
    {
        return false;
    }

    std::string myhandle = checkThenSetupHandle(handle_AKA_name, socketNum);

    give_to_caller_handle->net_context->selector = FDSelector{};
    give_to_caller_handle->net_context->callback = callback;
    give_to_caller_handle->net_context->caller_context = caller_context;
    give_to_caller_handle->net_context->handle = myhandle;
    give_to_caller_handle->net_context->socketNum = socketNum;

    *out_handle = give_to_caller_handle;
    return true;
}

CHECK_RETURN_VAL bool publicClientPollSelectForMessages(NetworksHandle* handle)
{
    int socketNum = handle->net_context->socketNum;

again:
    handle->net_context->selector.clearFds();
    handle->net_context->selector.addFd(socketNum);

    if (!handle->net_context->selector.performSelect(0))
    {
        perror("Select owie");
        exit(20);
    }

    if (handle->net_context->selector.testPostSelectMembership(socketNum))
    {
        if (!clientProcessServer(handle->net_context))
        {
            return false;
        }
        goto again;
    }
    return true;
}

CHECK_RETURN_VAL bool publicBroadcastOutgoing(NetworksHandle* handle,
                                                                 uint8_t* data,
                                                                 size_t data_size)
{
    uint8_t buf[4096];
    size_t used;
    if (!messageFillOutgoingPacket(

            FLAG_BROADCAST_MESSAGE, handle->net_context->handle, data, data_size, buf, &used, sizeof(buf)))
    {
        return false;
    }
    packetSend(handle->net_context->socketNum, buf, used);

    return true;
}

static bool clientProcessServer(NetworksContext* context)
{
    uint8_t buf[4096];
    LibPacketHeader* header = packetFillIncomingUsingDualRecv(context->socketNum, buf, sizeof(buf));
    if (!header)
    {
        std::cout << "Server Terminated." << std::endl;
        return false;
    }
    else
    {
        if (!fromServerProcessPacket(header, context))
        {
            std::cerr << "Failed to process an incoming packet on the "
                         "client side. Continuing..."
                      << std::endl;
        }
    }
    return true;
}

static std::string checkThenSetupHandle(const char* handle, int socketNum)
{
    std::string myhandle = std::string{handle};
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
    std::cerr << "Handle proposal phase finished. Now going to enter select state." << std::endl;
#endif
    return myhandle;
}

static bool setupHandle(std::string handle, int fd)
{
    using namespace std;

    uint8_t buf[200];
    size_t len;

    if (!clientHandleFillSimplePacket(FLAG_CLIENT_HANDLE_PROPOSAL, handle, buf, &len, sizeof(buf)))
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

    LibPacketHeader* header = packetFillIncomingUsingDualRecv(fd, buf, len);
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
    if (TO_FLAG(header->flag) == FLAG_SERVER_HANDLE_PROPOSAL_OK)
    {
#if DEBUG_FLAG == 1
        cerr << "Handle proposal approved by server." << endl;
#endif
        return true;
    }
    else if (TO_FLAG(header->flag) == FLAG_SERVER_HANDLE_PROPOSAL_REJECTION)
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

// Client library main file for use in online games
// Author- Kevin Kellar - 2021