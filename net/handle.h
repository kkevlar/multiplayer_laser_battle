// Handle-related logic
// Author- Kevin Kellar - 2021
#pragma once

#include <string>

#include "packet.h"
#define HANDLE_MAX (100)

__attribute__((warn_unused_result)) bool handleCheckValid(std::string handle);

__attribute__((warn_unused_result)) const std::string clientHandleGetFromSimpleHandlePacket(
    const LibPacketHeader* const header);

__attribute__((warn_unused_result)) const std::string clientHandleGetFromDataFragment(const uint8_t* const frag,
                                                                                      size_t* bytes_unpacked);

__attribute__((warn_unused_result)) bool clientHandleFillSimplePacket(
    PacketFlag flag, const std::string handle, uint8_t* buf, size_t* filled_size, size_t buf_size);

__attribute__((warn_unused_result)) bool clientHandleFillDataFragment(const std::string handle,
                                                                      uint8_t* buf,
                                                                      size_t* filled_size,
                                                                      size_t buf_size);

// Handle-related logic
// Author- Kevin Kellar - 2021