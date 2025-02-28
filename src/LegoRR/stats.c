#include "stats.h"
#include "lego_object.h"

Stats_Globs statsGlobs = { NULL };

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

B32 StatsObject_SetObjectLevel(lpLegoObject liveObj, U32 newLevel)
{
    LegoObject_Type objType = liveObj->type;
    S32 objIndex = liveObj->id;
    if (newLevel < statsGlobs.objectLevels[objType][objIndex])
    {
        U32 oldLevel = liveObj->objLevel;
        if (newLevel != oldLevel)
        {
            objectGlobs.objectPrevLevels[objType][objIndex][oldLevel]++;
            objectGlobs.objectTotalLevels[objType][objIndex][newLevel]--;
        }

        liveObj->objLevel = newLevel;
        liveObj->stats = statsGlobs.objectStats[liveObj->type][liveObj->id] + newLevel;
        return TRUE;
    }

    return FALSE;
}

StatsFlags1 StatsObject_GetStatsFlags1(lpLegoObject liveObj)
{
    if (liveObj->stats == NULL) // TEMP: This check not in original code, remove for accuracy later
        return STATS1_NONE | STATS1_CROSSLAND; // TEMP: cross land for testing

    return liveObj->stats->flags1;
}

StatsFlags2 StatsObject_GetStatsFlags2(lpLegoObject liveObj)
{
    if (liveObj->stats == NULL) // TEMP: This check not in original code, remove for accuracy later
        return STATS2_NONE;

    return liveObj->stats->flags2;
}

StatsFlags3 StatsObject_GetStatsFlags3(lpLegoObject liveObj)
{
    if (liveObj->stats == NULL) // TEMP: This check not in original code, remove for accuracy later
        return STATS3_NONE;

    return liveObj->stats->flags3;
}
