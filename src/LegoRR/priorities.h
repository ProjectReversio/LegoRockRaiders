#pragma once

#include "platform.h"
#include "config.h"

extern void Priorities_LoadPositions(lpConfig config, const char* gameName);
extern void Priorities_LoadImages(lpConfig config, const char* gameName);

extern void Priorities_Draw();

extern B32 Priorities_LoadLevel(lpConfig config, const char* gameName, const char* levelName);

extern B32 Priorities_HandleInput(U32 mouseX, U32 mouseY, B32 leftButton, B32 leftButtonLast, B32 leftReleased);
