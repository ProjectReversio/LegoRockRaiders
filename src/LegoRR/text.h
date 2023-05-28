#pragma once

#include "types.h"
#include "text_window.h"

extern void Text_Initialize(const char* filename, U32 param2, U32 param3, U32 unusedInt, Area2F* param5, Area2F* unusedRect, F32 param7);
extern void Text_Load(lpTextWindow textWindow1, lpTextWindow textWindow2, S32 imageX, S32 imageY, F32 pauseTime);
