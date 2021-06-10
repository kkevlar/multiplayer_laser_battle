#pragma once

#include "LaserRenderer.h"

struct LaserMangerInternal;

typedef struct
{
    glm::vec4 position_source;
    glm::vec4 position_target;
    glm::vec4 color;
    uint16_t ucid_shooter_check_endianness;
    uint8_t pad[2];
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
    bool shouldDie(glm::vec3 pos, uint8_t my_ucid, float currentTime, uint16_t* killer_ucid);
};
