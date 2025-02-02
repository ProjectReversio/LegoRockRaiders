#pragma once

#include "platform.h"
#include "fonts.h"
#include "front_end.h"

extern void ToolTip_Initialize(lpFont font, S32 value2, S32 value1, F32 float1, U32 width, U32 height, S32 value32, F32 red, F32 green, F32 blue);

extern void ToolTip_Update(U32 mousex, U32 mousey, F32 elapsedAbs);

extern void ToolTip_AddFlag4(ToolTip_Type toolTipType);

extern B32 ToolTip_GetType(const char* toolTipName, ToolTip_Type* toolTipType);

extern void ToolTip_SetFlag10(ToolTip_Type toolTipType, B32 state);

extern void ToolTip_ResetTimer(ToolTip_Type toolTipType);

extern void ToolTip_SetText(ToolTip_Type toolTipType, const char* msg, ...);

extern void ToolTip_AddIcon(ToolTip_Type toolTipType, lpImage image);
