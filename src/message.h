#pragma once

#include <inttypes.h>
#include <stdbool.h>
#include <stdlib.h>

#include <list>
#include <string>

#include "packet.h"
#include "log.h"

#define MAX_RECIPS_FOR_MESSAGE (9)

CHECK_RETURN_VAL bool messageFillOutgoingPacket(PacketFlag flag,
                                                                   std::string sender_handle,
                                                                   uint8_t* outgoing_data,
                                                                   size_t outgoing_data_size,
                                                                   uint8_t* buf,
                                                                   size_t* out_total_bytes_used,
                                                                   size_t buf_size);