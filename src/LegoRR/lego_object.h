#pragma once

#include "platform.h"
#include "container.h"
#include "config.h"
#include "stats.h"

typedef enum LegoObject_Type
{
    LegoObject_None              = 0,
    LegoObject_Vehicle           = 1,
    LegoObject_MiniFigure        = 2,
    LegoObject_RockMonster       = 3,
    LegoObject_Building          = 4,
    LegoObject_Boulder           = 5,
    LegoObject_PowerCrystal      = 6,
    LegoObject_Ore               = 7,
    LegoObject_Dynamite          = 8,
    LegoObject_Barrier           = 9,
    LegoObject_UpgradePart       = 10,
    LegoObject_ElectricFence     = 11,
    LegoObject_SpiderWeb         = 12,
    LegoObject_OohScary          = 13,
    LegoObject_ElectricFenceStud = 14,
    LegoObject_Path              = 15,
    LegoObject_Pusher            = 16,
    LegoObject_Freezer           = 17,
    LegoObject_IceCube           = 18,
    LegoObject_LaserShot         = 19,
    LegoObject_Type_Count        = 20,
    LegoObject_TVCamera          = -1,
} LegoObject_Type;

typedef U32 LegoObject_ID;

typedef struct LegoObject
{
    // TODO: Implement LegoObject
    lpObjectStats stats;
    // TODO: Implement LegoObject
} LegoObject, *lpLegoObject;

typedef struct LegoObject_Globs
{
    lpLegoObject listSet[32];
    lpLegoObject freeList;

    // TODO: Implement LegoObject_Globs

    lpLegoObject minifigureObj_9cb8;

    // TODO: Implement LegoObject_Globs
} LegoObject_Globs;

extern LegoObject_Globs objectGlobs;

extern void LegoObject_Initialize();

extern void LegoObject_TriggerFrameCallback(lpContainer cont, void* data);

extern void Object_LoadToolNames(lpConfig config, const char* gameName);
extern void Object_LoadToolTipIcons(lpConfig config);

extern void LegoObject_LoadObjTtsSFX(lpConfig config, const char* gameName);

extern void LegoObject_SetLevelEnding(B32 ending);

extern void LegoObject_RegisterSlimySlugHole(Point2I* blockPos);
