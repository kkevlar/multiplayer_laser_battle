// message fill packet related logic
// author- kevin kellar - 2021

#include "message.h"

#include <iostream>
#include <list>
#include <string>

#include "handle.h"
#include "packet.h"
#include "string.h"

bool messageFillOutgoingPacket(PacketFlag flag,
                               std::string sender_handle,
                               uint8_t* outgoing_data,
                               size_t outgoing_data_size,
                               uint8_t* buf,
                               size_t* out_total_bytes_used,
                               size_t buf_size)
{
    using namespace std;

    // Function misuse checks
    if (!(flag == FLAG_BROADCAST_MESSAGE))

    {
        cerr << "Don't know how to fill outgoing message packet for that flag." << endl;
        return false;
    }

    // Responsible for packing "my" handle
    size_t bytes_used = sizeof(LibPacketHeader);
    size_t fill_handle_bytes_used;
    bool result = clientHandleFillDataFragment(sender_handle,
                                               (uint8_t*)MAKE_POINTER_AT_OFFSET(buf, bytes_used),
                                               &fill_handle_bytes_used,
                                               buf_size - bytes_used);
    if (!result)
    {
        cerr << "Failed to fill MY handle into data fragment" << endl;
        return false;
    }
    bytes_used += fill_handle_bytes_used;

    // Some helpful error checking
    if (buf_size - bytes_used < outgoing_data_size)
    {
        cerr << "Not enough space now to put in the message" << endl;
        return false;
    }
    if (outgoing_data_size > 200)
    {
        cerr << "Message length of > 200 is not allowed." << endl;
        return false;
    }

    // Packs the actual message
    memcpy((uint8_t*)MAKE_POINTER_AT_OFFSET(buf, bytes_used), outgoing_data, outgoing_data_size);
    bytes_used += outgoing_data_size;

    packetFillOutgoingHeader(bytes_used, flag, buf, sizeof(LibPacketHeader));

    *out_total_bytes_used = bytes_used;

    return true;
}

// Message packing related logic
// Author- Kevin Kellar - 2021