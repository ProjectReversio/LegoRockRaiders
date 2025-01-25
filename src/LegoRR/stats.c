#include "stats.h"
#include "lego_object.h"

B32 Stats_Initialize(lpConfig config, const char* gameName)
{
    // TODO: Implement Stats_Initialize
    return FALSE;
}

F32 StatsObject_GetTrackDist(lpLegoObject liveObj)
{
    if (liveObj->stats == NULL) // TEMP: This check not in original code, remove for accuracy later
        return 0.0f;

    return liveObj->stats->TrackDist;
}

B32 StatsObject_SetObjectLevel(struct LegoObject* liveObj, U32 newLevel)
{
    // TODO: Implement StatsObject_SetObjectLevel

    return FALSE;
}

StatsFlags2 StatsObject_GetStatsFlags2(struct LegoObject* liveObj)
{
    if (liveObj->stats == NULL) // TEMP: This check not in original code, remove for accuracy later
        return STATS2_NONE;

    return liveObj->stats->flags2;
}
