#pragma once

#include <string>
#include "fdselect.h"

#include "simple_ll_netlib_client.h"

struct NetworksContext
{
    std::string handle;
    void* socketNum;
    void* caller_context;
    NetworksCallback callback;
    FDSelector selector;
};
