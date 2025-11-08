#pragma once

#include "platform.h"
#include "lego.h"
#include "search.h"

typedef enum CrossTerrainType //: U32
{
    CrossTerrainType_CantCross = 0,
    CrossTerrainType_CanCross = 1,
    CrossTerrainType_Diagonal = 2,
    CrossTerrainType_Wall = 3,
} CrossTerrainType;

extern B32 Level_DestroyWall(lpLego_Level level, U32 bx, U32 by, B32 isPredug);
extern B32 Level_DestroyWallConnection(lpLego_Level level, U32 bx, U32 by);

extern void Level_UncoverHiddenCavern(U32 bx, U32 by);

// Increases damage on the block, used in mining
extern B32 Level_Block_Damage(U32 bx, U32 by, F32 param3, F32 param4);

extern Lego_SurfaceType Level_Block_ChoosePathTexture(S32 bx, S32 by, U8* ref_direction, B32 powered);

extern void Level_BlockUpdateSurface(lpLego_Level level, S32 bx, S32 by, B32 reserved);

extern void Level_Block_LowerRoofVertices(lpLego_Level level, U32 bx, U32 by);

extern B32 Level_AddCryOreToToolStore(lpLegoObject liveObj, SearchAddCryOre_c* search);

extern void Level_SetPointer_FromSurfaceType(Lego_SurfaceType8 surfaceType);

extern B32 Level_BlockPointerCheck(Point2I* blockPos);

extern B32 Level_FindSelectedUnit_BlockCheck_FUN_00431960(U32 bx, U32 by, B32 param3);

extern B32 Level_Block_IsImmovable(Point2I* blockPos);
extern B32 Level_Block_IsLava(Point2I* blockPos);
extern B32 Level_Block_IsPath(Point2I* blockPos);

// Generally used to tell Rock Monsters where they can't go.
extern B32 Level_Block_IsFoundationOrBusyFloor(Point2I* blockPos);

extern B32 Level_Block_IsSurveyed(U32 bx, U32 by);
extern B32 Level_Block_IsWall(U32 bx, U32 by);

extern void Level_Block_SetBusy(Point2I* blockPos, B32 state);

extern void Level_BlockActivity_UpdateAll(lpLego_Level level, F32 elapsedGame);

// NOTE: This does not DIRECTLY remove the block activity. But it will subsequently mark it for
// removal in the next UpdateAll loop.
extern void Level_BlockActivity_Destroy(lpLego_Level level, Point2I blockPos, B32 wallDestroyed);

extern void Level_UpdateEffects(lpLego_Level level, F32 elapsedGame);

// 0 = Can't Cross, 1 = Can Cross (floor), 2 = diagonal, 3 = wall
extern CrossTerrainType Lego_GetCrossTerrainType(struct LegoObject* liveObj, S32 bx1, S32 by1, S32 bx2, S32 by2, B32 param6);
