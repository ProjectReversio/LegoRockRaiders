#pragma once

#include "platform.h"
#include "config.h"

extern void Encyclopedia_Initialize(lpConfig config, const char* gameName);

extern void Encyclopedia_Update(F32 elapsedAbs);

extern void Encyclopedia_UnsetFlag1();
