#pragma once

#include "platform.h"
#include "lego_object.h"

extern void AITask_Initialize();

extern void AITask_Game_SetNoGather(B32 noGather);

extern void AITask_DoCollect(lpLegoObject liveObj, F32 param2);
