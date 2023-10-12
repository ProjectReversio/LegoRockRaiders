#pragma once

#include "platform.h"
#include "config.h"

typedef struct ObjectStats
{
    // TODO: Implement ObjectStats
    F32 TrackDist;
    // TODO: Implement ObjectStats
} ObjectStats, *lpObjectStats;

extern B32 Stats_Initialize(lpConfig config, const char* gameName);

struct LegoObject;
extern F32 StatsObject_GetTrackDist(struct LegoObject* liveObj);
