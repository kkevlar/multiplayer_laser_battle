// Kevin Kellar's FD Selector - 2021

#include "fdselect.h"

#include <string.h>
#include <sys/select.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>

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
    FD_ZERO(&this->internal->set);
    this->internal->max_fd = 0;
}

void FDSelector::addFd(int fd)
{
    FD_SET(fd, &this->internal->set);

    if (fd > this->internal->max_fd)
    {
        this->internal->max_fd = fd;
    }
}

bool FDSelector::performSelect(long timeout_ms)
{
    struct timeval time;
    struct timeval* time_ptr = &time;

    memset(&time, 0, sizeof(time));

    if (timeout_ms > 0)
    {
        time.tv_sec = timeout_ms / 1000;
        time.tv_usec = (timeout_ms - time.tv_sec * 1000) * 1000;
    }
    else if (timeout_ms < 0)
    {
        time_ptr = NULL;
    }

    return select(this->internal->max_fd + 1, &this->internal->set, NULL, NULL, time_ptr) >= 0;
}

bool FDSelector::testPostSelectMembership(int fd)
{
    return (FD_ISSET(fd, &this->internal->set));
}

// Kevin Kellar's FD Selector - 2021