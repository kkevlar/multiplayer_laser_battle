#pragma once

#include <string>
#include "fdselect.h"
#include "compat.h"

#include "simple_ll_netlib_client.h"

struct NetworksContext
{
    std::string handle;
    CompatSocket socketNum;
    void* caller_context;
    NetworksCallback callback;
    FDSelector selector;
};
