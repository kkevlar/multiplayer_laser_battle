#pragma once 

#ifdef _MSC_VER
#include <winsock2.h>
#include <ws2tcpip.h>
#else
#error "Why would you ever include this godforsaken header on nonwindows"
#endif
