// Packet Utility Functions
// Author- Kevin Kellar - 2021

#pragma once

#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include "log.h"
#include "compat.h"

#define MAKE_POINTER_AT_OFFSET(base, offset) (((uintptr_t)base) + ((uintptr_t)offset))

#define PACKET_PDU_DATA_SEGMENT(header)                                                      \
    ((uint8_t*)((bool)header->flag ? MAKE_POINTER_AT_OFFSET(header, sizeof(LibPacketHeader)) \
                                   : MAKE_POINTER_AT_OFFSET(header, sizeof(LibPacketHeader))))

#define COMPILER_ASSERT(C, NAME) typedef int compiler_assert_##NAME[(C) ? 1 : -1]

#define TO_FLAG(it) ((PacketFlag)it)

typedef enum
{
    FLAG_CLIENT_HANDLE_PROPOSAL = 1,
    FLAG_SERVER_HANDLE_PROPOSAL_OK_WITH_COLOR_ALLOCATION,
    FLAG_SERVER_HANDLE_PROPOSAL_REJECTION,
    FLAG_BROADCAST_MESSAGE,
} PacketFlag;
COMPILER_ASSERT(FLAG_BROADCAST_MESSAGE == 4, flags);

typedef struct 
{
    uint16_t pdu_size_check_endianness;
    uint8_t flag;
    uint8_t pad;
} LibPacketHeader;
COMPILER_ASSERT(sizeof(LibPacketHeader) == 4, LibPacketHeader);

typedef struct 
{
    uint8_t magic1;
    uint8_t magic2;
    uint16_t ucid_check_endianness;
    uint32_t num_ms_check_endiannes;
} UCIDPayload;
COMPILER_ASSERT(sizeof(UCIDPayload) == 8, UCIDPayload);

#define UCID_PAYLOAD_MAGIC_1 (0x19)
#define UCID_PAYLOAD_MAGIC_2 (0xd3)

#ifdef __cplusplus
extern "C"
{
#endif

    CHECK_RETURN_VAL LibPacketHeader* packetFillIncomingUsingDualRecv(CompatSocket sock,
                                                                                         uint8_t* buffer,
                                                                                         size_t buf_size);
    size_t packetFillOutgoingHeader(size_t pdu_size, PacketFlag flag, uint8_t* buf, size_t buf_size);
    void packetSend(CompatSocket sock, uint8_t* buf, size_t buf_size);
    CHECK_RETURN_VAL bool packetFillSendHeaderOnlyPacket(CompatSocket socket, PacketFlag flag);

#ifdef __cplusplus
}
#endif

// Packet Utility Functions
// Author- Kevin Kellar - 2021