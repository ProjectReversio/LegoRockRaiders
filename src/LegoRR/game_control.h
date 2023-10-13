#pragma once

#include "common.h"

typedef struct GameControl_Globs
{
    // TODO: Implement GameControl_Globs
    B32 isGameSpeedLocked; // When this is TRUE, game speed can only be lowered when calling `Lego_SetGameSpeed`.
    // TODO: Implement GameControl_Globs
} GameControl_Globs;

extern GameControl_Globs gamectrlGlobs;