// Kevin Kellar's FD Selector - 2021
#pragma once

#ifdef _MSC_VER
#include <winsock2.h>
#else
#include <sys/select.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>
#endif

#include "compat.h"

struct FDSelectorContext
{
    fd_set set;
    int max_fd;
};

    void fd_selector_clearFds(FDSelectorContext* context);
    void fd_selector_addFd(FDSelectorContext* context, CompatSocket fd);

    // -1 for forever, 0 for no timeout
    // Returns false on error
    bool fd_selector_performSelect(FDSelectorContext*context,long  timeout_ms);

    bool fd_selector_testPostSelectMembership(FDSelectorContext* context,  CompatSocket fd);

// Kevin Kellar's FD Selector - 2021