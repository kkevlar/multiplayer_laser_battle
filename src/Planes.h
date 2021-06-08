#pragma once

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

using namespace std;
using namespace glm;

class player
{
   public:
    vec3 pos;
    vec3 velocity_cached;
    vec3 forward, up, right;
    float speed;
    bool w, a, s, d;

    player();
    void update(float ftime, float xangle, float yangle);
};
