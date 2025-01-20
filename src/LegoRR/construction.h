#pragma once

#include "platform.h"
#include "lego_object.h"

// Disables dropping resource costs for an object when teleported up. Generally used at the end of a level.
extern void Construction_DisableCryOreDrop(B32 disabled);

// Immediately spawns a Building object without the need of a construction zone.
extern lpLegoObject Construction_SpawnBuilding(LegoObject_ID objID, Point2I* originBlockPos, Direction direction, Point2I* shapeBlocks, U32 shapeCount, B32 teleportDown);
