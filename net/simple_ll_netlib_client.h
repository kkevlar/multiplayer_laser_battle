#pragma once

#include <string>

typedef bool (*NetworksCallback)(void*, std::string, const uint8_t*, size_t);

struct NetworksContext;
typedef struct
{
    struct NetworksContext* net_context;
} NetworksHandle;

__attribute__((warn_unused_result)) bool publicClientInitialize(const char* handle_AKA_name,
                                                                const char* host_name,
                                                                const char* port_num,
                                                                void* caller_context,
                                                                NetworksCallback callback,
                                                                NetworksHandle** out_handle);
__attribute__((warn_unused_result)) bool publicClientPollSelectForMessages(NetworksHandle* handle);
__attribute__((warn_unused_result)) bool publicBroadcastOutgoing(NetworksHandle* handle,
                                                                 uint8_t* data,
                                                                 size_t data_size);
