#pragma once

#include <stdbool.h>

#include "packet.h"

bool __attribute__((warn_unused_result)) incomingPacket(const LibPacketHeader* const header, int clientSockFd);
bool __attribute__((warn_unused_result))
incomingClientHandleProposal(const LibPacketHeader* const header, int clientSockFd);
