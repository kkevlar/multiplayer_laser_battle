// Packet Utility Functions
// Author- Kevin Kellar - 2021

#include "packet.h"

#include <arpa/inet.h>
#include <stdio.h>
#include <sys/socket.h>
#include <sys/types.h>

#define PACKET_DEBUG 0

LibPacketHeader* packetFillIncomingUsingDualRecv(int socket_fd, uint8_t* buf, size_t buf_size)
{
    if (!buf)
    {
        fprintf(stderr, "Null buf ptr packetFillIncomingUsingDualRecv\n");
        exit(8);
    }

    LibPacketHeader* header = (LibPacketHeader*)MAKE_POINTER_AT_OFFSET(buf, 0);
    int bytes_read;

    size_t bytes_requested = sizeof(LibPacketHeader);
    if (bytes_requested > buf_size)
    {
        fprintf(stderr,
                "WAY too small buf size passed into "
                "packetFillIncomingUsingDualRecv\n");
        exit(2);
    }

    if ((bytes_read = recv(socket_fd, buf, bytes_requested, MSG_WAITALL)) < 0)
    {
        perror("recv call errored");
        fprintf(stderr, "GONNA JUST PRENTEND IT DIDNT FAIL\n");
    }
    else if (bytes_read < (int)bytes_requested)
    {
        return NULL;
    }

    bytes_requested = ntohs(header->pdu_size_check_endianness) - sizeof(LibPacketHeader);

    if (bytes_requested + sizeof(LibPacketHeader) > buf_size)
    {
        fprintf(stderr,
                "Too small buf size passed into "
                "packetFillIncomingUsingDualRecv\nManually disconnecting this "
                "malicious client\n");
        return NULL;
    }
    else if (bytes_requested > 0)

    {
#if PACKET_DEBUG == 1
        fprintf(stderr, "Gonna block on getting %lu more bytes of data \n", bytes_requested);
#endif
        if ((bytes_read = recv(socket_fd, PACKET_PDU_DATA_SEGMENT(header), bytes_requested, MSG_WAITALL)) < 0)
        {
            perror("recv call2 errored");
            fprintf(stderr, "GONNA JUST PRENTEND IT DIDNT FAIL\n");
        }
        else if (bytes_read < (int)bytes_requested)
        {
            return NULL;
        }
    }

    else
    {
#if PACKET_DEBUG == 1
        fprintf(stderr, "Skipping second stage in 2-stage recv. Header only packet. \n");
#endif
    }

#if PACKET_DEBUG == 1
    fprintf(stderr, "Incoming: ");
    for (size_t i = 0; i < bytes_read + sizeof(LibPacketHeader); i++)
    {
        fprintf(stderr, "%x(%u)(%c) ", buf[i], buf[i], buf[i]);
    }
    fprintf(stderr, "\n");
#endif

    return header;
}

size_t packetFillOutgoingHeader(size_t pdu_size, PacketFlag flag, uint8_t* buf, size_t buf_size)
{
    if (!buf)
    {
        fprintf(stderr, "Null buf ptr packetFillOutgoingHeaderOnly \n");
        exit(7);
    }

    if (buf_size < sizeof(LibPacketHeader))
    {
        fprintf(stderr, "Too small buf size packetFillOutgoingHeaderOnly \n");
        exit(9);
    }

    LibPacketHeader* header = (LibPacketHeader*)MAKE_POINTER_AT_OFFSET(buf, 0);
    header->pdu_size_check_endianness = htons((uint16_t)pdu_size);
    header->flag = flag;

    return sizeof(LibPacketHeader);
}

void packetSend(int fd, uint8_t* buf, size_t buf_size)
{
#if PACKET_DEBUG == 1
    fprintf(stderr, "Outgoing: ");
    for (size_t i = 0; i < buf_size; i++)
    {
        fprintf(stderr, "%x(%u)(%c) ", buf[i], buf[i], buf[i]);
    }
    fprintf(stderr, "\n");
#endif

    if (send(fd, buf, buf_size, 0) < 0)
    {
        perror("send failed");
        fprintf(stderr, "GONNA JUST PRENTEND IT DIDNT FAIL\n");
    }
}

bool packetFillSendHeaderOnlyPacket(int fd, PacketFlag flag)
{
    uint8_t buf[64];
    packetFillOutgoingHeader(sizeof(LibPacketHeader), flag, buf, sizeof(buf));
    packetSend(fd, buf, sizeof(LibPacketHeader));
    return true;
}

// Packet Utility Functions
// Author- Kevin Kellar - 2021