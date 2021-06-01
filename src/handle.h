// Handle-related logic
// Author- Kevin Kellar - 2021
#pragma once

#include <string>

#include "packet.h"
#include "log.h"
#define HANDLE_MAX (100)

CHECK_RETURN_VAL bool handleCheckValid(std::string handle);

CHECK_RETURN_VAL const std::string clientHandleGetFromSimpleHandlePacket(
    const LibPacketHeader* const header);

CHECK_RETURN_VAL const std::string clientHandleGetFromDataFragment(const uint8_t* const frag,
                                                                                      size_t* bytes_unpacked);

CHECK_RETURN_VAL bool clientHandleFillSimplePacket(
    PacketFlag flag, const std::string handle, uint8_t* buf, size_t* filled_size, size_t buf_size);

CHECK_RETURN_VAL bool clientHandleFillDataFragment(const std::string handle,
                                                                      uint8_t* buf,
                                                                      size_t* filled_size,
                                                                      size_t buf_size);

// Handle-related logic
// Author- Kevin Kellar - 2021