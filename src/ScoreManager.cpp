#include "ScoreManager.h"

#include <stdlib.h>
#include <string.h>

#include <iostream>
#include <vector>
#include <string>
#include <map>

#include "log.h"

struct ScoreManagerInternal
{
    std::map<std::string, int> scoremap;
};

ScoreManager::ScoreManager()
{
    this->internal = new ScoreManagerInternal;
    this->internal->scoremap = std::map<std::string, int>{};
}

ScoreManager::~ScoreManager()
{
    delete this->internal;
}

