// Logic for processing messages on the clientside from the server
// Author- Kevin Kellar - 2021

#pragma once

#include <stdbool.h>

#include "log.h"
#include "packet.h"
#include "simple_ll_netlib_client.h"

CHECK_RETURN_VAL bool fromServerProcessPacket(const LibPacketHeader* const header, NetworksHandle* context);

// Logic for processing messages on the clientside from the server
// Author- Kevin Kellar - 2021
