#pragma once

#include "types.h"
#include "fonts.h"

extern void ToolTip_Initialize(lpFont font, S32 value2, S32 value1, F32 float1, U32 width, U32 height, S32 value32, F32 red, F32 green, F32 blue);

extern void ToolTip_Update(U32 mousex, U32 mousey, F32 elapsedAbs);
