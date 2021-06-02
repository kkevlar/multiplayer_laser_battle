
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

size_t compat_recv_waitall(int sockfd, void* buf, size_t len)
{
    
    return recv(sockfd, buf, len, MSG_WAITALL);
    
}
