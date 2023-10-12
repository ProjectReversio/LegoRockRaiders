#include "stats.h"
#include "lego_object.h"

B32 Stats_Initialize(lpConfig config, const char* gameName)
{
    // TODO: Implement Stats_Initialize
    return FALSE;
}

F32 StatsObject_GetTrackDist(lpLegoObject liveObj)
{
    return liveObj->stats->TrackDist;
}
