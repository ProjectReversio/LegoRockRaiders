#pragma once

#include "platform.h"
#include "config.h"

typedef enum Pointer_Type
{
    Pointer_Standard                 = 0,
    Pointer_Blank                    = 1,
    Pointer_Selected                 = 2,
    Pointer_Drill                    = 3,
    Pointer_CantDrill                = 4,
    Pointer_Clear                    = 5,
    Pointer_Go                       = 6,
    Pointer_CantGo                   = 7,
    Pointer_Teleport                 = 8,
    Pointer_CantTeleport             = 9,
    Pointer_Reinforce                = 10,
    Pointer_CantReinforce            = 11,
    Pointer_RadarPan                 = 12,
    Pointer_TrackObject              = 13,
    Pointer_Help                     = 14,
    Pointer_CantHelp                 = 15,
    Pointer_PutDown                  = 16,
    Pointer_GetIn                    = 17,
    Pointer_GetOut                   = 18,
    Pointer_TutorialBlockInfo        = 19,
    Pointer_Okay                     = 20,
    Pointer_NotOkay                  = 21,
    Pointer_CanBuild                 = 22,
    Pointer_CannotBuild              = 23,
    Pointer_Dynamite                 = 24,
    Pointer_CantDynamite             = 25,
    Pointer_PickUp                   = 26,
    Pointer_CantPickUp               = 27,
    Pointer_PickUpOre                = 28,
    Pointer_LegoManCantDig           = 29,
    Pointer_VehicleCantDig           = 30,
    Pointer_LegoManDig               = 31,
    Pointer_VehicleDig               = 32,
    Pointer_LegoManCantPickUp        = 33,
    Pointer_VehicleCantPickUp        = 34,
    Pointer_LegoManPickUp            = 35,
    Pointer_VehiclePickUp            = 36,
    Pointer_LegoManCantGo            = 37,
    Pointer_VehicleCantGo            = 38,
    Pointer_LegoManGo                = 39,
    Pointer_VehicleGo                = 40,
    Pointer_LegoManClear             = 41,
    Pointer_VehicleClear             = 42,
    Pointer_SurfaceType_Immovable    = 43,
    Pointer_SurfaceType_Hard         = 44,
    Pointer_SurfaceType_Medium       = 45,
    Pointer_SurfaceType_Loose        = 46,
    Pointer_SurfaceType_Soil         = 47,
    Pointer_SurfaceType_Lava         = 48,
    Pointer_SurfaceType_Water        = 49,
    Pointer_SurfaceType_OreSeam      = 50,
    Pointer_SurfaceType_Lake         = 51,
    Pointer_SurfaceType_CrystalSeam  = 52,
    Pointer_SurfaceType_RechargeSeam = 53,
    Pointer_CantZoom                 = 54,
    Pointer_Zoom                     = 55,
    Pointer_Type_Count               = 56,
} Pointer_Type;

extern void Pointer_Initialize();

/* Load all Pointers in the CFG block (prop is the first child in the block)
   USAGE: <PointerType>  <bmpPath>
   USAGE: <PointerType>  <flicPath>,<num1>,<num2>
*/
extern void Pointer_LoadPointers(lpConfig config);

extern Pointer_Type Pointer_GetCurrentType();

extern void Pointer_SetCurrent_IfTimerFinished(Pointer_Type pointerType);

extern void Pointer_DrawPointer(U32 mouseX, U32 mouseY);
