// Logic for processing messages on the clientside from the server
// Author- Kevin Kellar - 2021

#include "fromserver_client.h"

#include <iostream>

#include "byte_order.h"
#include "handle.h"
#include "message.h"
#include "netcontext.h"
#include "packet.h"
#include "string.h"

#define DEBUG_FLAG 0

static CHECK_RETURN_VAL bool fromServerProcessBroadcastPacket(const LibPacketHeader* const header,
                                                              NetworksHandle* context);

bool fromServerProcessPacket(const LibPacketHeader* const header, NetworksHandle* context)
{
    using namespace std;
    if (!header)
    {
        cerr << "Bad client logic passed in a null header" << endl;
        exit(21);
    }

    switch (TO_FLAG(header->flag))
    {
        case FLAG_BROADCAST_MESSAGE:
        {
            return fromServerProcessBroadcastPacket(header, context);
            break;
        }
        default:
            cerr << "Unimplemented flag processing on the client" << header->flag << endl;
            return false;
    }

    return true;
}

static bool fromServerProcessBroadcastPacket(const LibPacketHeader* const header, NetworksHandle* context)
{
    using namespace std;

    size_t unpacked;
    string handle = clientHandleGetFromDataFragment(PACKET_PDU_DATA_SEGMENT(header), &unpacked);

    if (handle.empty())
    {
        cerr << "Failed to unpack a handle from a broadcast packet" << endl;
        return false;
    }

    // This copying block is almost certainly unessesary, but it provides good
    // error checking
    uint8_t buf[4096];
    size_t message_bytes_left = ntohs(header->pdu_size_check_endianness) - (unpacked + sizeof(LibPacketHeader));
    if (message_bytes_left > sizeof(buf))
    {
        cerr << "Won't be able to handle that large of broadcast packet" << endl;
        return false;
    }
    memcpy(buf, (char*)MAKE_POINTER_AT_OFFSET(header, sizeof(LibPacketHeader) + unpacked), message_bytes_left);

    if (!context->callback(context->caller_context, handle, buf, message_bytes_left))
    {
        cerr << "Failed user callback" << endl;
        return false;
    }

    return true;
}

// Logic for processing messages on the clientside from the server
// Author- Kevin Kellar - 2021