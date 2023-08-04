#pragma once

#include "platform.h"
#include "text_window.h"
#include "lego.h"

extern void Objective_Update(lpTextWindow textWnd, lpLego_Level level, F32 elapsedGamed, F32 elapsedAbs);

extern void Objective_SetEndTeleportEnabled(B32 on);
