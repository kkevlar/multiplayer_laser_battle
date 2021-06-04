// Kevin Kellar's FD Selector - 2021

#include "fdselect.h"
#include "log.h"

#include <stdlib.h>
#include <string.h>

#include <iostream>
#include "log.h"


    void fd_selector_clearFds(FDSelectorContext* context)
{
    NULLCHECK(context);
    FD_ZERO(&context->set);
    context->max_fd = 0;
}

void fd_selector_addFd(FDSelectorContext* context, CompatSocket fd)
{
    NULLCHECK(context);


#ifdef _MSC_VER
    FD_SET(fd, &context->set);
#else
    FD_SET(fd.unix_socket, &context->set);
    if (fd.unix_socket > context->max_fd)
    {
        context->max_fd = fd.unix_socket;
    }
#endif


}

bool fd_selector_performSelect(FDSelectorContext* context, long timeout_ms)
{
    NULLCHECK(context);
    struct timeval time;
    struct timeval* time_ptr = &time;

time.tv_sec = 0;
time.tv_usec = 0;

    if (timeout_ms > 0)
    {
        time.tv_sec = timeout_ms / 1000;
        time.tv_usec = (timeout_ms - time.tv_sec * 1000) * 1000;
    }
    else if (timeout_ms < 0)
    {
        time_ptr = NULL;
    }

    int rett = select(context->max_fd + 1, &context->set, NULL, NULL, time_ptr);

    return rett >= 0;
}

bool fd_selector_testPostSelectMembership(FDSelectorContext* context,CompatSocket fd)
{
    NULLCHECK(context);
#ifdef _MSC_VER
    return (FD_ISSET(fd.unix_socket, &context->set));
#else
    return (FD_ISSET(fd.unix_socket, &context->set));
#endif
}

