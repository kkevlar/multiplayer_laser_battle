#include "ScoreManager.h"

#include <stdlib.h>
#include <string.h>

#include <algorithm>
#include <iostream>
#include <map>
#include <memory>
#include <string>
#include <vector>

#include "log.h"

#define FASTEST_SORT_HZ (1.0f)

typedef struct
{
    uint32_t score;
    uint32_t legacy;
} ScoreInfo;

typedef struct
{
    ScoreInfo info;
    std::string name;
} ScoreInfoBigger;

struct ScoreManagerInternal
{
    std::map<std::string, ScoreInfo> scoremap;
    std::vector<std::string> curr_scoreboard;
    float last_sort_time;
};

ScoreManager::ScoreManager()
{
    this->internal = new ScoreManagerInternal;
    this->internal->scoremap = std::map<std::string, ScoreInfo>{};
    this->internal->curr_scoreboard = std::vector<std::string>{};
    this->internal->last_sort_time = 0;
}

ScoreManager::~ScoreManager()
{
    delete this->internal;
}

static bool compareBigs(ScoreInfoBigger a, ScoreInfoBigger b)
{
    if (a.info.score > b.info.score)
    {
        return true;
    }
    else if (b.info.score > a.info.score)
    {
        return false;
    }
    else
    {
        if (a.info.legacy > b.info.legacy)
        {
            return true;
        }
        else if (b.info.legacy > a.info.legacy)
        {
            return false;
        }
        else
        {
            return strcmp(a.name.c_str(), b.name.c_str()) < 0;
        }
    }
}

void ScoreManager::admitScore(std::string username, int score)
{
    NULLCHECK(this->internal);

    if (this->internal->scoremap.find(username) == this->internal->scoremap.end())
    {
        this->internal->scoremap[username].score == score;
    }
    else
    {
        ScoreInfo si;
        si.score = score;
        si.legacy = 0;
        this->internal->scoremap[username] = si;
    }
}

std::vector<std::string> ScoreManager::supplyScoreboardTexts()
{
    NULLCHECK(this->internal);
    return this->internal->curr_scoreboard;
}

void ScoreManager::maybeUpdateScoreboard(float time)
{
    NULLCHECK(this->internal);
    if (time - this->internal->last_sort_time > (1.0f / FASTEST_SORT_HZ))
    {
        this->internal->last_sort_time = time;
        std::vector<ScoreInfoBigger> myvector;

        auto it = this->internal->scoremap.begin();
        while (it != this->internal->scoremap.end())
        {
            auto& ss = it->second;
            ss.legacy += 1;
            it++;
        }

        for (const std::pair<std::string, ScoreInfo>& element : this->internal->scoremap)
        {
            ScoreInfoBigger big;
            big.info = element.second;
            big.name = element.first;
            myvector.push_back(big);
        }

        sort(myvector.begin(), myvector.end(), compareBigs);

        while (myvector.size() > 10)
        {
            myvector.pop_back();
        }

        std::vector<std::string> svec{};
        for (auto v : myvector)
        {
            std::string mystring{};
            mystring.clear();

            for (int i = 0; i < 8; i++)
            {
                if (i >= v.name.size())
                {
                    mystring.push_back(' ');
                }
                else
                {
                    char c = v.name.at(i);
                    mystring.push_back(c);
                }
            }

            char bruh[64];
            snprintf(bruh, 5, "%02d", v.info.score);

            std::string bb{bruh};
            svec.push_back(mystring + bb);
        }

        this->internal->curr_scoreboard = svec;
    }
}
