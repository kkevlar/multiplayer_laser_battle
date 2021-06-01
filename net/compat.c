
#include "compat.h"

#include <sys/socket.h>
#include <sys/types.h>

#ifdef __cplusplus
#error
#endif 

size_t compat_recv_waitall(int sockfd, void* buf, size_t len)
{
    return recv(sockfd, buf, len, MSG_WAITALL);
}
