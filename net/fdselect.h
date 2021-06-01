#pragma once

struct FDSelectorInternal;

class FDSelector
{
   private:
    struct FDSelectorInternal* internal;

   public:
    FDSelector();
    ~FDSelector();
    void clearFds();
    void addFd(int fd);

    // -1 for forever, 0 for no timeout
    // Returns false on error
    bool performSelect(long timeout_ms);

    bool testPostSelectMembership(int fd);
};