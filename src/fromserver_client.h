// Logic for processing messages on the clientside from the server
// Author- Kevin Kellar - 2021

#pragma once

#include <stdbool.h>

#include "packet.h"
#include "simple_ll_netlib_client.h"

CHECK_RETURN_VAL bool fromServerProcessPacket(const LibPacketHeader* const header,
                                                                 NetworksContext* context);

// Logic for processing messages on the clientside from the server
// Author- Kevin Kellar - 2021