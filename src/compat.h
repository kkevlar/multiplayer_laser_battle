// RECV COMPAT BY KEVIN KELLAR

#pragma once

#include <stdlib.h>

typedef struct {
#ifdef _MSC_VER
void* ms_socket;
#else
    int unix_socket;
    #endif
} CompatSocket;

#ifdef __cplusplus
extern "C"
{
#endif

    size_t compat_recv_waitall(CompatSocket sockfd, void* buf, size_t len);
size_t compat_send_noflags(CompatSocket, void* buf, size_t len);

#ifdef __cplusplus
}
#endif
