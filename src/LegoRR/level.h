#pragma once

#include "platform.h"
#include "lego.h"
#include "search.h"

extern B32 Level_DestroyWall(lpLego_Level level, U32 bx, U32 by, B32 isHiddenCavern);

extern void Level_BlockUpdateSurface(lpLego_Level level, S32 bx, S32 by, B32 reserved);

extern void Level_Block_LowerRoofVertices(lpLego_Level level, U32 bx, U32 by);

extern B32 Level_AddCryOreToToolStore(lpLegoObject liveObj, SearchAddCryOre_c* search);

extern void Level_SetPointer_FromSurfaceType(Lego_SurfaceType8 surfaceType);

extern B32 Level_BlockPointerCheck(Point2I* blockPos);
