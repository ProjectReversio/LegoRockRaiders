#include "construction.h"

#include "lego_object.h"

void Construction_DisableCryOreDrop(B32 disabled)
{
    // TODO: Implement Construction_DisableCryOreDrop
}

// Immediately spawns a Building object without the need of a construction zone.
lpLegoObject Construction_SpawnBuilding(LegoObject_ID objID, Point2I* originBlockPos, Direction direction, Point2I* shapeBlocks, U32 shapeCount, B32 teleportDown)
{
    // TODO: Implement Construction_SpawnBuilding
}

B32 Construction_Zone_ExistsAtBlock(Point2I* originBlockPos)
{
    // TODO: Implement Construction_Zone_ExistsAtBlock

    return FALSE;
}

void Construction_UpdateAll(F32 elapsedGame)
{
    // TODO: Implement Construction_UpdateAll
}
