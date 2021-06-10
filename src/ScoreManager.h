#pragma once

struct ScoreMangerInternal;

#include <string>
#include <vector>

class ScoreManager
{
   private:
    struct ScoreManagerInternal* internal;

   public:
    ScoreManager();
    ~ScoreManager();
    void admitScore(std::string username, int score);
    std::vector<std::string> supplyScoreboardTexts();
    void maybeUpdateScoreboard(float time);
};
