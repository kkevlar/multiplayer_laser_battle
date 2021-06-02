// Kevin Kellar's FD Selector - 2021
#pragma once

#include "compat.h"

struct FDSelectorInternal;

class FDSelector
{
   private:
    struct FDSelectorInternal* internal;

   public:
    FDSelector();
    ~FDSelector();
    void clearFds();
    void addFd(CompatSocket fd);

    // -1 for forever, 0 for no timeout
    // Returns false on error
    bool performSelect(long timeout_ms);

    bool testPostSelectMembership(CompatSocket fd);
};
// Kevin Kellar's FD Selector - 2021