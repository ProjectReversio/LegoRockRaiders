#pragma once

#include "platform.h"
#include "config.h"

typedef enum StatsFlags1
{
    STATS1_NONE                  = 0,
    STATS1_SINGLEWIDTHDIG        = 0x1,
    STATS1_PROCESSORE            = 0x2,
    STATS1_PROCESSCRYSTAL        = 0x4,
    STATS1_STOREOBJECTS          = 0x8,
    STATS1_SMALLTELEPORTER       = 0x10,
    STATS1_BIGTELEPORTER         = 0x20,
    STATS1_WATERTELEPORTER       = 0x40,
    STATS1_COLLRADIUS            = 0x80,
    STATS1_COLLBOX               = 0x100,
    STATS1_CAUSESLIP             = 0x200,
    STATS1_RANDOMMOVE            = 0x400,
    STATS1_CANSCARE              = 0x800,
    STATS1_RANDOMENTERWALL       = 0x1000,
    STATS1_SCAREDBYPLAYER        = 0x2000,
    STATS1_SNAXULIKE             = 0x4000,
    STATS1_GRABMINIFIGURE        = 0x8000,
    STATS1_CANCLEARRUBBLE        = 0x10000,
    STATS1_CANBEDRIVEN           = 0x20000,
    STATS1_CANSCARESCORPION      = 0x40000,
    STATS1_CANSTEAL              = 0x80000,
    STATS1_TOOLSTORE             = 0x100000,
    STATS1_FLOCKS                = 0x200000,
    STATS1_FLOCKS_DEBUG          = 0x400000,
    STATS1_FLOCKS_SMOOTH         = 0x800000,
    STATS1_CROSSWATER            = 0x1000000,
    STATS1_CROSSLAND             = 0x2000000,
    STATS1_FLOCKS_ONGROUND       = 0x4000000,
    STATS1_FLOCKS_QUICKDESTROY   = 0x8000000,
    STATS1_FLOCKS_ANIMATEBYPITCH = 0x10000000,
    STATS1_ROUTEAVOIDANCE        = 0x20000000,
    STATS1_BUMPDAMAGE            = 0x40000000,
    STATS1_MANTELEPORTER         = 0x80000000,
} StatsFlags1;

typedef enum StatsFlags2
{
    STATS2_NONE                   = 0,
    STATS2_SCAREDBYBIGBANGS       = 0x1,
    STATS2_UPGRADEBUILDING        = 0x2,
    STATS2_TRAINPILOT             = 0x4,
    STATS2_TRAINSAILOR            = 0x8,
    STATS2_TRAINDRIVER            = 0x10,
    STATS2_TRAINDYNAMITE          = 0x20,
    STATS2_TRAINREPAIR            = 0x40,
    STATS2_TRAINSCANNER           = 0x80,
    STATS2_TRACKER                = 0x100,
    STATS2_GENERATEPOWER          = 0x200,
    STATS2_SELFPOWERED            = 0x400,
    STATS2_POWERBUILDING          = 0x600,
    STATS2_USEBIGTELEPORTER       = 0x800,
    STATS2_USESMALLTELEPORTER     = 0x1000,
    STATS2_USEWATERTELEPORTER     = 0x2000,
    STATS2_ATTACKPATHS            = 0x4000,
    STATS2_SPLITONZEROHEALTH      = 0x8000,
    STATS2_CANBEHITBYFENCE        = 0x10000,
    STATS2_CANDOUBLESELECT        = 0x20000,
    STATS2_CANBESHOTAT            = 0x40000,
    STATS2_DRAINPOWER             = 0x80000,
    STATS2_USEHOLES               = 0x100000,
    STATS2_CROSSLAVA              = 0x200000,
    STATS2_USELEGOMANTELEPORTER   = 0x400000,
    STATS2_DAMAGECAUSESCALLTOARMS = 0x800000,
    STATS2_CANFREEZE              = 0x1000000,
    STATS2_CANLASER               = 0x2000000,
    STATS2_CANPUSH                = 0x4000000,
    STATS2_DONTSHOWDAMAGE         = 0x8000000,
    STATS2_REMOVEREINFORCEMENT    = 0x10000000,
    STATS2_DONTSHOWONRADAR        = 0x20000000,
    STATS2_INVISIBLEDRIVER        = 0x40000000,
    STATS2_UNSELECTABLE           = 0x80000000,
} StatsFlags2;

typedef enum StatsFlags3
{
    STATS3_NONE                = 0,
    STATS3_CARRYVEHICLES       = 0x1,
    STATS3_VEHICLECANBECARRIED = 0x2,
    STATS3_CANSTRAFE           = 0x4,
    STATS3_CLASSASLARGE        = 0x8,
    STATS3_GETOUTATLAND        = 0x10,
    STATS3_GETINATLAND         = 0x20,
    STATS3_TAKECARRYINGDRIVERS = 0x40,
    STATS3_SHOWHEALTHBAR       = 0x80,
    STATS3_NEEDSPILOT          = 0x100,
    STATS3_ENTERTOOLSTORE      = 0x200,
} StatsFlags3;

typedef struct ObjectStats
{
    // TODO: Implement ObjectStats

    F32 TrackDist;

    // TODO: Implement ObjectStats

    F32 RandomMoveTime; // (default: 200.0)
    StatsFlags1 flags1;
    StatsFlags2 flags2;
    StatsFlags3 flags3;

} ObjectStats, *lpObjectStats;

extern B32 Stats_Initialize(lpConfig config, const char* gameName);

struct LegoObject;
extern F32 StatsObject_GetTrackDist(struct LegoObject* liveObj);

extern B32 StatsObject_SetObjectLevel(struct LegoObject* liveObj, U32 newLevel);

extern StatsFlags1 StatsObject_GetStatsFlags1(struct LegoObject* liveObj);
extern StatsFlags2 StatsObject_GetStatsFlags2(struct LegoObject* liveObj);
extern StatsFlags3 StatsObject_GetStatsFlags3(struct LegoObject* liveObj);
