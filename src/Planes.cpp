#include "Planes.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

// TODO
#define MAX_SPD 240.0f
#define MIN_SPD 100.0f
// #define MAX_SPD 2.0f
// #define MIN_SPD 0.5f
#define MED_SPD (MAX_SPD + MIN_SPD) / 2.0f

#define MAP_X_MAX_BOUND 1000.0f
#define MAP_Y_MAX_BOUND 1000.0f
#define MAP_Z_MAX_BOUND 1000.0f

#define MAP_X_MIN_BOUND -1000.0f
#define MAP_Y_MIN_BOUND 0.0f
#define MAP_Z_MIN_BOUND -1000.0f

using namespace std;
using namespace glm;

player::player()
{
    w = a = s = d = false;
    pos = vec3(0, 180, 20);
    speed = MIN_SPD;

    forward = vec3(0, 0, 1);
    up = vec3(0, 1, 0);
    right = vec3(1, 0, 0);
    score = 0;
}

void player::update(float ftime, float xangle, float yangle)
{
    if (w)
    {
        speed += 100 * ftime;
    }
    if (s) speed -= 100 * ftime;
    speed = clamp(speed, MIN_SPD, MAX_SPD);

    float zangle = 0;
    if (a) zangle = 1.3f * ftime;
    if (d) zangle = -1.3f * ftime;

    mat4 rotate_x = rotate(mat4(1), xangle, right);
    mat4 rotate_y = rotate(mat4(1), yangle, up);
    mat4 rotate_z = rotate(mat4(1), zangle, forward);
    mat4 R = rotate_z * rotate_y * rotate_x;

    forward = normalize(vec3(R * vec4(forward.x, forward.y, forward.z, 1)));
    up = normalize(vec3(R * vec4(up.x, up.y, up.z, 1)));
    right = normalize(vec3(R * vec4(right.x, right.y, right.z, 1)));

    velocity_cached = forward * speed;
    pos += velocity_cached * ftime;

    if (pos.x > MAP_X_MAX_BOUND) pos.x = MAP_X_MAX_BOUND;
    if (pos.y > MAP_Y_MAX_BOUND) pos.y = MAP_Y_MAX_BOUND;
    if (pos.z > MAP_Z_MAX_BOUND) pos.z = MAP_Z_MAX_BOUND;

    if (pos.x < MAP_X_MIN_BOUND) pos.x = MAP_X_MIN_BOUND;
    if (pos.y < MAP_Y_MIN_BOUND) pos.y = MAP_Y_MIN_BOUND;
    if (pos.z < MAP_Z_MIN_BOUND) pos.z = MAP_Z_MIN_BOUND;
}

// class npc
// {
//    public:
//     vec3 pos;
//     vec3 velocity_cached;
//     vec3 forward, up, right;
//     float speed;

//     npc()
//     {
//         pos = vec3(0, 180, 20);
//         speed = MIN_SPD;

//         forward = vec3(0, 0, 1);
//         up = vec3(0, 1, 0);
//         right = vec3(1, 0, 0);
//     }

//     void update(float ftime, vec3 target)
//     {
//         float xangle, yangle, zangle;
//         xangle = yangle = zangle = 0;

//         vec3 to_targ = target - pos;
//         float distance = length(to_targ);

//         if (distance > 400)
//             speed += 100 * ftime;
//         else if (distance > 200)
//         {
//             if (speed > MED_SPD)
//                 speed -= 100 * ftime;
//             else
//                 speed += 100 * ftime;
//         }
//         else
//         {
//             speed -= 100 * ftime;
//             to_targ = to_targ + 20.0f * up;
//         }

//         speed = clamp(speed, MIN_SPD, MAX_SPD);

//         float updown = dot(to_targ, up);
//         float leftright = dot(to_targ, right);

//         float right_w_trueup = dot(right, vec3(0, 1, 0));

//         if (updown > 0)
//             xangle = -1.0f * ftime;
//         else if (updown < 0)
//             xangle = 1.0f * ftime;
//         if (leftright > 0)
//             yangle = 1.0f * ftime;
//         else if (leftright < 0)
//             yangle = -1.0f * ftime;

//         if (right_w_trueup > 0) zangle = -1.0f * ftime;
//         if (right_w_trueup < 0) zangle = 1.0f * ftime;

//         mat4 rotate_x = rotate(mat4(1), xangle, right);
//         mat4 rotate_y = rotate(mat4(1), yangle, up);
//         mat4 rotate_z = rotate(mat4(1), zangle, forward);
//         mat4 R = rotate_z * rotate_y * rotate_x;

//         forward = normalize(vec3(R * vec4(forward.x, forward.y, forward.z, 1)));
//         up = normalize(vec3(R * vec4(up.x, up.y, up.z, 1)));
//         right = normalize(vec3(R * vec4(right.x, right.y, right.z, 1)));

//         velocity_cached = forward * speed;
//         pos += velocity_cached * ftime;

//         if (pos.x > MAP_X_MAX_BOUND) pos.x = MAP_X_MAX_BOUND;
//         if (pos.y > MAP_Y_MAX_BOUND) pos.y = MAP_Y_MAX_BOUND;
//         if (pos.z > MAP_Z_MAX_BOUND) pos.z = MAP_Z_MAX_BOUND;

//         if (pos.x < MAP_X_MIN_BOUND) pos.x = MAP_X_MIN_BOUND;
//         if (pos.y < MAP_Y_MIN_BOUND) pos.y = MAP_Y_MIN_BOUND;
//         if (pos.z < MAP_Z_MIN_BOUND) pos.z = MAP_Z_MIN_BOUND;
//     }
// };

// TODO rename setup bots
// void setup_players()
// {
//     for (int i = 0; i < NUM_BOTS; i++)
//     {
//         npc bot;

//         // bot.pos = vec3(bot.pos.x * i, 200, bot.pos.z);
//         bot.pos = theplayer.pos + vec3(10) * ((float)i);

//         // float rho, theta;
//         // rho = 5.0f;
//         // theta = (i * 2 * PI) / NUM_BOTS;
//         // // bot.pos = vec3(acos(theta) * rho, (i % 2) ? 150 : 250, asin(theta) * rho);
//         // bot.pos = vec3(acos(theta) * rho, 200, asin(theta) * rho);
//         thebots.push_back(bot);
//     }
// }
