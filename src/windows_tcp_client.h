#pragma once

#include "compat.c"
#include <stdbool.h >

#ifdef __cplusplus
extern "C"
{
#endif

bool winTcpClientSetup(const char* serverName, const char* port, CompatSocket* compat);
#ifdef __cplusplus
}
#endif