
#include "PlanesNetworked.h"

#include <iostream>
#include <map>
#include <vector>

#include "LaserManager.h"
#include "byte_order.h"
#include "log.h"
#include "simple_ll_netlib_client.h"

#define NETWORK_PORT "55555"
#define FASTEST_BROADCAST_FREQ_ALLOWED_HZ (10.0f)
#define PLANES_BIG_MAGIC_VALUE (0x75)
#define PLANES_POS_MAGIC_VALUE1 (0x9b)
#define PLANES_LASER_MAGIC_VALUE (0xbf)
#define MAX_LASERS_TO_BUFFER (64)

using namespace std;
using namespace glm;

#define FLAG_POSITION (1)
#define FLAG_SHOOT_LASER (2)
#define FLAG_HITCONFIRM (3)

typedef struct
{
    vec4 position;
    vec4 velocity;
    quat orientation;
    vec4 color;
    uint8_t magic;
    uint8_t bool_is_dead;
    uint16_t my_score_check_endianness;
} PlanePositionInfoPDU;

typedef struct
{
    PlanePositionInfoPDU pdu;
    float timeUpdated;
} PlanePositionInfoWithTime;

typedef struct
{
    NewShotLaserInfo laser;
    uint16_t magic;
} ShootLaserPDU;

typedef struct
{
    ShootLaserPDU buf[MAX_LASERS_TO_BUFFER];
    int start;
    int count;
} IncomingLaserBufferer;

typedef struct
{
    uint8_t pad[6];
    uint16_t shotby_check_endianess;
} KillConfirmPDU;

typedef union
{
    PlanePositionInfoPDU plane_pos;
    ShootLaserPDU laser;
    KillConfirmPDU shoot_confirm;
} UnionPDU;

typedef struct
{
    uint32_t data_size_check_endianess;
    uint8_t big_magic;
    uint8_t flag;
    uint8_t pad[2];
    UnionPDU data;
} PlanePDU;

typedef struct
{
    glm::vec4 color;
    int score;
} CachedScoreInfo;

struct PlanesNetworkedInternal
{
    NetworksHandle net_handle;
    map<string, PlanePositionInfoWithTime> position_map;
    map<string, int> all_scores;
    float last_poll_time;
    float last_broadcast_time;
    IncomingLaserBufferer laser_buffer;
    int my_ucid = 0;
    int unclaimed_hits = 0;
};

PlanesNetworked::PlanesNetworked()
{
    this->internal = new PlanesNetworkedInternal;
    NULLCHECK(this->internal);
    memset(&this->internal->net_handle, 0, sizeof(this->internal->net_handle));
    this->internal->position_map = map<string, PlanePositionInfoWithTime>{};
    this->internal->all_scores = map<string, int>{};
    this->internal->last_poll_time = 0;
    this->internal->last_broadcast_time = 0;
    this->internal->laser_buffer.count = 0;
    this->internal->laser_buffer.start = 0;
    this->internal->unclaimed_hits = 0;
}

PlanesNetworked::~PlanesNetworked()
{
    delete this->internal;
}

static bool handlePositionUpdate(PlanesNetworkedInternal* internal, string handle, PlanePositionInfoPDU* planepospdu)
{
    NULLCHECK(planepospdu);
    PlanePositionInfoWithTime ppiwt;
    ppiwt.timeUpdated = internal->last_poll_time;
    memcpy(&ppiwt.pdu, planepospdu, sizeof(PlanePositionInfoPDU));

    internal->position_map[handle] = ppiwt;
    return true;
}

static bool handleShootLaser(PlanesNetworkedInternal* internal, string handle, ShootLaserPDU* laserpdu)
{
    NULLCHECK(laserpdu);

    if (internal->laser_buffer.count > MAX_LASERS_TO_BUFFER)
    {
        log_fatal("bug");
        exit(99);
    }
    else if (internal->laser_buffer.count == MAX_LASERS_TO_BUFFER)
    {
        log_warn("Too many lasers! Shooting too fast! Dropping this laser ");
        return true;
    }
    else
    {
        int safeindex = (internal->laser_buffer.count + internal->laser_buffer.start) % MAX_LASERS_TO_BUFFER;
        memcpy(&internal->laser_buffer.buf[safeindex], &laserpdu->laser, sizeof(NewShotLaserInfo));
        internal->laser_buffer.count += 1;

        return true;
    }
}

static void mismatched_pdu_sizes(int flag, size_t expect, size_t actual)
{
    log_fatal("Mismatched PDU sizes. Expected %zu, Actual %zu for flag %d", expect, actual, flag);
    exit(12);
}

static bool internal_callback(void* context, std::string handle, const uint8_t* memory, size_t mem_size)
{
    NULLCHECK(context);
    NULLCHECK(memory);
    NULLCHECK(mem_size);

    PlanesNetworked* me = (PlanesNetworked*)context;
    PlanesNetworkedInternal* internal = me->internal;
    NULLCHECK(internal);

    PlanePDU* plane_pdu = (PlanePDU*)memory;
    const uint32_t safe_data_size = ntohl(plane_pdu->data_size_check_endianess);

    if (plane_pdu->big_magic != PLANES_BIG_MAGIC_VALUE)
    {
        log_fatal("Bad big magic value %x from handle %s.", plane_pdu->big_magic, handle.c_str());
        exit(8);
    }

    if (plane_pdu->flag == FLAG_POSITION)
    {
        if (safe_data_size != sizeof(PlanePositionInfoPDU))
        {
            mismatched_pdu_sizes(plane_pdu->flag, sizeof(PlanePositionInfoPDU), safe_data_size);
        }

        PlanePositionInfoPDU* planepos = &plane_pdu->data.plane_pos;
        if (planepos->magic != PLANES_POS_MAGIC_VALUE1)
        {
            log_fatal("Bad pos magic1 value %x from handle %s.", planepos->magic, handle.c_str());
            exit(8);
        }
        return handlePositionUpdate(internal, handle, planepos);
    }
    else if (plane_pdu->flag == FLAG_SHOOT_LASER)
    {
        if (safe_data_size != sizeof(ShootLaserPDU))
        {
            mismatched_pdu_sizes(plane_pdu->flag, sizeof(ShootLaserPDU), safe_data_size);
        }

        ShootLaserPDU* laser = &plane_pdu->data.laser;
        if (laser->magic != PLANES_LASER_MAGIC_VALUE)
        {
            log_fatal("Bad shoot magic value %d from handle %s.", laser->magic, handle.c_str());
            exit(8);
        }
        return handleShootLaser(internal, handle, laser);
    }
    else if (plane_pdu->flag == FLAG_HITCONFIRM)
    {
        if (safe_data_size != sizeof(KillConfirmPDU))
        {
            mismatched_pdu_sizes(plane_pdu->flag, sizeof(KillConfirmPDU), safe_data_size);
        }

        if (ntohs(plane_pdu->data.shoot_confirm.shotby_check_endianess) == internal->my_ucid)
        {
            internal->unclaimed_hits += 1;
        }
    }

    log_error("Unknown flag %x from handle %s", plane_pdu->flag, handle.c_str());
    return false;
}

void PlanesNetworked::PlanesNetworkedSetup(const char* username,
                                           const char* hostname,
                                           uint16_t* ucid,
                                           uint32_t* ms_elapsed)
{
    NULLCHECK(this);
    NULLCHECK(this->internal);
    NULLCHECK(username);
    NULLCHECK(hostname);
    NULLCHECK(NETWORK_PORT);
    NULLCHECK(ucid);
    NULLCHECK(ms_elapsed);

    if (!publicClientInitialize(
            username, hostname, NETWORK_PORT, this, internal_callback, &this->internal->net_handle, ucid, ms_elapsed))
    {
        exit(7);
    }

    this->internal->my_ucid = *ucid;
}

void PlanesNetworked::BroadcastSelfPosition(
    float time, glm::vec3 pos, glm::vec3 velocity, glm::quat rot, glm::vec3 color, bool is_dead, uint16_t score)
{
    NULLCHECK(this->internal);
    if (time - this->internal->last_broadcast_time > (1.0f / FASTEST_BROADCAST_FREQ_ALLOWED_HZ))

    {
        PlanePositionInfoPDU pp;
        pp.orientation = rot;
        pp.position = vec4(pos, 0);
        pp.velocity = vec4(velocity, 0);
        pp.color = vec4(color, 0);
        pp.bool_is_dead = is_dead;
        pp.magic = PLANES_POS_MAGIC_VALUE1;
        pp.my_score_check_endianness = htons(score);

        UnionPDU un;
        un.plane_pos = pp;

        PlanePDU pdu;
        pdu.data = un;
        pdu.data_size_check_endianess = htonl(sizeof(PlanePositionInfoPDU));
        pdu.flag = FLAG_POSITION;
        pdu.big_magic = PLANES_BIG_MAGIC_VALUE;

        if (!publicBroadcastOutgoing(&this->internal->net_handle, (uint8_t*)&pdu, sizeof(pdu)))
        {
            log_fatal("Failed to broadcast outgoing");
            exit(-1);
        }

        this->internal->last_broadcast_time = time;
    }
}

void PlanesNetworked::BroadcastNewLaser(NewShotLaserInfo* laser)
{
    NULLCHECK(this->internal);
    NULLCHECK(laser);

    UnionPDU un;
    memcpy(&un.laser.laser, laser, sizeof(NewShotLaserInfo));
    un.laser.magic = PLANES_LASER_MAGIC_VALUE;

    PlanePDU pdu;
    pdu.data = un;
    pdu.data_size_check_endianess = htonl(sizeof(ShootLaserPDU));
    pdu.flag = FLAG_SHOOT_LASER;
    pdu.big_magic = PLANES_BIG_MAGIC_VALUE;

    if (!publicBroadcastOutgoing(&this->internal->net_handle, (uint8_t*)&pdu, sizeof(pdu)))
    {
        log_fatal("Failed to broadcast outgoing");
        exit(-1);
    }
}

void PlanesNetworked::BroadcastKillConfirmation(uint16_t ucid_of_killer_host_order)
{
    NULLCHECK(this->internal);

    KillConfirmPDU killpdu;
    memset(&killpdu, 0, sizeof(killpdu));
    killpdu.shotby_check_endianess = htons(ucid_of_killer_host_order);

    PlanePDU pdu;
    pdu.data.shoot_confirm = killpdu;
    pdu.data_size_check_endianess = htonl(sizeof(KillConfirmPDU));
    pdu.flag = FLAG_HITCONFIRM;
    pdu.big_magic = PLANES_BIG_MAGIC_VALUE;

    if (!publicBroadcastOutgoing(&this->internal->net_handle, (uint8_t*)&pdu, sizeof(pdu)))
    {
        log_fatal("Failed to broadcast outgoing");
        exit(-1);
    }
}

void PlanesNetworked::PollIncoming(float time)
{
    NULLCHECK(this->internal);
    this->internal->last_poll_time = time;
    if (!publicClientPollSelectForMessages(&this->internal->net_handle))
    {
        log_fatal("Failed to poll for incoming messages");
        exit(-1);
    }
}

glm::vec3 trunc_v4(glm::vec4 four)
{
    return vec3(four.x, four.y, four.z);
}

std::vector<NetworkedInterpolatedPlanePositionInfo> PlanesNetworked::GiveOtherPlaneEstimates(float time)
{
    NULLCHECK(this->internal);
    vector<NetworkedInterpolatedPlanePositionInfo> ret;
    for (const pair<string, PlanePositionInfoWithTime>& element : this->internal->position_map)
    {
        vec3 velocity = vec3(0);

        float difftime = time - element.second.timeUpdated;

        if (!element.second.pdu.bool_is_dead)
        {
            velocity = trunc_v4(element.second.pdu.velocity);
        }

        vec3 pos = trunc_v4(element.second.pdu.position) + difftime * velocity;
        quat rot = element.second.pdu.orientation;

        vec3 color = trunc_v4(element.second.pdu.color);
        NetworkedInterpolatedPlanePositionInfo interpinfo = {pos,
                                                             rot,
                                                             color,
                                                             element.first,
                                                             (bool)element.second.pdu.bool_is_dead,
                                                             ntohs(element.second.pdu.my_score_check_endianness)};
        ret.push_back(interpinfo);
    }

    return ret;
}

bool PlanesNetworked::MaybePopIncomingNetworkedLaser(NewShotLaserInfo* laser)
{
    NULLCHECK(laser);

    if (this->internal->laser_buffer.count < 0)
    {
        log_fatal("bug");
        exit(80);
    }
    else if (this->internal->laser_buffer.count == 0)
    {
        return false;
    }
    else
    {
        memcpy(laser, &this->internal->laser_buffer.buf[this->internal->laser_buffer.start], sizeof(NewShotLaserInfo));
        this->internal->laser_buffer.start += 1;
        this->internal->laser_buffer.start %= MAX_LASERS_TO_BUFFER;
        this->internal->laser_buffer.count -= 1;

        return true;
    }
}

int PlanesNetworked::PopUnclaimedKillCount()
{
    NULLCHECK(this->internal);

    int ret = this->internal->unclaimed_hits;

    this->internal->unclaimed_hits = 0;
    return ret;
}
