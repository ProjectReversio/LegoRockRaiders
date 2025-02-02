#pragma once

#include "platform.h"
#include "lego.h"
#include "search.h"

extern B32 Level_DestroyWall(lpLego_Level level, U32 bx, U32 by, B32 isHiddenCavern);

extern Lego_SurfaceType Level_Block_ChoosePathTexture(S32 bx, S32 by, U8* ref_direction, B32 powered);

extern void Level_BlockUpdateSurface(lpLego_Level level, S32 bx, S32 by, B32 reserved);

extern void Level_Block_LowerRoofVertices(lpLego_Level level, U32 bx, U32 by);

extern B32 Level_AddCryOreToToolStore(lpLegoObject liveObj, SearchAddCryOre_c* search);

extern void Level_SetPointer_FromSurfaceType(Lego_SurfaceType8 surfaceType);

extern B32 Level_BlockPointerCheck(Point2I* blockPos);

extern B32 Level_FindSelectedUnit_BlockCheck_FUN_00431960(U32 bx, U32 by, B32 param3);

extern B32 Level_Block_IsImmovable(Point2I* blockPos);
extern B32 Level_Block_IsLava(Point2I* blockPos);
extern B32 Level_Block_IsSurveyed(U32 bx, U32 by);

extern void Level_BlockActivity_UpdateAll(lpLego_Level level, F32 elapsedGame);

extern void Level_UpdateEffects(lpLego_Level level, F32 elapsedGame);
