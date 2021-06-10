#pragma once

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <string>
#include <vector>

// Really sad we need this include im a bad coder
#include "LaserManager.h"

struct PlanesNetworkedInternal;

typedef struct
{
    glm::vec3 pos;
    glm::quat rot;
    glm::vec3 color;
    std::string username;
    bool is_dead;

} NetworkedInterpolatedPlanePositionInfo;

class PlanesNetworked
{
   public:
    struct PlanesNetworkedInternal* internal;
    PlanesNetworked();
    ~PlanesNetworked();
    void PlanesNetworkedSetup(const char* username, const char* hostname, uint16_t* out_ucid, uint32_t* out_ms_elapsed);
    void BroadcastSelfPosition(
        float time, glm::vec3 pos, glm::vec3 velocity, glm::quat rot, glm::vec3 color, bool is_dead);
    void BroadcastNewLaser(NewShotLaserInfo* laser);
    void PollIncoming(float time);
    bool MaybePopIncomingNetworkedLaser(NewShotLaserInfo* laser);

    std::vector<NetworkedInterpolatedPlanePositionInfo> GiveOtherPlaneEstimates(float time);
};

glm::vec3 trunc_v4(glm::vec4 four);
