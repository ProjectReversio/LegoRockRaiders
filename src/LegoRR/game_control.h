#pragma once

#include "common.h"

typedef struct GameControl_Globs
{
    S32 msy_Last_1;
    S32 msx_Last_1;
    Point2F pointf_8;
    U32 reserved1;
    B32 mslb_Last_1;
    B32 renameUseQuotes;
    U32 reserved2;
    B32 typingState_Map[256];
    F32 dbgUpgradeChangeTimer;
    F32 dbgSpeedChangeTimer;
    F32 dbgRollOffChangeTimer;
    F32 sceneFogDelta; // Delta value used in Lego_UpdateSceneFog ((M_PI*2)/fogRate * elapsed).
    B32 handleKeysLastLeftMouseButtonDown; // Tracks mouse released state in Lego_HandleKeys
    F32 dbgCursorLightLevel;
    B32 isGameSpeedLocked; // When this is TRUE, game speed can only be lowered when calling `Lego_SetGameSpeed`.
    B32 mslb_Last_3;
    B32 mslb_Last_4;
    Point2F pointf_144;
    B32 mslb_Last_5;
    B32 mslb_Last_6;
    B32 mslb_Last_0;
    B32 mslr_Last_0;
    F32 handleWorldNoMouseButtonsElapsed; // Duration that neither left or right mouse buttons have been down for (in game time).
    B32 bool_160;
    struct LegoObject* toolTipObject;
    B32 dbgF10InvertLighting;
    B32 dbgF9DisableLightEffects;
    U32 reserved3;
    struct LegoObject* dbgGetInVehicle; // (K debug key, assigned to selected vehicle, pressing K with a rock raider will tell them to get in this registered vehicle)
    Direction direction_178;
    F32 timerTutorialBlockFlash;
} GameControl_Globs;

extern GameControl_Globs gamectrlGlobs;