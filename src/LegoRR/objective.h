#pragma once

#include "platform.h"
#include "text_window.h"
#include "lego.h"

extern void Objective_SetStatus(LevelStatus status);

extern void Objective_Update(lpTextWindow textWnd, lpLego_Level level, F32 elapsedGamed, F32 elapsedAbs);

extern void Objective_SetEndTeleportEnabled(B32 on);

extern void Objective_LoadLevel(lpConfig config, const char* gameName, const char* levelName, struct Lego_Level* level, U32 screenWidth, U32 screenHeight);
