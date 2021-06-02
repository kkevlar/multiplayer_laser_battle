
#include "incoming.h"

#include <arpa/inet.h>
#include <stdbool.h>

#include <iostream>

#include "data_structure.h"
#include "handle.h"
#include "message.h"
#include "packet.h"

#define DEBUG_FLAG 0

static __attribute__((warn_unused_result)) bool incomingBroadcast(const LibPacketHeader* const header,
                                                                  int clientSockFd);

bool incomingPacket(const LibPacketHeader* const header, int clientSockFd)
{
    using namespace std;
    if (!header)
    {
        cerr << "Bad server logic passed in a null header" << endl;
        exit(5);
    }

    switch (TO_FLAG(header->flag))
    {
        case FLAG_CLIENT_HANDLE_PROPOSAL:
        {
            return incomingClientHandleProposal(header, clientSockFd);
            break;
        }
        case FLAG_BROADCAST_MESSAGE:
        {
            return incomingBroadcast(header, clientSockFd);
            break;
        }
        default:
            cerr << "Unimplemented flag processing on the server" << header->flag << endl;
            return false;
    }
    return true;
}

bool incomingClientHandleProposal(const LibPacketHeader* const header, int clientSockFd)
{
    using namespace std;
    if (!header)
    {
        cerr << "Bad server logic passed in a null theader" << endl;
        exit(6);
    }

    if (clientMapDoesFdExist(clientSockFd))
    {
        cerr << "Client (fd=" << clientSockFd << " , " << clientMapFindByFD(clientSockFd)
             << ") tried to reinitialize its handle..... Ignoring this." << endl;
        return false;
    }

    const std::string handle = clientHandleGetFromSimpleHandlePacket(header);
    if (handle.empty())
    {
        cerr << "Failed to process handle proposal for client fd=" << clientSockFd << endl;
        return false;
    }

    if (clientMapCheckHandleExistence(handle))
    {
        cout << "Client fd=" << clientSockFd << " proposed a duplicate handle. Sending rejection packet." << endl;
        if (!packetFillSendHeaderOnlyPacket(clientSockFd, FLAG_SERVER_HANDLE_PROPOSAL_REJECTION))
        {
            cerr << "Creative failure message 04" << endl;
            return false;
        }
    }
    else
    {
        clientMapAddHandle(handle, clientSockFd);
        cout << "Client fd=" << clientSockFd << " successfully bound it's handle to be \""
             << clientMapFindByFD(clientSockFd) << "\"" << endl;
        if (!packetFillSendHeaderOnlyPacket(clientSockFd, FLAG_SERVER_HANDLE_PROPOSAL_OK))
        {
            cerr << "Creative failure message 05" << endl;
            return false;
        }
    }
    return true;
}

static bool incomingBroadcast(const LibPacketHeader* const header, int clientSockFd)
{
    using namespace std;

#if DEBUG_FLAG
    cerr << "Dealing with broadcast message" << endl;
#endif

    for (const int& fd : clientFdListGetAll())
    {
        if (fd != clientSockFd)
        {
            packetSend(fd, (uint8_t*)header, ntohs(header->pdu_size_check_endianness));
        }
    }

    return true;
}
