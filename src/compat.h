// RECV COMPAT BY KEVIN KELLAR

#pragma once

#include <stdlib.h>

#ifdef __cplusplus
extern "C"
{
#endif

    size_t compat_recv_waitall(void* sockfd, void* buf, size_t len);

#ifdef __cplusplus
}
#endif
