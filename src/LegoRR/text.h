#pragma once

#include "platform.h"
#include "text_window.h"

typedef enum Text_Type
{
    Text_Walk                = 0,
    Text_Drill               = 1,
    Text_NoDrill             = 2,
    Text_CantDrill           = 3,
    Text_Reinforce           = 4,
    Text_NoReinforce         = 5,
    Text_CantReinforce       = 6,
    Text_PickupCrystal       = 7,
    Text_Encyclopedia        = 8,
    Text_RadarHelp           = 9,
    Text_BuildHelp           = 10,
    Text_SelectionHelp       = 11,
    Text_Drive               = 12,
    Text_CantDrive           = 13,
    Text_MakeTeleporter      = 14,
    Text_CannotPlaceBuilding = 15,
    Text_CrystalOreDisabled  = 16,
    Text_CrystalFound        = 17,
    Text_CavernDiscovered    = 18,
    Text_OreFound            = 19,
    Text_AirSupplyLow        = 20,
    Text_AirSupplyRunningOut = 21,
    Text_SpaceToContinue     = 22,
    Text_GameCompleted       = 23,
    Text_ManTrained          = 24,
    Text_UnitUpgraded        = 25,
    Text_Type_Count          = 26,
} Text_Type;

extern void Text_Initialize(const char* filename, U32 param2, U32 param3, U32 unusedInt, Area2F* param5, Area2F* unusedRect, F32 param7);
extern void Text_Load(lpTextWindow textWindow1, lpTextWindow textWindow2, S32 imageX, S32 imageY, F32 pauseTime);

extern void Text_DisplayMessage(Text_Type textType, B32 changeTiming, B32 setFlag4);

extern void Text_Update(F32 elapsedAbs);
