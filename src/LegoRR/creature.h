#pragma once

#include "platform.h"
#include "lego_object.h"

typedef enum CreatureFlags
{
    CREATURE_FLAG_NONE   = 0,
    CREATURE_FLAG_SOURCE = 0x1,
} CreatureFlags;

typedef F32 (*GetWorldZCallback)(F32, F32, struct Map3D*);

typedef struct CreatureModel
{
    LegoObject_ID objID;
    lpContainer contAct; // (ACT, true)
    U32 reserved1[24];
    const char* cameraNullName; // (ae: CameraNullName)
    U32 cameraNullFrames; // (ae: CameraNullFrames)
    Bool3 cameraFlipDir; // (ae: CameraFlipDir)
    const char* drillNullName; // (ae: DrillNullName)
    const char* footStepNullName; // (ae: FootStepNullName)
    const char* carryNullName; // (ae: CarryNullName)
    const char* throwNullName; // (ae: ThrowNullName)
    const char* depositNullName; // (ae: DepositNullName)
    lpContainer drillNull;
    lpContainer footStepNull; // (unused)
    lpContainer carryNull;
    lpContainer throwNull;
    lpContainer depositNull;
    lpContainer cameraNulls[4];
    lpMeshLOD polyMedium; // (ae: MediumPoly)
    lpMeshLOD polyHigh; // (ae: HighPoly)
    lpMeshLOD polyFP; // (ae: FPPoly::Camera#)
    CreatureFlags flags; // (0x1: original that holds memory)

} CreatureModel, *lpCreatureModel;

extern B32 Creature_Load(lpCreatureModel creature, LegoObject_ID objID, lpContainer root, const char* filename, const char* gameName);

extern void Creature_Clone(lpCreatureModel srcCreature, lpCreatureModel destCreature);

extern void Creature_SetOrientation(lpCreatureModel creature, F32 xDir, F32 yDir);
extern void Creature_SetPosition(lpCreatureModel creature, F32 xPos, F32 yPos, GetWorldZCallback zCallback, struct Map3D* map);
