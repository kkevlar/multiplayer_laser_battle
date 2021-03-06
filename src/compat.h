// RECV COMPAT BY KEVIN KELLAR

#pragma once

#include <stdlib.h>
#ifdef _MSC_VER
#include "windows_net_stuff.h"
#endif

typedef struct
{
#ifdef _MSC_VER
    SOCKET ms_socket;
#else
    int unix_socket;
#endif
} CompatSocket;

#ifdef __cplusplus
extern "C"
{
#endif

    size_t compat_recv_waitall(CompatSocket sockfd, void* buf, size_t len);
    size_t compat_send_noflags(CompatSocket sock, void* buf, size_t len);

#ifdef __cplusplus
}
#endif
