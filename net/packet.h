// Packet Utility Functions
// Author- Kevin Kellar - 2021

#pragma once

#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>

#define MAKE_POINTER_AT_OFFSET(base, offset) (((uintptr_t)base) + ((uintptr_t)offset))

#define PACKET_PDU_DATA_SEGMENT(header)                                                      \
    ((uint8_t*)((bool)header->flag ? MAKE_POINTER_AT_OFFSET(header, sizeof(LibPacketHeader)) \
                                   : MAKE_POINTER_AT_OFFSET(header, sizeof(LibPacketHeader))))

#define COMPILER_ASSERT(C, NAME) typedef int compiler_assert_##NAME[(C) ? 1 : -1]

#define TO_FLAG(it) ((PacketFlag)it)

typedef enum
{
    FLAG_CLIENT_HANDLE_PROPOSAL = 1,
    FLAG_SERVER_HANDLE_PROPOSAL_OK,
    FLAG_SERVER_HANDLE_PROPOSAL_REJECTION,
    FLAG_BROADCAST_MESSAGE,
} PacketFlag;
COMPILER_ASSERT(FLAG_BROADCAST_MESSAGE == 4, flags);

typedef struct __attribute__((packed))
{
    uint16_t pdu_size_check_endianness;
    uint8_t flag;
} LibPacketHeader;
COMPILER_ASSERT(sizeof(LibPacketHeader) == 3, LibPacketHeader);

#ifdef __cplusplus
extern "C"
{
#endif

    __attribute__((warn_unused_result)) LibPacketHeader* packetFillIncomingUsingDualRecv(int socket_fd,
                                                                                         uint8_t* buffer,
                                                                                         size_t buf_size);
    size_t packetFillOutgoingHeader(size_t pdu_size, PacketFlag flag, uint8_t* buf, size_t buf_size);
    void packetSend(int fd, uint8_t* buf, size_t buf_size);
    __attribute__((warn_unused_result)) bool packetFillSendHeaderOnlyPacket(int fd, PacketFlag flag);

#ifdef __cplusplus
}
#endif

// Packet Utility Functions
// Author- Kevin Kellar - 2021