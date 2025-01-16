#pragma once

#include "platform.h"
#include "fonts.h"

extern void Info_Initialize(lpFont font);

extern void Info_SetOverFlowImageFile(const char* filename);

extern void Info_DrawPanel(F32 elapsedAbs);

extern void Info_Draw(F32 elapsedAbs);

extern void Info_SetFlag4(B32 state);

// TODO: Should this be in info.h?
extern void ScrollInfo_Update(B32 index);
