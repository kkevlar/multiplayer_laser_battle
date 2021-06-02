
#include "compat.h"

#ifdef _MSC_VER

#include "windows_net_stuff.h"

#else
#include <sys/socket.h>
#include <sys/types.h>
#endif



#ifdef __cplusplus
#error
#endif

size_t compat_recv_waitall(CompatSocket sockfd, void* buf, size_t len)
{
    
#ifdef _MSC_VER

    return recv(sockfd.ms_socket, buf, len, MSG_WAITALL);

#else
    return recv(sockfd.unix_socket, buf, len, MSG_WAITALL);
#endif
    
}

size_t compat_send_noflags(CompatSocket sockfd, void* buf, size_t len)
{
#ifdef _MSC_VER

    return send(sockfd.ms_socket, buf, len, 0);

#else
    return send(sockfd.unix_socket, buf, len, 0);
#endif

}
