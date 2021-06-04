// Client library main file for use in online games
// Author- Kevin Kellar - 2021

#include "simple_ll_netlib_client.h"

#include <string.h>

#include <iostream>
#include <list>
#include <string>
#include <assert.h>

#include "fdselect.h"
#include "fromserver_client.h"
#include "compat.h"
#include "handle.h"
#include "message.h"
#include "packet.h"
#include "netcontext.h"
#include "byte_order.h"

#ifdef _MSC_VER
#include "windows_tcp_client.h"
#else
#include "networks.h"
#endif

#define DEBUG_FLAG 0

static bool clientProcessServer(NetworksHandle* handle)
{
    uint8_t buf[4096];
    LibPacketHeader* header = packetFillIncomingUsingDualRecv(handle->socketNum, buf, sizeof(buf));
    if (!header)
    {
        std::cout << "Server Terminated." << std::endl;
        return false;
    }
    else
    {
        if (!fromServerProcessPacket(header, handle))
        {
            std::cerr << "Failed to process an incoming packet on the "
                         "client side. Continuing..."
                      << std::endl;
        }
    }
    return true;
}

static CHECK_RETURN_VAL bool setupHandle(std::string handle, CompatSocket sock, UCIDPayload* ucid)
{
    using namespace std;
NULLCHECK(ucid);

    uint8_t buf[200];
    size_t len;

    if (!clientHandleFillSimplePacket(FLAG_CLIENT_HANDLE_PROPOSAL, handle, buf, &len, sizeof(buf)))
    {
        cerr << "Failed to fill the handle proposal packet" << endl;
        return false;
    }

    // TODO does this need to have a return value so that we exit super
    // gracefully on server exit
    packetSend(sock, buf, len);

#if DEBUG_FLAG == 1
    cerr << "Handle proposal submitted to server. Waiting for reply..." << endl;
#endif

    LibPacketHeader* header = packetFillIncomingUsingDualRecv(sock, buf, sizeof(buf));
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
    if (TO_FLAG(header->flag) == FLAG_SERVER_HANDLE_PROPOSAL_OK_WITH_COLOR_ALLOCATION)
    {
        log_info("Handle proposal approved by server handle: \"%s\"", handle.c_str());

        UCIDPayload* payload = (UCIDPayload*) PACKET_PDU_DATA_SEGMENT(header);
        memcpy (ucid, payload, sizeof(UCIDPayload));

        return true;
    }
    else if (TO_FLAG(header->flag) == FLAG_SERVER_HANDLE_PROPOSAL_REJECTION)
    {
        log_error("Handle already in use!! \"%s\"", handle.c_str());
        return false;
    }
    else
    {
        cerr << "Unexpected reply from server during handle exchange" << endl;
        return false;
    }
}

static CHECK_RETURN_VAL std::string checkThenSetupHandle(const char* handle, CompatSocket sock, UCIDPayload* ucid)
{
    NULLCHECK(ucid);
    std::string myhandle = std::string{handle};
    if (!handleCheckValid(myhandle))
    {
        std::cerr << "Bad handle. Terminating." << std::endl;
        exit(31);
    }

    if (!setupHandle(myhandle, sock, ucid))
    {
        std::cerr << "Failed to setup handle. Terminating." << std::endl;
        exit(40);
    }

#if DEBUG_FLAG == 1
    std::cerr << "Handle proposal phase finished. Now going to enter select state." << std::endl;
#endif
    return myhandle;
}

CHECK_RETURN_VAL bool publicClientInitialize(const char* handle_AKA_name,
                                                                const char* host_name,
                                                                const char* port_num,
                                                                void* caller_context,
                                                                NetworksCallback callback,
                                                                NetworksHandle* out_handle)
                                                                NetworksHandle** out_handle,
                                                                uint16_t* ucid,
                                                                uint32_t* ms_elapsed)
{
    NULLCHECK(out_handle);
    NULLCHECK(ucid);
    NULLCHECK(ms_elapsed);
    CompatSocket myCompatSocket;

    // THIS IS NEVER FREED LOL
    memset(out_handle, 0, sizeof(NetworksHandle));

    /* set up the TCP Client socket  */
#ifdef _MSC_VER
    if(!
    winTcpClientSetup(host_name, port_num, &myCompatSocket))
{return false;
    }
#else
    if(!tcpClientSetup(host_name, port_num, DEBUG_FLAG, &myCompatSocket))
{return false;

    }
#endif

UCIDPayload payload;

    std::string myhandle = checkThenSetupHandle(handle_AKA_name, myCompatSocket, &payload);

    out_handle->callback = callback;
    out_handle->caller_context = caller_context;
    assert(sizeof(out_handle->username) ==100);
    strncpy(out_handle->username, myhandle.c_str(), sizeof(out_handle->username-1));
    memcpy(&out_handle->socketNum ,&myCompatSocket, sizeof(myCompatSocket)) ;

    *out_handle = give_to_caller_handle;

using namespace std;
    if(payload.magic1 != UCID_PAYLOAD_MAGIC_1)
    {
        cerr << "BAD MAGIC 1 UCID" << endl;
        return false;
    }
    else if(payload.magic2 != UCID_PAYLOAD_MAGIC_2)
    {
        cerr << "BAD MAGIC 2 UCID" << endl;
        return false;
    }

    *ucid = ntohs(payload.ucid_check_endianness);
    *ms_elapsed = ntohl(payload.num_ms_check_endiannes);

    return true;
}

CHECK_RETURN_VAL bool publicClientPollSelectForMessages(NetworksHandle* handle)
{

again:
fd_selector_clearFds(&handle->selector);
fd_selector_addFd(&handle->selector, handle->socketNum);

    if (!fd_selector_performSelect(&handle->selector, 0))
    {
        perror("Select owie");
        exit(20);
    }

    if (fd_selector_testPostSelectMembership(&handle->selector, handle->socketNum))
    {
        if (!clientProcessServer(handle))
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

            FLAG_BROADCAST_MESSAGE, handle->username, data, data_size, buf, &used, sizeof(buf)))
    {
        return false;
    }
    packetSend(handle->socketNum, buf, used);

    return true;
}




// Client library main file for use in online games
