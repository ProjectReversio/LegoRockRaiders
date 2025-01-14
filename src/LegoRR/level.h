#pragma once

#include "platform.h"
#include "lego.h"

extern B32 Level_DestroyWall(lpLego_Level level, U32 bx, U32 by, B32 isHiddenCavern);

extern void Level_BlockUpdateSurface(lpLego_Level level, S32 bx, S32 by, B32 reserved);
