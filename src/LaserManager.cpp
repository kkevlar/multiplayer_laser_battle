#include "LaserManager.h"

#include <stdlib.h>
#include <string.h>

#include <iostream>
#include <vector>

#include "PlanesNetworked.h"
#include "byte_order.h"
#include "log.h"

typedef struct
{
    bool valid;
    NewShotLaserInfo info;
} LaserInfoWithValid;

struct LaserManagerInternal
{
    std::vector<LaserInfoWithValid> lasers;
    bool has_invalid = 0;
};

LaserManager::LaserManager()
{
    this->internal = new LaserManagerInternal;
    this->internal->has_invalid = false;
    this->internal->lasers = std::vector<LaserInfoWithValid>{};
}

LaserManager::~LaserManager()
{
    delete this->internal;
}

void LaserManager::admitLaser(NewShotLaserInfo* info)
{
    LaserInfoWithValid toadd_laserinfo;

    toadd_laserinfo.valid = true;
    memcpy(&toadd_laserinfo.info, info, sizeof(NewShotLaserInfo));

again:
    if (!this->internal->has_invalid)
    {
        this->internal->lasers.push_back(toadd_laserinfo);
    }
    else
    {
        // This block reclaims memory from the vec thats invalid
        size_t i = 0;
        for (i = 0; i < this->internal->lasers.size(); i++)
        {
            if (!this->internal->lasers.at(i).valid) break;
        }

        if (i >= this->internal->lasers.size())
        {
            this->internal->has_invalid = false;
            goto again;
        }

        memcpy(&this->internal->lasers.data()[i], &toadd_laserinfo, sizeof(toadd_laserinfo));
    }
}

void LaserManager::renderLasers(glm::mat4& P, glm::mat4& V, glm::vec3 campos, float currentTime, LaserRenderer* laser)
{
    for (auto& info : this->internal->lasers)
    {
        if (info.valid)
        {
            glm::vec3 along_route = trunc_v4(info.info.position_target) - trunc_v4(info.info.position_source);
            float ratio_to_target = (currentTime - info.info.start_time) * info.info.speed;
            glm::vec3 where = trunc_v4(info.info.position_source) + ratio_to_target * along_route;
            laser->renderLaser(P,
                               V,
                               campos,
                               where,
                               info.info.position_target,
                               trunc_v4(info.info.color),
                               currentTime - info.info.start_time);

            if (ratio_to_target > 1.0f)
            {
                info.valid = false;
                this->internal->has_invalid = true;
            }
        }
    }
}

bool LaserManager::shouldDie(glm::vec3 pos, uint8_t my_ucid, float currentTime, uint16_t* killer_ucid)
{
    NULLCHECK(killer_ucid);

    for (auto& info : this->internal->lasers)
    {
        glm::vec3 along_route = trunc_v4(info.info.position_target) - trunc_v4(info.info.position_source);
        float ratio_to_target = (currentTime - info.info.start_time) * info.info.speed;
        glm::vec3 where = trunc_v4(info.info.position_source) + ratio_to_target * along_route;

        const uint16_t safe_laser_ucid = ntohs(info.info.ucid_shooter_check_endianness);

        if (info.valid && my_ucid != safe_laser_ucid && length(where - pos) < 10)
        {
            info.valid = false;
            this->internal->has_invalid = true;
            *killer_ucid = safe_laser_ucid;
            return true;
        }
    }
    return false;
}
