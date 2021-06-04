// Packet Utility Functions
// Author- Kevin Kellar - 2021

#include "packet.h"

#include <stdio.h>

#include "byte_order.h"
#include "compat.h"
#include "log.h"

#define PACKET_DEBUG 0

LibPacketHeader* packetFillIncomingUsingDualRecv(CompatSocket sock, uint8_t* buf, size_t buf_size)
{
    if (!buf)
    {
        log_error( "Null buf ptr packetFillIncomingUsingDualRecv\n");
        exit(8);
    }

    LibPacketHeader* header = (LibPacketHeader*)MAKE_POINTER_AT_OFFSET(buf, 0);
    int bytes_read;

    size_t bytes_requested = sizeof(LibPacketHeader);
    if (bytes_requested > buf_size)
    {
        log_error(
                "WAY too small buf size passed into "
                "packetFillIncomingUsingDualRecv\n");
        exit(2);
    }

    if ((bytes_read = compat_recv_waitall(sock, buf, bytes_requested)) < 0)
    {
        perror("recv call errored");
        log_error( "GONNA JUST PRENTEND IT DIDNT FAIL\n");
    }
    else if (bytes_read < (int)bytes_requested)
    {
        return NULL;
    }

    bytes_requested = ntohs(header->pdu_size_check_endianness) - sizeof(LibPacketHeader);

    if (bytes_requested + sizeof(LibPacketHeader) > buf_size)
    {
        log_error(
                "Too small buf size passed into "
                "packetFillIncomingUsingDualRecv\nManually disconnecting this "
                "malicious client\n");
        return NULL;
    }
    else if (bytes_requested > 0)

    {
#if PACKET_DEBUG == 1
        log_error( "Gonna block on getting %lu more bytes of data \n", bytes_requested);
#endif
        if ((bytes_read = compat_recv_waitall(sock, PACKET_PDU_DATA_SEGMENT(header), bytes_requested)) < 0)
        {
            perror("recv call2 errored");
            log_error( "GONNA JUST PRENTEND IT DIDNT FAIL\n");
        }
        else if (bytes_read < (int)bytes_requested)
        {
            return NULL;
        }
    }

    else
    {
#if PACKET_DEBUG == 1
        log_info( "Skipping second stage in 2-stage recv. Header only packet. \n");
#endif
    }

#if PACKET_DEBUG == 1
    log_error( "Incoming: ");
    for (size_t i = 0; i < bytes_read + sizeof(LibPacketHeader); i++)
    {
        log_info( "%x(%u)(%c) ", buf[i], buf[i], buf[i]);
    }
    log_info( "\n");
#endif

    return header;
}

size_t packetFillOutgoingHeader(size_t pdu_size, PacketFlag flag, uint8_t* buf, size_t buf_size)
{
    if (!buf)
    {
        log_error( "Null buf ptr packetFillOutgoingHeaderOnly \n");
        exit(7);
    }

    if (buf_size < sizeof(LibPacketHeader))
    {
        log_error( "Too small buf size packetFillOutgoingHeaderOnly \n");
        exit(9);
    }

    LibPacketHeader* header = (LibPacketHeader*)MAKE_POINTER_AT_OFFSET(buf, 0);
    header->pdu_size_check_endianness = htons((uint16_t)pdu_size);
    header->flag = flag;

    return sizeof(LibPacketHeader);
}

void packetSend(CompatSocket sock, uint8_t* buf, size_t buf_size)
{
#if PACKET_DEBUG == 1
    log_error( "Outgoing: ");
    for (size_t i = 0; i < buf_size; i++)
    {
        log_info( "%x(%u)(%c) ", buf[i], buf[i], buf[i]);
    }
    log_info( "\n");
#endif

    if (compat_send_noflags(sock, buf, buf_size) < 0)
    {
        perror("send failed");
        log_warn( "GONNA JUST PRENTEND IT DIDNT FAIL\n");
    }
}

bool packetFillSendHeaderOnlyPacket(CompatSocket sock, PacketFlag flag)
{
    uint8_t buf[64];
    packetFillOutgoingHeader(sizeof(LibPacketHeader), flag, buf, sizeof(buf));
    packetSend(sock, buf, sizeof(LibPacketHeader));
    return true;
}

// Packet Utility Functions
// Author- Kevin Kellar - 2021