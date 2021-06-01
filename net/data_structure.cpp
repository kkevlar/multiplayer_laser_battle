// Global dynamic datastructure for tracking clients on the server
// Author- Kevin Kellar - 2021

#include "data_structure.h"

#include <iostream>
#include <map>

using namespace std;

typedef struct
{
    int fd;
} ClientInfo;

#define DATA_STRUCTURE_DEBUG 0

static map<string, ClientInfo> handleMap{};
static list<int> fdList{};

void clientFDListAddFd(int fd)
{
#if DATA_STRUCTURE_DEBUG == 1
    cerr << "Adding fd=" << fd << " to fdlist" << endl;
#endif
    fdList.push_back(fd);
}

int clientFDListIndexof(int fd)
{
    int index = 0;
    for (const int& listfd : fdList)
    {
        if (listfd == fd)
        {
            return index;
        }
        index += 1;
    }
    return -1;
}

bool clientFDListCheckExistence(int fd)
{
    if (clientFDListIndexof(fd) < 0)
    {
        return false;
    }
    else
    {
        return true;
    }
}

bool clientFDListRemoveFd(int fd)
{
#if DATA_STRUCTURE_DEBUG == 1
    cerr << "Attempting to remove fd=" << fd << " from fdlist" << endl;
#endif

    int index = clientFDListIndexof(fd);
    if (index < 0)
    {
        return false;
    }

    fdList.remove(fd);
    return true;
}

const std::list<int> clientFdListGetAll() { return fdList; }

bool clientMapCheckHandleExistence(const string handle)
{
    return handleMap.find(handle) != handleMap.end();
}

void clientMapAddHandle(const string handle, int fd)
{
    ClientInfo info{fd};
    handleMap[handle] = info;
}

bool clientMapDeleteHandle(const string handle)
{
    if (!clientMapCheckHandleExistence(handle))
    {
        return false;
    }

    handleMap.erase(handle);
    return true;
}

int clientMapFindByHandle(const string handle)
{
    if (!clientMapCheckHandleExistence(handle))
    {
        return -1;
    }

    return handleMap[handle].fd;
}

bool clientMapDoesFdExist(int fd)
{
    for (const auto& pair : handleMap)
    {
        if (pair.second.fd == fd)
        {
            return true;
        }
    }
    return false;
}

const string clientMapFindByFD(int fd)
{
    for (const auto& pair : handleMap)
    {
        if (pair.second.fd == fd)
        {
            return pair.first;
        }
    }
    return string{};
}

const vector<string> clientMapGetAllHandles()
{
    vector<string> toReturn{};
    for (const auto& pair : handleMap)
    {
        toReturn.push_back(pair.first);
    }
    return toReturn;
}

// Global dynamic datastructure for tracking clients on the server
// Author- Kevin Kellar - 2021