// Global dynamic datastructure for tracking clients on the server
// Author- Kevin Kellar - 2021

#pragma once

#include <list>
#include <string>
#include <vector>

typedef struct
{
    int unique_color_id;
} PersistentInfo;

void clientFDListAddFd(int fd);
__attribute__((warn_unused_result)) int clientFDListIndexof(int fd);
__attribute__((warn_unused_result)) bool clientFDListCheckExistence(int fd);
__attribute__((warn_unused_result)) bool clientFDListRemoveFd(int fd);
__attribute__((warn_unused_result)) const std::list<int> clientFdListGetAll();
__attribute__((warn_unused_result)) bool clientMapCheckHandleExistence(
    const std::string handle);
__attribute__((warn_unused_result)) PersistentInfo clientMapAddHandle(const std::string handle, int fd);
__attribute__((warn_unused_result)) bool clientMapDeleteHandle(
    const std::string handle);
__attribute__((warn_unused_result)) int clientMapFindByHandle(
    const std::string handle);
__attribute__((warn_unused_result)) bool clientMapDoesFdExist(int fd);
__attribute__((warn_unused_result)) const std::string clientMapFindByFD(int fd);
__attribute__((warn_unused_result)) const std::vector<std::string> clientMapGetAllHandles();

// Global dynamic datastructure for tracking clients on the server
// Author- Kevin Kellar - 2021