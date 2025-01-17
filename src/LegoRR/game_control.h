#pragma once

#include "common.h"

typedef struct GameControl_Globs
{
    // TODO: Implement GameControl_Globs
    F32 sceneFogDelta; // Delta value used in Lego_UpdateSceneFog ((M_PI*2)/fogRate * elapsed).
    B32 handleKeysLastLeftMouseButtonDown; // Tracks mouse released state in Lego_HandleKeys
    F32 dbgCursorLightLevel;
    B32 isGameSpeedLocked; // When this is TRUE, game speed can only be lowered when calling `Lego_SetGameSpeed`.
    // TODO: Implement GameControl_Globs
} GameControl_Globs;

extern GameControl_Globs gamectrlGlobs;