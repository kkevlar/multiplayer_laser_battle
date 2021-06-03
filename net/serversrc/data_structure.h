// Global dynamic datastructure for tracking clients on the server
// Author- Kevin Kellar - 2021

#pragma once

#include <list>
#include <string>
#include <vector>
#include "log.h"

typedef struct
{
    int unique_color_id;
} PersistentInfo;

void clientFDListAddFd(int fd);
CHECK_RETURN_VAL int clientFDListIndexof(int fd);
CHECK_RETURN_VAL bool clientFDListCheckExistence(int fd);
CHECK_RETURN_VAL bool clientFDListRemoveFd(int fd);
CHECK_RETURN_VAL const std::list<int> clientFdListGetAll();
CHECK_RETURN_VAL bool clientMapCheckHandleExistence(
    const std::string handle);
CHECK_RETURN_VAL PersistentInfo clientMapAddHandle(const std::string handle, int fd);
CHECK_RETURN_VAL bool clientMapDeleteHandle(
    const std::string handle);
CHECK_RETURN_VAL int clientMapFindByHandle(
    const std::string handle);
CHECK_RETURN_VAL bool clientMapDoesFdExist(int fd);
CHECK_RETURN_VAL const std::string clientMapFindByFD(int fd);
CHECK_RETURN_VAL const std::vector<std::string> clientMapGetAllHandles();

// Global dynamic datastructure for tracking clients on the server
// Author- Kevin Kellar - 2021