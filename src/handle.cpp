// Handle-related logic
// Author- Kevin Kellar - 2021

#include "handle.h"

#include <string.h>

#include <iostream>

#define HANDLE_DEBUG 0

typedef struct
{
    uint8_t handle_len;
    char handle[HANDLE_MAX];
} HandlePacket;
COMPILER_ASSERT(HANDLE_MAX + 1 == sizeof(HandlePacket), handlepacket);

bool handleCheckValid(std::string handle)
{
    using namespace std;
    if (handle[0] >= '0' && handle[0] <= '9')
    {
        cerr << "Handle may not begin with a letter" << endl;
        return false;
    }
    for (const char& c : handle)
    {
        if (c == '\0' || c == '\n' || c == ' ')
        {
            cerr << "Malformed handle. Illegal character in handle." << endl;
            return false;
        }
    }

    if (handle.length() > HANDLE_MAX)
    {
        cerr << "Malformed handle. Handle is too long." << endl;
        return false;
    }

    return true;
}

const std::string clientHandleGetFromDataFragment(const uint8_t* const frag, size_t* bytes_unpacked)
{
    using namespace std;
    HandlePacket* handle_packet = (HandlePacket*)frag;

#if HANDLE_DEBUG == 1
    cerr << "Incoming handle length " << (int)handle_packet->handle_len << endl;
#endif

    if (handle_packet->handle_len > HANDLE_MAX)
    {
        cerr << "Attemping to unpack too large of handle" << endl;
        return string{};
    }

    size_t unpacked = 1;  // Start at 1 because of the length
    string handle{};

    for (uint8_t i = 0; i < handle_packet->handle_len; i++)
    {
        const char c = handle_packet->handle[i];
        if (c == '\0')
        {
            cerr << "Attemping to unpack a handle with an unexpected null "
                    "terminator"
                 << endl;
            return string{};
        }
        if (c == '\n')
        {
            cerr << "Attemping to unpack a handle with an unexpected newline" << endl;
            return string{};
        }
        handle.append(string{c});
        unpacked += 1;
    }

    if (bytes_unpacked)
    {
        *bytes_unpacked = unpacked;
    }
#if HANDLE_DEBUG == 1
    cerr << "Consumed " << unpacked << " bytes when unpacking handle=" << handle << endl;
#endif

    return handle;
}

const std::string clientHandleGetFromSimpleHandlePacket(const LibPacketHeader* const header)
{
    using namespace std;
    if (!(header->flag == FLAG_CLIENT_HANDLE_PROPOSAL))
    {
        cerr << "Ambiguious packet passed to gethandle " << header->flag << endl;
        exit(4);
    }

    return clientHandleGetFromDataFragment(PACKET_PDU_DATA_SEGMENT(header), NULL);
}

bool clientHandleFillDataFragment(const std::string handle, uint8_t* buf, size_t* filled_size, size_t buf_size)
{
    using namespace std;
    if (!buf || !filled_size)
    {
        cerr << "Null buf size ptr clientHandleFillDataFragment" << endl;
        exit(14);
    }
    if (buf_size < sizeof(HandlePacket))
    {
        cerr << "Too small buffer size passed to clientHandleFillDataFragment" << endl;
        return false;
    }
    if (handle.length() > HANDLE_MAX)
    {
        cerr << "Too large of handle for clientHandleFillDataFragment" << endl;
        return false;
    }
    HandlePacket* packet = (HandlePacket*)MAKE_POINTER_AT_OFFSET(buf, 0);
    packet->handle_len = (uint8_t)handle.length();
    memcpy(packet->handle, handle.c_str(), packet->handle_len);
    *filled_size = handle.length() + 1;
    return true;
}

bool clientHandleFillSimplePacket(
    PacketFlag flag, const std::string handle, uint8_t* buf, size_t* filled_size, size_t buf_size)
{
    using namespace std;
    if (!(flag == FLAG_CLIENT_HANDLE_PROPOSAL))
    {
        cerr << "Ambiguious packet passed to fill simple packet " << flag << endl;
        exit(12);
    }
    if (!buf || !filled_size)
    {
        cerr << "Null buf or size ptr clientHandleFillSimplePacket" << endl;
        exit(13);
    }

    size_t frag_filled_size = 0;
    bool result = clientHandleFillDataFragment(handle,
                                               (uint8_t*)MAKE_POINTER_AT_OFFSET(buf, sizeof(LibPacketHeader)),
                                               &frag_filled_size,
                                               buf_size - sizeof(LibPacketHeader));
    *filled_size = sizeof(LibPacketHeader) + frag_filled_size;
    packetFillOutgoingHeader(*filled_size, flag, buf, buf_size);
#if HANDLE_DEBUG == 1
    cout << "Outgoing packet length for clientHandleFillSimplePacket " << (int)*filled_size << endl;
#endif
    return result;
}

// Handle-related logic
// Author- Kevin Kellar - 2021