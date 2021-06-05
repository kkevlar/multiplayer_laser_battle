
#include "compat.h"
#include "log.h"

#ifdef _MSC_VER

#include "windows_net_stuff.h"
#include <stdio.h>

#else
#include <sys/socket.h>
#include <sys/types.h>
#endif


size_t compat_recv_waitall(CompatSocket sockfd, void* buf, size_t len)
{
    
#ifdef _MSC_VER

    return recv(sockfd.ms_socket, (char*)buf, len, 0);

#else
    return recv(sockfd.unix_socket, buf, len, MSG_WAITALL);
#endif
    
}

size_t compat_send_noflags(CompatSocket sockfd, void* buf, size_t len)
{

#ifdef _MSC_VER
    int iResult;

        iResult = send( sockfd.ms_socket, (const char*) buf, len, 0 );
    if (iResult == SOCKET_ERROR) {
        printf("send failed with error: %d\n", WSAGetLastError());
        closesocket(sockfd.ms_socket);
        WSACleanup();
        return 0;
    }
    return len;

#else

    return send(sockfd.unix_socket, buf, len, 0);
#endif

}
