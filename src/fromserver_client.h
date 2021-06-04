// Logic for processing messages on the clientside from the server
// Author- Kevin Kellar - 2021

#pragma once

#include <stdbool.h>

#include "packet.h"
#include "simple_ll_netlib_client.h"

__attribute__((warn_unused_result)) bool fromServerProcessPacket(const LibPacketHeader* const header,
                                                                 NetworksHandle* context);

// Logic for processing messages on the clientside from the server
// Author- Kevin Kellar - 2021
