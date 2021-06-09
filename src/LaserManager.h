#pragma once

#include "LaserRenderer.h"

struct LaserMangerInternal;

typedef struct
{
    glm::vec3 position_source;
    glm::vec3 position_target;
    glm::vec3 color;
    float speed;
    float start_time;
} NewShotLaserInfo;

class LaserManager
{
   private:
    struct LaserManagerInternal* internal;

   public:
    LaserManager();
    ~LaserManager();
    void admitLaser(NewShotLaserInfo* info);
    void renderLasers(glm::mat4& P, glm::mat4& V, glm::vec3 campos, float currentTime, LaserRenderer* laser);
bool shouldDie(glm::vec3 pos, glm::vec3 color, float currentTime);
};
