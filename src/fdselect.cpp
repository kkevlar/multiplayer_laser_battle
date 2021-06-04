// Kevin Kellar's FD Selector - 2021

#include "fdselect.h"

#ifdef _MSC_VER
#include <winsock2.h>

#else
#include <sys/select.h>
#include <sys/time.h>
#include <unistd.h>

#endif

#include <stdlib.h>
#include <string.h>
#include <sys/types.h>


#include <iostream>
#include "log.h"

struct FDSelectorInternal
{
    fd_set set;
    int max_fd;
};

FDSelector::FDSelector()
{
    this->internal = new FDSelectorInternal;
}

FDSelector::~FDSelector()
{
    delete this->internal;
}

void FDSelector::clearFds()
{
    if (!this->internal)
    {
        std::cerr << "Bad0" << std::endl;
        exit(99);
    }
    FD_ZERO(&this->internal->set);
    this->internal->max_fd = 0;
}

void FDSelector::addFd(CompatSocket fd)
{
    if (!this->internal)
    {
        std::cerr << "Bad1" << std::endl;
        exit(99);
    }
#ifdef _MSC_VER

      FD_SET(fd.ms_socket, &this->internal->set);
#else
    FD_SET(fd.unix_socket, &this->internal->set);

#endif

#ifdef _MSC_VER
this->internal->max_fd = 30;
#else
    if (fd.unix_socket > this->internal->max_fd)
    {
        this->internal->max_fd = fd.unix_socket;
    }

#endif
}

bool FDSelector::performSelect(long timeout_ms)
{
    if (!this->internal)
    {
        std::cerr << "Bad2" << std::endl;
        exit(99);
    }
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

log_trace("Calling select with maxfd %d", this->internal->max_fd+1);
    return select(this->internal->max_fd + 1, &this->internal->set, NULL, NULL, time_ptr) >= 0;
}

bool FDSelector::testPostSelectMembership(CompatSocket fd)
{
    if (!this->internal)
    {
        std::cerr << "Bad3" << std::endl;
        exit(99);
    }
#ifdef _MSC_VER
    return (FD_ISSET(fd.ms_socket, &this->internal->set));
#else
    return (FD_ISSET(fd.unix_socket, &this->internal->set));

#endif
}

// Kevin Kellar's FD Selector - 2021