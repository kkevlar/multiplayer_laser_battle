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
    std::vector<std::string> curr_scoreboard;
};

ScoreManager::ScoreManager()
{
    this->internal = new ScoreManagerInternal;
    this->internal->scoremap = std::map<std::string, int>{};
    this->internal->curr_scoreboard = std::vector<std::string>{};
}

ScoreManager::~ScoreManager()
{
    delete this->internal;
}

    void ScoreManager::admitScore(std::string username, int score )
    {
	    NULLCHECK(this->internal);
	    this->internal->scoremap[username] = score;
    }

    std::vector<std::string> ScoreManager::supplyScoreboardTexts()
    {
NULLCHECK(this->internal);
return this->internal->curr_scoreboard;
    }

    void ScoreManager::maybeUpdateScoreboard(float time)
    {
	    
    }

