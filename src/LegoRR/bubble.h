#pragma once

#include "platform.h"
#include "config.h"

extern void Bubble_Initialize();

extern void Bubble_LoadBubbles(lpConfig config);

extern void Bubble_Unk_DrawObjectUIs_FUN_004074d0(F32 elapsedAbs);

extern void Bubble_LiveObject_MiniFigure_FUN_00407380(struct LegoObject* liveObj);
