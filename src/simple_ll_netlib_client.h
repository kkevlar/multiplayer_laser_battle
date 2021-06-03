#pragma once

#include <string>
#include "log.h"
typedef bool (*NetworksCallback)(void*, std::string, const uint8_t*, size_t);

struct NetworksContext;
typedef struct
{
    struct NetworksContext* net_context;
} NetworksHandle;

CHECK_RETURN_VAL bool publicClientInitialize(const char* handle_AKA_name,
                                                                const char* host_name,
                                                                const char* port_num,
                                                                void* caller_context,
                                                                NetworksCallback callback,
                                                                NetworksHandle** out_handle,
                                                                uint16_t* out_ucid,
                                                                uint32_t* out_ms_elapsed);

CHECK_RETURN_VAL bool publicClientPollSelectForMessages(NetworksHandle* handle);
CHECK_RETURN_VAL bool publicBroadcastOutgoing(NetworksHandle* handle,
                                                                 uint8_t* data,
                                                                 size_t data_size);
