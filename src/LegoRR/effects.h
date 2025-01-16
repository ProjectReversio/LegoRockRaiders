#pragma once

#include "platform.h"
#include "container.h"
#include "config.h"

extern void Effect_Load_RockFallStylesAll(lpConfig config, const char* gameName, lpContainer root);

extern void Effect_GetRockFallStyle(const char* name, U32* outIndex);
extern void Effect_SetRockFallStyle(U32 rockFallStyleIndex);
