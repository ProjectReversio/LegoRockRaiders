#include "lego_object.h"

#include <stdio.h>

#include "aitask.h"
#include "creature.h"
#include "dummy.h"
#include "game_control.h"
#include "lego.h"
#include "level.h"
#include "mem.h"
#include "routing.h"
#include "sfx.h"
#include "unknown.h"

LegoObject_Globs objectGlobs = { NULL };

void LegoObject_Initialize()
{
    memset(&objectGlobs, 0, sizeof(LegoObject_Globs));

    objectGlobs.activityName[Activity_Stand] = "Activity_Stand";
    objectGlobs.activityName[Activity_Route] = "Activity_Route";
    objectGlobs.activityName[Activity_RouteRubble] = "Activity_RouteRubble";
    objectGlobs.activityName[Activity_RunPanic] = "Activity_RunPanic";
    objectGlobs.activityName[Activity_Drill] = "Activity_Drill";
    objectGlobs.activityName[Activity_Teleport] = "Activity_Teleport";
    objectGlobs.activityName[Activity_Walk] = "Activity_Walk";
    objectGlobs.activityName[Activity_Reinforce] = "Activity_Reinforce";
    objectGlobs.activityName[Activity_Reverse] = "Activity_Reverse";
    objectGlobs.activityName[Activity_TurnLeft] = "Activity_TurnLeft";
    objectGlobs.activityName[Activity_TurnRight] = "Activity_TurnRight";
    objectGlobs.activityName[Activity_CantDo] = "Activity_CantDo";
    objectGlobs.activityName[Activity_Emerge] = "Activity_Emerge";
    objectGlobs.activityName[Activity_Enter] = "Activity_Enter";
    objectGlobs.activityName[Activity_EnterRein] = "Activity_EnterRein";
    objectGlobs.activityName[Activity_Collect] = "Activity_Collect";
    objectGlobs.activityName[Activity_Gather] = "Activity_Gather";
    objectGlobs.activityName[Activity_Carry] = "Activity_Carry";
    objectGlobs.activityName[Activity_CarryRubble] = "Activity_CarryRubble";
    objectGlobs.activityName[Activity_Throw] = "Activity_Throw";
    objectGlobs.activityName[Activity_CarryTurnLeft] = "Activity_CarryTurnLeft";
    objectGlobs.activityName[Activity_CarryTurnRight] = "Activity_CarryTurnRight";
    objectGlobs.activityName[Activity_CarryStand] = "Activity_CarryStand";
    objectGlobs.activityName[Activity_HitLeft] = "Activity_HitLeft";
    objectGlobs.activityName[Activity_HitRight] = "Activity_HitRight";
    objectGlobs.activityName[Activity_HitFront] = "Activity_HitFront";
    objectGlobs.activityName[Activity_HitBack] = "Activity_HitBack";
    objectGlobs.activityName[Activity_HitHard] = "Activity_HitHard";
    objectGlobs.activityName[Activity_Dynamite] = "Activity_Dynamite";
    objectGlobs.activityName[Activity_Deposit] = "Activity_Deposit";
    objectGlobs.activityName[Activity_Clear] = "Activity_Clear";
    objectGlobs.activityName[Activity_Place] = "Activity_Place";
    objectGlobs.activityName[Activity_Repair] = "Activity_Repair";
    objectGlobs.activityName[Activity_Slip] = "Activity_Slip";
    objectGlobs.activityName[Activity_Rest] = "Activity_Rest";
    objectGlobs.activityName[Activity_Eat] = "Activity_Eat";
    objectGlobs.activityName[Activity_Stamp] = "Activity_Stamp";
    objectGlobs.activityName[Activity_ThrowMan] = "Activity_ThrowMan";
    objectGlobs.activityName[Activity_ThrownByRockMonster] = "Activity_ThrownByRockMonster";
    objectGlobs.activityName[Activity_GetUp] = "Activity_GetUp";
    objectGlobs.activityName[Activity_BuildPath] = "Activity_BuildPath";
    objectGlobs.activityName[Activity_Upgrade] = "Activity_Upgrade";
    objectGlobs.activityName[Activity_Explode] = "Activity_Explode";
    objectGlobs.activityName[Activity_Unpowered] = "Activity_Unpowered";
    objectGlobs.activityName[Activity_FireLaser] = "Activity_FireLaser";
    objectGlobs.activityName[Activity_Freezed] = "Activity_Freezed";
    objectGlobs.activityName[Activity_FreezeStart] = "Activity_FreezeStart";
    objectGlobs.activityName[Activity_FreezeMelt] = "Activity_FreezeMelt";
    objectGlobs.activityName[Activity_Recharge] = "Activity_Recharge";
    objectGlobs.activityName[Activity_WakeUp] = "Activity_WakeUp";
    objectGlobs.activityName[Activity_Train] = "Activity_Train";
    objectGlobs.activityName[Activity_FloatOn] = "Activity_FloatOn";
    objectGlobs.activityName[Activity_FloatOff] = "Activity_FloatOff";
    objectGlobs.activityName[Activity_Opening] = "Activity_Opening";
    objectGlobs.activityName[Activity_Closing] = "Activity_Closing";
    objectGlobs.activityName[Activity_Open] = "Activity_Open";
    objectGlobs.activityName[Activity_Waiting1] = "Activity_Waiting1";
    objectGlobs.activityName[Activity_Waiting2] = "Activity_Waiting2";
    objectGlobs.activityName[Activity_Waiting3] = "Activity_Waiting3";
    objectGlobs.activityName[Activity_Waiting4] = "Activity_Waiting4";
    objectGlobs.activityName[Activity_Waiting5] = "Activity_Waiting5";
    objectGlobs.activityName[Activity_Waiting6] = "Activity_Waiting6";
    objectGlobs.activityName[Activity_Waiting7] = "Activity_Waiting7";
    objectGlobs.activityName[Activity_Waiting8] = "Activity_Waiting8";
    objectGlobs.activityName[Activity_Waiting9] = "Activity_Waiting9";
    objectGlobs.activityName[Activity_Waiting10] = "Activity_Waiting10";
    objectGlobs.activityName[Activity_Waiting11] = "Activity_Waiting11";
    objectGlobs.activityName[Activity_Waiting12] = "Activity_Waiting12";
    objectGlobs.activityName[Activity_Waiting13] = "Activity_Waiting13";
    objectGlobs.activityName[Activity_Waiting14] = "Activity_Waiting14";
    objectGlobs.activityName[Activity_Waiting15] = "Activity_Waiting15";
    objectGlobs.activityName[Activity_Waiting16] = "Activity_Waiting16";
    objectGlobs.activityName[Activity_Waiting17] = "Activity_Waiting17";
    objectGlobs.activityName[Activity_Waiting18] = "Activity_Waiting18";
    objectGlobs.activityName[Activity_Waiting19] = "Activity_Waiting19";
    objectGlobs.activityName[Activity_Waiting20] = "Activity_Waiting20";
    objectGlobs.activityName[Activity_Waiting21] = "Activity_Waiting21";
    objectGlobs.activityName[Activity_Crumble] = "Activity_Crumble";
    objectGlobs.activityName[Activity_TeleportIn] = "Activity_TeleportIn";

    objectGlobs.abilityName[0] = "LegoObject_AbilityType_Pilot";
    objectGlobs.abilityName[1] = "LegoObject_AbilityType_Sailor";
    objectGlobs.abilityName[2] = "LegoObject_AbilityType_Driver";
    objectGlobs.abilityName[3] = "LegoObject_AbilityType_Dynamite";
    objectGlobs.abilityName[4] = "LegoObject_AbilityType_Repair";
    objectGlobs.abilityName[5] = "LegoObject_AbilityType_Scanner";

    memset(objectGlobs.listSet, 0, sizeof(objectGlobs.listSet));

    legoGlobs.toolName[0] = "LegoObject_ToolType_Drill";
    legoGlobs.toolName[1] = "LegoObject_ToolType_Spade";
    legoGlobs.toolName[2] = "LegoObject_ToolType_Hammer";
    legoGlobs.toolName[3] = "LegoObject_ToolType_Spanner";
    legoGlobs.toolName[4] = "LegoObject_ToolType_Laser";
    legoGlobs.toolName[5] = "LegoObject_ToolType_PusherGun";
    legoGlobs.toolName[6] = "LegoObject_ToolType_BirdScarer";
    legoGlobs.toolName[7] = "LegoObject_ToolType_FreezerGun";

    objectGlobs.toolNullIndex[0] = 0;
    objectGlobs.toolNullIndex[1] = 0;
    objectGlobs.toolNullIndex[2] = 1;
    objectGlobs.toolNullIndex[3] = 1;
    objectGlobs.toolNullIndex[4] = 4;
    objectGlobs.toolNullIndex[5] = 4;
    objectGlobs.toolNullIndex[6] = 4;
    objectGlobs.toolNullIndex[7] = 4;
    objectGlobs.toolNullIndex[8] = 3;
    objectGlobs.toolNullIndex[9] = 2;
    objectGlobs.toolNullIndex[10] = 4;

    objectGlobs.freeList = NULL;
    objectGlobs.listCount = 0;
    objectGlobs.flags = OBJECT_GLOB_FLAG_INITIALISED;
}

void LegoObject_AddList()
{
    // TODO: Clean up this code

    U32 size = 1 << ((U8)objectGlobs.listCount & 0x1f);
    objectGlobs.listSet[objectGlobs.listCount] = Mem_Alloc(size * sizeof(LegoObject));
    LegoObject* list = objectGlobs.listSet[objectGlobs.listCount];
    if (list != NULL)
    {
        objectGlobs.listCount++;
        if (size > 1)
        {
            S32 i = size - 1;
            LegoObject* obj = list;
            do
            {
                obj->nextFree = obj + 1;
                i--;
                obj++;
            } while (i != 0);
        }

        list[size - 1].nextFree = objectGlobs.freeList;
        objectGlobs.freeList = list;
    }
}

void LegoObject_TriggerFrameCallback(lpContainer cont, void* data)
{
    // TODO: Implement LegoObject_TriggerFrameCallback
}

void Object_LoadToolNames(lpConfig config, const char* gameName)
{
    // TODO: Implement Object_LoadToolNames
}

void Object_LoadToolTipIcons(lpConfig config)
{
    // TODO: Implement Object_LoadToolTipIcons
}

void LegoObject_LoadObjTtsSFX(lpConfig config, const char* gameName)
{
    // TODO: Implement LegoObject_LoadObjTtsSFX
}

void LegoObject_SetLevelEnding(B32 ending)
{
    // TODO: Implement LegoObject_SetLevelEnding
}

void LegoObject_RegisterSlimySlugHole(Point2I* blockPos)
{
    // TODO: Implement LegoObject_RegisterSlimySlugHole
}

void LegoObject_RegisterVehicle__callsForWater(lpLegoObject liveObj)
{
    LegoObject_WaterVehicle_Register(liveObj);
}

void LegoObject_WaterVehicle_Register(lpLegoObject liveObj)
{
    if (liveObj->type == LegoObject_Vehicle)
    {
        // TODO: Implement LegoObject_WaterVehicle_Register
    }
}

void LegoObject_IncLevelPathsBuilt(B32 incCurrent)
{
    // TODO: Implement LegoObject_IncLevelPathsBuilt
}

void LegoObject_SetPositionAndHeading(lpLegoObject liveObj, F32 xPos, F32 yPos, F32 theta, B32 assignHeading)
{
    Point3F orientation;
    Point3F axis;
    if (assignHeading)
    {
        orientation.x = 0.0f;
        orientation.y = 1.0f;
        orientation.z = 0.0f;
        axis.x = 0.0f;
        axis.y = 0.0f;
        axis.z = -1.0f;
        Maths_Vector3DRotate(&orientation, &orientation, &axis, theta);
    }

    if (liveObj->type == LegoObject_Vehicle)
    {
        // TODO: Implement LegoObject_SetPositionAndHeading
    }
    else if (liveObj->type == LegoObject_MiniFigure)
    {
        if (assignHeading)
            Creature_SetOrientation(liveObj->miniFigure, orientation.x, orientation.y);

        Creature_SetPosition(liveObj->miniFigure, xPos, yPos, LegoObject_GetWorldZCallback_Lake, Lego_GetMap());
    }
    else if (liveObj->type == LegoObject_RockMonster)
    {
        // TODO: Implement LegoObject_SetPositionAndHeading
    }
    else if (liveObj->type == LegoObject_Building)
    {
        // TODO: Implement LegoObject_SetPositionAndHeading
    }
    else if ((liveObj->flags3 & LIVEOBJ3_SIMPLEOBJECT) != LIVEOBJ3_NONE)
    {
        // TODO: Implement LegoObject_SetPositionAndHeading
    }

    if (assignHeading)
    {
        liveObj->faceDirection.x = orientation.x;
        liveObj->faceDirection.y = orientation.y;
        liveObj->faceDirection.z = orientation.z;
    }

    liveObj->faceDirectionLength = 0.01f;

    liveObj->tempPosition.x = liveObj->faceDirection.x * -0.01f;
    liveObj->tempPosition.y = liveObj->faceDirection.y * -0.01f;
    liveObj->tempPosition.z = liveObj->faceDirection.z * -0.01f;

    liveObj->tempPosition.x += xPos;
    liveObj->tempPosition.y += yPos;
    liveObj->tempPosition.z += 0.0f;
}

void LegoObject_FUN_00438720(lpLegoObject liveObj)
{
    // TODO: Implement LegoObject_FUN_00438720
}

B32 LegoObject_RunThroughListsSkipUpgradeParts(LegoObject_RunThroughListsCallback callback, void* search)
{
    return LegoObject_RunThroughLists(callback, search, TRUE);
}

B32 LegoObject_DoPickSphereSelection(U32 mouseX, U32 mouseY, lpLegoObject* selectedObj)
{
    // TODO: Implement LegoObject_DoPickSphereSelection
    return FALSE;
}

lpLegoObject LegoObject_Create_internal()
{
    if (objectGlobs.freeList == NULL)
        LegoObject_AddList();

    lpLegoObject liveObj = objectGlobs.freeList;
    objectGlobs.freeList = objectGlobs.freeList->nextFree;

    memset(liveObj, 0, sizeof(LegoObject));
    liveObj->nextFree = liveObj;
    liveObj->targetBlockPos.x = -1.0f;
    liveObj->targetBlockPos.y = -1.0f;

#ifdef LEGORR_DEBUG_SHOW_INFO
    static U32 count = 0;
    liveObj->index = count;
    count++;
#endif

    return liveObj;
}

lpLegoObject LegoObject_Create(void** objModel, LegoObject_Type objType, LegoObject_ID objID)
{
    lpLegoObject liveObj = LegoObject_Create_internal();
    if (!liveObj)
        return NULL;

    liveObj->type = objType;
    liveObj->id = objID;
    liveObj->flags3 = LIVEOBJ3_NONE;
    liveObj->health = 100.0f;
    liveObj->energy = 100.0f;

    Container_EnableSoundTriggers(FALSE);

    B32 soundOn = SFX_IsSoundOn();

    SFX_SetSoundOn(FALSE, FALSE);

    if (liveObj->type == LegoObject_Vehicle)
    {
        // TODO: Implement LegoObject_Create
    }
    else if (liveObj->type == LegoObject_MiniFigure)
    {
        liveObj->miniFigure = Mem_Alloc(sizeof(CreatureModel));
        Creature_Clone(objModel, liveObj->miniFigure);
        Object_SetOwnerObject(liveObj->miniFigure, liveObj);
        liveObj->carryNullFrames = NERPFunc__True(liveObj->miniFigure);
        //LiveFlags3 ogFlags = liveObj->flags3;
        liveObj->flags3 |= (LIVEOBJ3_UNK_1 | LIVEOBJ3_CANREINFORCE | LIVEOBJ3_CANFIRSTPERSON | LIVEOBJ3_CANCARRY | LIVEOBJ3_CANPICKUP | LIVEOBJ3_CANYESSIR | LIVEOBJ3_CANSELECT | LIVEOBJ3_CANDAMAGE | LIVEOBJ3_AITASK_UNK_400000);

        if (((CreatureModel*)objModel)->drillNullName != NULL)
        {
            //liveObj->flags3 = ogFlags | (LIVEOBJ3_UNK_1 | LIVEOBJ3_CANDIG | LIVEOBJ3_CANREINFORCE | LIVEOBJ3_CANFIRSTPERSON | LIVEOBJ3_CANCARRY | LIVEOBJ3_CANPICKUP | LIVEOBJ3_CANYESSIR | LIVEOBJ3_CANSELECT | LIVEOBJ3_CANDAMAGE | LIVEOBJ3_AITASK_UNK_400000);
            liveObj->flags3 |= LIVEOBJ3_CANDIG;
        }

        StatsObject_SetObjectLevel(liveObj, 0);
        liveObj->flags3 |= LIVEOBJ3_CANDYNAMITE;
        LegoObject_MiniFigure_EquipTool(liveObj, LegoObject_ToolType_Drill);
    }
    else if (liveObj->type == LegoObject_RockMonster)
    {
        // TODO: Implement LegoObject_Create
    }
    else if (liveObj->type == LegoObject_Building)
    {
        // TODO: Implement LegoObject_Create
    }
    else if (liveObj->type == LegoObject_UpgradePart)
    {
        // TODO: Implement LegoObject_Create
    }
    else
    {
        // TODO: Implement LegoObject_Create
    }

    liveObj->activityName2 = objectGlobs.activityName[Activity_Stand];
    liveObj->activityName1 = objectGlobs.activityName[Activity_TurnLeft];

    if (LegoObject_UpdateActivityChange(liveObj))
        liveObj->flags3 |= LIVEOBJ3_CANTURN;

    liveObj->activityName1 = objectGlobs.activityName[Activity_Gather];
    if (LegoObject_UpdateActivityChange(liveObj))
        liveObj->flags3 |= LIVEOBJ3_CANGATHER;

    liveObj->activityName1 = objectGlobs.activityName[Activity_RouteRubble];
    if (LegoObject_UpdateActivityChange(liveObj))
        liveObj->flags3 |= LIVEOBJ3_CANROUTERUBBLE;

    liveObj->activityName1 = objectGlobs.activityName[Activity_Stand];
    LegoObject_UpdateActivityChange(liveObj);

    AITask_LiveObject_Unk_UpdateAITask_AnimationWait(liveObj);

    Container_EnableSoundTriggers(TRUE);
    if (soundOn)
        SFX_SetSoundOn(TRUE, FALSE);

    LegoObject_UpdatePowerConsumption(liveObj);

    objectGlobs.objectTotalLevels[liveObj->type][liveObj->id][0]++;

    if ((StatsObject_GetStatsFlags2(liveObj) & STATS2_UNSELECTABLE) != STATS2_NONE)
    {
        liveObj->flags3 &= (LIVEOBJ3_UNK_1|LIVEOBJ3_CANDIG|LIVEOBJ3_CANREINFORCE|LIVEOBJ3_CANTURN|LIVEOBJ3_CANFIRSTPERSON|LIVEOBJ3_CANCARRY|LIVEOBJ3_CANPICKUP|LIVEOBJ3_CANYESSIR|LIVEOBJ3_UNK_200|LIVEOBJ3_UNK_400|
          LIVEOBJ3_UNUSED_800|LIVEOBJ3_CENTERBLOCKIDLE|LIVEOBJ3_UNK_2000|LIVEOBJ3_UNK_4000|LIVEOBJ3_CANDYNAMITE|LIVEOBJ3_UNK_10000|LIVEOBJ3_SIMPLEOBJECT|LIVEOBJ3_CANDAMAGE|LIVEOBJ3_UPGRADEPART|
          LIVEOBJ3_ALLOWCULLING_UNK|LIVEOBJ3_SELECTED|LIVEOBJ3_AITASK_UNK_400000|LIVEOBJ3_REMOVING|LIVEOBJ3_UNK_1000000|LIVEOBJ3_UNK_2000000|LIVEOBJ3_CANGATHER|LIVEOBJ3_MONSTER_UNK_8000000|
          LIVEOBJ3_CANROUTERUBBLE|LIVEOBJ3_HASPOWER|LIVEOBJ3_UNK_40000000|LIVEOBJ3_POWEROFF);
    }

    return liveObj;
}

B32 LegoObject_Remove(lpLegoObject liveObj)
{
    // TODO: Implement LegoObject_Remove
    return FALSE;
}

void LegoObject_MiniFigure_EquipTool(lpLegoObject liveObj, LegoObject_ToolType toolType)
{
    // TODO: Implement LegoObject_MiniFigure_EquipTool
}

void LegoObject_UpdatePowerConsumption(lpLegoObject liveObj)
{
    // TODO: Implement LegoObject_UpdatePowerConsumption
}

B32 LegoObject_UpdateActivityChange(lpLegoObject liveObj)
{
    const char* actName = liveObj->activityName1;
    if (actName == liveObj->activityName2)
        return FALSE;

    if (actName != objectGlobs.activityName[Activity_Stand] &&
        actName != objectGlobs.activityName[Activity_Route] &&
        actName != objectGlobs.activityName[Activity_Drill] &&
        actName != objectGlobs.activityName[Activity_Walk] &&
        actName != objectGlobs.activityName[Activity_Reverse] &&
        actName != objectGlobs.activityName[Activity_Carry] &&
        actName != objectGlobs.activityName[Activity_CarryStand] &&
        actName != objectGlobs.activityName[Activity_RouteRubble])
    {
        AITask_DoAnimationWait(liveObj);
    }

    B32 result = FALSE;

    switch (liveObj->type)
    {
        case LegoObject_Vehicle:
        {
            // TODO: Implement LegoObject_UpdateActivityChange
            break;
        }
        case LegoObject_MiniFigure:
        {
            result = Creature_SetActivity(liveObj->miniFigure, liveObj->activityName1, liveObj->animTime);
            liveObj->cameraNull = Creature_GetCameraNull(liveObj->miniFigure, liveObj->cameraFrame);
            break;
        }
        case LegoObject_RockMonster:
        {
            // TODO: Implement LegoObject_UpdateActivityChange
            break;
        }
        case LegoObject_Building:
        {
            // TODO: Implement LegoObject_UpdateActivityChange
            break;
        }
    }

    if ((StatsObject_GetStatsFlags1(liveObj) & STATS1_CANBEDRIVEN) != STATS1_NONE && liveObj->driveObject != NULL &&
        liveObj->driveObject->type == LegoObject_MiniFigure)
    {
        // TODO: Implement LegoObject_UpdateActivityChange
    }

    liveObj->animTime = 0.0f;
    liveObj->activityName2 = liveObj->activityName1;

    liveObj->flags2 &= ~LIVEOBJ2_UNK_20000;

    return result;
}

void LegoObject_SetActivity(lpLegoObject liveObj, Activity_Type activityType, U32 repeatCount)
{
    liveObj->activityName1 = objectGlobs.activityName[activityType];
    liveObj->animRepeat = repeatCount;
    liveObj->animTime = 0.0f;
}

lpContainer LegoObject_GetActivityContainer(lpLegoObject liveObj)
{
    switch (liveObj->type)
    {
        case LegoObject_Vehicle:
            return Vehicle_GetActivityContainer(liveObj->vehicle);
        case LegoObject_MiniFigure:
            return Object_GetActivityContainer((BasicObjectModel*) liveObj->miniFigure);
        case LegoObject_RockMonster:
            return Object_GetActivityContainer((BasicObjectModel*) liveObj->rockMonster);
        case LegoObject_Building:
            return Object_GetActivityContainer((BasicObjectModel*) liveObj->building);
        case LegoObject_UpgradePart:
            return Object_GetActivityContainer((BasicObjectModel*) liveObj->upgradePart);
    }

    if ((liveObj->flags3 & LIVEOBJ3_SIMPLEOBJECT) != LIVEOBJ3_NONE)
        return liveObj->other;

    // TODO: This seems wrong?
    return (lpContainer) liveObj;
}

void LegoObject_HideAllCertainObjects()
{
    // TODO: Implement LegoObject_HideAllCertainObjects
}

void LegoObject_UpdateAll(F32 elapsedGame)
{
    // Were we updating our power grid after the update loop, last tick?
    if ((objectGlobs.flags & OBJECT_GLOB_FLAG_POWERUPDATING) != OBJECT_GLOB_FLAG_NONE)
    {
        Level_PowerGrid_ClearBlockPowered_100_Points28C();
    }

    objectGlobs.flags |= OBJECT_GLOB_FLAG_UPDATING;

    LegoObject_RunThroughLists(LegoObject_Callback_Update, &elapsedGame, FALSE);

    LegoObject_GlobFlags flagUnsetUpdating = objectGlobs.flags & ~OBJECT_GLOB_FLAG_UPDATING;
    LegoObject_GlobFlags flagPowerRecalc = objectGlobs.flags & OBJECT_GLOB_FLAG_POWERUPDATING;
    objectGlobs.flags = flagUnsetUpdating;

    // Were we updating our power grid after the update loop, last tick?
    if (flagPowerRecalc != OBJECT_GLOB_FLAG_NONE)
    {
        Level_PowerGrid_UpdateLevelBlocks_PointsAAC();
    }

    // Turn off power updating if an update is no longer needed.
    if ((objectGlobs.flags & OBJECT_GLOB_FLAG_POWERNEEDSUPDATE) == OBJECT_GLOB_FLAG_NONE)
    {
        objectGlobs.flags &= ~OBJECT_GLOB_FLAG_POWERUPDATING;
    }
    else
    {
        // Otherwise, restart the request for a power grid update (turns on POWERUPDATING flag).
        LegoObject_RequestPowerGridUpdate();
        objectGlobs.flags &= ~OBJECT_GLOB_FLAG_POWERNEEDSUPDATE;
    }

    LegoObject_Flocks_Update_FUN_0044c1c0(&elapsedGame);

    objectGlobs.s_sound3DUpdateTimer -= elapsedGame;
    if (objectGlobs.s_sound3DUpdateTimer <= 0.0f)
    {
        objectGlobs.s_sound3DUpdateTimer = 125.0f;
        Lego_UpdateAll3DSounds(TRUE);
    }
}

B32 LegoObject_Callback_Update(lpLegoObject liveObj, void* search)
{
    F32* pElapsed = (F32*)search;
    F32 elapsed = *pElapsed;

    // TODO: Implement LegoObject_Callback_Update

    lpLegoObject driveObj;
    if (liveObj->type == LegoObject_MiniFigure && (liveObj->flags2 & LIVEOBJ2_DRIVING) != LIVEOBJ2_NONE &&
        (driveObj = liveObj->driveObject, driveObj != NULL && driveObj->type == LegoObject_Vehicle))
    {
        // TODO: Implement LegoObject_Callback_Update
    }
    else
    {
        LegoObject_MoveAnimation(liveObj, elapsed);
    }

    // TODO: Implement LegoObject_Callback_Update

    if ((liveObj->flags1 & LIVEOBJ1_DRILLING) == LIVEOBJ1_NONE) goto notDrilling;
    if ((liveObj->flags1 & LIVEOBJ1_DRILLINGSTART) != LIVEOBJ1_NONE)
    {
        lpContainer actCont = LegoObject_GetActivityContainer(liveObj);
        SFX_ID sfxId = StatsObject_GetDrillSoundType(liveObj, FALSE);
        liveObj->drillSoundHandle = SFX_Random_PlaySound3DOnContainer(actCont, sfxId, TRUE, TRUE, NULL);
        liveObj->flags4 |= LIVEOBJ4_DRILLSOUNDPLAYING;
        liveObj->flags1 &= ~LIVEOBJ1_DRILLINGSTART;
    }

    if (liveObj->routeBlocks == NULL)
    {
notRouting:
        liveObj->flags1 &= ~LIVEOBJ1_DRILLING;
        liveObj->flags3 &= (LIVEOBJ3_POWEROFF|LIVEOBJ3_UNK_40000000|LIVEOBJ3_HASPOWER|LIVEOBJ3_CANROUTERUBBLE|LIVEOBJ3_MONSTER_UNK_8000000|LIVEOBJ3_CANGATHER|LIVEOBJ3_UNK_1000000|LIVEOBJ3_REMOVING|
          LIVEOBJ3_AITASK_UNK_400000|LIVEOBJ3_SELECTED|LIVEOBJ3_ALLOWCULLING_UNK|LIVEOBJ3_UPGRADEPART|LIVEOBJ3_CANDAMAGE|LIVEOBJ3_SIMPLEOBJECT|LIVEOBJ3_UNK_10000|LIVEOBJ3_CANDYNAMITE|LIVEOBJ3_UNK_4000
          |LIVEOBJ3_UNK_2000|LIVEOBJ3_CENTERBLOCKIDLE|LIVEOBJ3_UNUSED_800|LIVEOBJ3_UNK_400|LIVEOBJ3_UNK_200|LIVEOBJ3_CANSELECT|LIVEOBJ3_CANYESSIR|LIVEOBJ3_CANPICKUP|LIVEOBJ3_CANCARRY|
          LIVEOBJ3_CANFIRSTPERSON|LIVEOBJ3_CANTURN|LIVEOBJ3_CANREINFORCE|LIVEOBJ3_CANDIG|LIVEOBJ3_UNK_1);

        lpContainer actCont = LegoObject_GetActivityContainer(liveObj);
        SFX_Sound3D_StopSound(liveObj->drillSoundHandle);
        liveObj->flags4 &= ~LIVEOBJ4_DRILLSOUNDPLAYING;
        SFX_ID sfxId = StatsObject_GetDrillSoundType(liveObj, TRUE);
        SFX_Random_PlaySound3DOnContainer(actCont, sfxId, FALSE, TRUE, NULL);

        Unknown_Empty_FUN_00410195_LegoStripped(NULL, NULL, 0);

        liveObj->flags3 &= (LIVEOBJ3_POWEROFF|LIVEOBJ3_UNK_40000000|LIVEOBJ3_HASPOWER|LIVEOBJ3_CANROUTERUBBLE|LIVEOBJ3_MONSTER_UNK_8000000|LIVEOBJ3_CANGATHER|LIVEOBJ3_UNK_2000000|LIVEOBJ3_UNK_1000000|
               LIVEOBJ3_REMOVING|LIVEOBJ3_AITASK_UNK_400000|LIVEOBJ3_SELECTED|LIVEOBJ3_ALLOWCULLING_UNK|LIVEOBJ3_UPGRADEPART|LIVEOBJ3_CANDAMAGE|LIVEOBJ3_SIMPLEOBJECT|LIVEOBJ3_UNK_10000|
               LIVEOBJ3_CANDYNAMITE|LIVEOBJ3_UNK_2000|LIVEOBJ3_CENTERBLOCKIDLE|LIVEOBJ3_UNUSED_800|LIVEOBJ3_UNK_400|LIVEOBJ3_UNK_200|LIVEOBJ3_CANSELECT|LIVEOBJ3_CANYESSIR|LIVEOBJ3_CANPICKUP|
               LIVEOBJ3_CANCARRY|LIVEOBJ3_CANFIRSTPERSON|LIVEOBJ3_CANTURN|LIVEOBJ3_CANREINFORCE|LIVEOBJ3_CANDIG|LIVEOBJ3_UNK_1);
    }
    else
    {
        S32 bpX = liveObj->routeBlocks[liveObj->routeBlocksCurrent].blockPos.x;
        S32 bpY = liveObj->routeBlocks[liveObj->routeBlocksCurrent].blockPos.y;

        if (!LiveObject_BlockCheck_FUN_004326a0(liveObj, liveObj->targetBlockPos.x, liveObj->targetBlockPos.y, liveObj->flags3 & LIVEOBJ3_UNK_2000000, TRUE))
            goto notRouting;

        Point2F drillNullPos;
        if (!LegoObject_GetDrillNullPosition(liveObj, &drillNullPos.x, &drillNullPos.y))
            goto notDrilling;

        // Since Unknown_Empty_FUN_00410195_LegoStripped is empty, this doesn't do anything.
        // But keeping it here for accuracy.
        if (liveObj->type == LegoObject_MiniFigure)
        {
            lpContainer camCont = Lego_GetCurrentCamera_Container();
            lpContainer drillNull = Creature_GetDrillNull(liveObj->miniFigure);
            Unknown_Empty_FUN_00410195_LegoStripped(drillNull, camCont, 0);
        }

        Lego_SurfaceType surfaceType = Lego_GetBlockTerrain(liveObj->targetBlockPos.x, liveObj->targetBlockPos.y);
        F32 drillSpeed = StatsObject_GetDrillTimeType(liveObj, surfaceType);
        if ((StatsObject_GetStatsFlags1(liveObj) & STATS1_SINGLEWIDTHDIG) != STATS1_NONE && (liveObj->flags3 & LIVEOBJ3_UNK_2000000) != LIVEOBJ3_NONE)
            drillSpeed *= 0.5f;

        if (!Level_Block_Damage(liveObj->targetBlockPos.x, liveObj->targetBlockPos.y, drillSpeed, elapsed))
        {
            Point2I blPos;
            B32 blockPosNoZ = Lego_WorldToBlockPos_NoZ(drillNullPos.x, drillNullPos.y, &blPos.x, &blPos.y);
            if (!blockPosNoZ || blPos.x != bpX || blPos.y != bpY)
                goto notDrilling;

            liveObj->flags3 |= LIVEOBJ3_UNK_4000;
            goto notDrilling;
        }

        if (LiveObject_BlockCheck_FUN_004326a0(liveObj, liveObj->targetBlockPos.x, liveObj->targetBlockPos.y, liveObj->flags3 & LIVEOBJ3_UNK_2000000, TRUE))
        {
            Point2I targetBlock;
            targetBlock.x = (S32)liveObj->targetBlockPos.x;
            targetBlock.y = (S32)liveObj->targetBlockPos.y;

            B32 destroyedWall;
            if ((liveObj->flags3 & LIVEOBJ3_UNK_2000000) == LIVEOBJ3_NONE)
            {
                destroyedWall = Level_DestroyWall(Lego_GetLevel(), bpX, bpY, FALSE);
            }
            else
            {
                destroyedWall = Level_DestroyWallConnection(Lego_GetLevel(), targetBlock.x, targetBlock.y);
            }

            if (!destroyedWall)
                goto notDrilling;

            AITask_LiveObject_SetAITaskUnk(liveObj, AITask_Type_Dig, NULL, TRUE);
            liveObj->flags1 &= ~LIVEOBJ1_DRILLING;

            Level_Block_SetBusy(&targetBlock, FALSE);

            if (liveObj->routeBlocks[liveObj->routeBlocksTotal - 1].actionByte == ROUTE_ACTION_UNK_1)
                liveObj->routeBlocks[liveObj->routeBlocksTotal - 1].actionByte = ROUTE_ACTION_NONE;

            lpContainer actCont = LegoObject_GetActivityContainer(liveObj);
            SFX_Sound3D_StopSound(liveObj->drillSoundHandle);
            liveObj->flags4 &= ~LIVEOBJ4_DRILLSOUNDPLAYING;
            SFX_ID sfxId = StatsObject_GetDrillSoundType(liveObj, TRUE);
            SFX_Random_PlaySound3DOnContainer(actCont, sfxId, FALSE, TRUE, NULL);

            Unknown_Empty_FUN_00410195_LegoStripped(NULL, NULL, 0);

            liveObj->flags3 &= (LIVEOBJ3_POWEROFF|LIVEOBJ3_UNK_40000000|LIVEOBJ3_HASPOWER|LIVEOBJ3_CANROUTERUBBLE|LIVEOBJ3_MONSTER_UNK_8000000|LIVEOBJ3_CANGATHER|LIVEOBJ3_UNK_2000000|LIVEOBJ3_UNK_1000000|
                   LIVEOBJ3_REMOVING|LIVEOBJ3_AITASK_UNK_400000|LIVEOBJ3_SELECTED|LIVEOBJ3_ALLOWCULLING_UNK|LIVEOBJ3_UPGRADEPART|LIVEOBJ3_CANDAMAGE|LIVEOBJ3_SIMPLEOBJECT|LIVEOBJ3_UNK_10000|
                   LIVEOBJ3_CANDYNAMITE|LIVEOBJ3_UNK_2000|LIVEOBJ3_CENTERBLOCKIDLE|LIVEOBJ3_UNUSED_800|LIVEOBJ3_UNK_400|LIVEOBJ3_UNK_200|LIVEOBJ3_CANSELECT|LIVEOBJ3_CANYESSIR|LIVEOBJ3_CANPICKUP|
                   LIVEOBJ3_CANCARRY|LIVEOBJ3_CANFIRSTPERSON|LIVEOBJ3_CANTURN|LIVEOBJ3_CANREINFORCE|LIVEOBJ3_CANDIG|LIVEOBJ3_UNK_1);
        }
    }

notDrilling:
    if ((liveObj->flags2 & LIVEOBJ2_PUSHED) != LIVEOBJ2_NONE)
        goto objectupdate_end;

    if ((liveObj->flags1 & LIVEOBJ1_TURNING) != LIVEOBJ1_NONE)
    {
        if ((liveObj->flags1 & LIVEOBJ1_TURNRIGHT) != LIVEOBJ1_NONE)
        {
            if (liveObj->animTime > 0.0f)
            {
                lpContainer container = LegoObject_GetActivityContainer(liveObj);
                Container_AddRotation(container, Container_Combine_Before, 0.0f, 1.0f, 0.0f, 1.5707964f);
                Container_SetAnimationTime(container, 0.0f);
                liveObj->flags1 &= ~LIVEOBJ1_TURNRIGHT;
            }
            goto objectupdate_end;
        }

        if (liveObj->animTime <= 0.0f)
            goto objectupdate_end;

        F32 turnAmount;
        lpContainer container = LegoObject_GetActivityContainer(liveObj);
        if (liveObj->activityName1 == objectGlobs.activityName[Activity_TurnLeft] ||
            liveObj->activityName1 == objectGlobs.activityName[Activity_CarryTurnLeft])
        {
            turnAmount = -1.5707964f;
        }
        else
        {
            turnAmount = 1.5707964f;
        }

        Container_AddRotation(container, Container_Combine_Before, 0.0f, 1.0f, 0.0f, turnAmount);

        liveObj->flags1 &= ~LIVEOBJ1_TURNING;
        liveObj->flags1 |= LIVEOBJ1_MOVING;

        if (liveObj->routeBlocksTotal != 0)
        {
            if ((liveObj->flags1 & LIVEOBJ1_RUNNINGAWAY) != LIVEOBJ1_NONE &&
                liveObj->type == LegoObject_MiniFigure)
            {
                LegoObject_SetActivity(liveObj, Activity_RunPanic, 0);
                goto objectupdate_end;
            }

xlabel:
            LegoObject_SetActivity(liveObj, Activity_Route, 0);
            goto objectupdate_end;
        }

nextLabel:
        LegoObject_SetActivity(liveObj, Activity_Stand, 0);
        goto objectupdate_end;
    }

    if ((liveObj->flags2 & LIVEOBJ2_UNK_20) != LIVEOBJ2_NONE)
    {
        LegoObject_TryDepart_FUN_004499c0(liveObj);
        goto objectupdate_end;
    }

    if ((liveObj->flags1 & LIVEOBJ1_CANTDO) != LIVEOBJ1_NONE)
    {
thelbx:
        if (liveObj->animTime > 0.0f)
        {
            liveObj->flags1 &= ~LIVEOBJ1_CANTDO;
        }

        goto objectupdate_end;
    }

    if ((liveObj->flags1 & LIVEOBJ1_UNUSED_10000000) != LIVEOBJ1_NONE)
    {
        if (liveObj->animTime > 0.0f)
        {
            lpContainer container = LegoObject_GetActivityContainer(liveObj);

            liveObj->flags1 &= ~LIVEOBJ1_UNUSED_10000000;
            Container_SetOrientation(container, NULL, liveObj->dirVector_2c8.x, liveObj->dirVector_2c8.y, 0.0f, 0.0f, 0.0f, -1.0f);
            LegoObject_SetActivity(liveObj, Activity_Stand, 0);
        }
        goto objectupdate_end;
    }

    if ((liveObj->flags2 & LIVEOBJ2_FIRINGLASER) != LIVEOBJ2_NONE)
    {
        // TODO: Implement LegoObject_Callback_Update
    }

    if ((liveObj->flags4 & LIVEOBJ4_UNK_8000) != LIVEOBJ4_NONE)
    {
        if (liveObj->animTime <= 0.0f)
            goto objectupdate_end;

        liveObj->flags4 &= ~LIVEOBJ4_UNK_8000;

        if (liveObj->routeToObject != NULL)
        {
            lpContainer depositNull = LegoObject_GetDepositNull(liveObj->routeToObject);
            if (depositNull != NULL)
            {
                Point3F pos;
                Point3F dir;
                Container_GetPosition(depositNull, NULL, &pos);
                Container_GetOrientation(depositNull, NULL, &dir, NULL);

                F32 zPos = Map3D_GetWorldZ(Lego_GetMap(), pos.x, pos.y);

                lpContainer container = LegoObject_GetActivityContainer(liveObj);
                Container_SetPosition(container, NULL, pos.x, pos.y, zPos);

                lpContainer container2 = LegoObject_GetActivityContainer(liveObj);
                Container_SetOrientation(container2, NULL, dir.x, dir.y, dir.z, 0.0f, 0.0f, -1.0f);
            }

            LegoObject_DoOpeningClosing(liveObj->routeToObject, FALSE);
            LegoObject_SetActivity(liveObj, Activity_Stand, 0);
            goto objectupdate_end;
        }
        goto nextLabel;
    }

    if ((liveObj->flags2 & LIVEOBJ2_UNK_80000000) != LIVEOBJ2_NONE)
    {
        if (liveObj->routeToObject == NULL)
        {
            liveObj->flags2 &= (LIVEOBJ2_DAMAGESHAKING|LIVEOBJ2_UNK_20000000|LIVEOBJ2_RECHARGING|
                      LIVEOBJ2_FROZEN|LIVEOBJ2_UNK_4000000|LIVEOBJ2_ACTIVEELECTRICFENCE|
                      LIVEOBJ2_FIRINGFREEZER|LIVEOBJ2_FIRINGPUSHER|LIVEOBJ2_FIRINGLASER|
                      LIVEOBJ2_UNK_200000|LIVEOBJ2_UNK_100000|LIVEOBJ2_UNK_80000|LIVEOBJ2_UNK_40000|
                      LIVEOBJ2_UNK_20000|LIVEOBJ2_TRIGGERFRAMECALLBACK|LIVEOBJ2_UPGRADING|
                      LIVEOBJ2_PUSHED|LIVEOBJ2_SHOWDAMAGENUMBERS|LIVEOBJ2_DAMAGE_UNK_1000|LIVEOBJ2_UNK_800|
                      LIVEOBJ2_TRAINING|LIVEOBJ2_BUILDPATH|LIVEOBJ2_UNK_100|LIVEOBJ2_UNK_80|
                      LIVEOBJ2_UNK_40|LIVEOBJ2_UNK_20|LIVEOBJ2_UNK_10|LIVEOBJ2_DRIVING|
                      LIVEOBJ2_UNK_4|LIVEOBJ2_THROWN|LIVEOBJ2_THROWING);
        }
        else
        {
            lpContainer container = LegoObject_GetActivityContainer(liveObj);
            lpContainer routeContainer = LegoObject_GetActivityContainer(liveObj->routeToObject);
            Point3F pos;
            Point3F dir;
            Point3F up;
            Container_GetPosition(routeContainer, NULL, &pos);
            Container_GetOrientation(routeContainer, NULL, &dir, &up);
            Container_SetPosition(container, NULL, pos.x, pos.y, pos.z);
            Container_SetOrientation(container, NULL, dir.x, dir.y, dir.z, up.x, up.y, up.z);

            if (liveObj->animTime > 0.0f)
            {
                liveObj->routeToObject->carriedObjects[liveObj->routeToObject->numCarriedObjects] = liveObj;
                liveObj->routeToObject->carriedObjects[liveObj->routeToObject->numCarriedObjects]->carryingThisObject = liveObj->routeToObject;

                // TODO: What the heck is this? This looks very wrong.
                liveObj->routeToObject->carriedObjects[liveObj->routeToObject->numCarriedObjects]->carriedObjects[6] = liveObj->routeToObject->numCarriedObjects;

                liveObj->routeToObject->flags1 |= LIVEOBJ1_CARRYING;

                LegoObject_DoOpeningClosing(liveObj->routeToObject, FALSE);

                liveObj->routeToObject->numCarriedObjects++;

                liveObj->flags2 &= (LIVEOBJ2_DAMAGESHAKING|LIVEOBJ2_UNK_20000000|LIVEOBJ2_RECHARGING|LIVEOBJ2_FROZEN|
                                    LIVEOBJ2_UNK_4000000|LIVEOBJ2_ACTIVEELECTRICFENCE|LIVEOBJ2_FIRINGFREEZER|
                                    LIVEOBJ2_FIRINGPUSHER|LIVEOBJ2_FIRINGLASER|LIVEOBJ2_UNK_200000|LIVEOBJ2_UNK_100000|
                                    LIVEOBJ2_UNK_80000|LIVEOBJ2_UNK_40000|LIVEOBJ2_UNK_20000|LIVEOBJ2_TRIGGERFRAMECALLBACK|
                                    LIVEOBJ2_UPGRADING|LIVEOBJ2_PUSHED|LIVEOBJ2_SHOWDAMAGENUMBERS|
                                    LIVEOBJ2_DAMAGE_UNK_1000|LIVEOBJ2_UNK_800|LIVEOBJ2_TRAINING|LIVEOBJ2_BUILDPATH|
                                    LIVEOBJ2_UNK_100|LIVEOBJ2_UNK_80|LIVEOBJ2_UNK_40|LIVEOBJ2_UNK_20|LIVEOBJ2_UNK_10|
                                    LIVEOBJ2_DRIVING|LIVEOBJ2_UNK_4|LIVEOBJ2_THROWN|LIVEOBJ2_THROWING);

                LegoObject_SetActivity(liveObj, Activity_Stand, 0);

                AITask_LiveObject_SetAITaskUnk(liveObj, AITask_Type_FindLoad, NULL, TRUE);
                liveObj->routeToObject->flags4 &= ~LIVEOBJ4_UNK_10000;
            }
        }
        goto objectupdate_end;
    }

    if ((liveObj->flags2 & LIVEOBJ2_RECHARGING) != LIVEOBJ2_NONE)
    {
        // TODO: Implement LegoObject_Callback_Update
    }

    if ((liveObj->flags2 & LIVEOBJ2_FIRINGPUSHER) != LIVEOBJ2_NONE)
    {
        // TODO: Implement LegoObject_Callback_Update
    }

    if ((liveObj->flags2 & LIVEOBJ2_FIRINGFREEZER) != LIVEOBJ2_NONE)
    {
        // TODO: Implement LegoObject_Callback_Update
    }

    if ((liveObj->flags1 & LIVEOBJ1_UNK_800) != LIVEOBJ1_NONE)
    {
        // TODO: Implement LegoObject_Callback_Update
    }

    if ((liveObj->flags1 & LIVEOBJ1_UNK_4000) != LIVEOBJ1_NONE)
    {
        // TODO: Implement LegoObject_Callback_Update
    }

    if ((liveObj->flags1 & LIVEOBJ1_STORING) != LIVEOBJ1_NONE)
    {
        // TODO: Implement LegoObject_Callback_Update
    }

    if ((liveObj->flags1 & LIVEOBJ1_RESTING) != LIVEOBJ1_NONE)
    {
        // TODO: Implement LegoObject_Callback_Update
    }

    if ((liveObj->flags2 & LIVEOBJ2_UNK_200000) != LIVEOBJ2_NONE)
    {
        // TODO: Implement LegoObject_Callback_Update
    }

    if ((liveObj->flags2 & LIVEOBJ2_UNK_40000) != LIVEOBJ2_NONE)
    {
        // TODO: Implement LegoObject_Callback_Update
    }

    if ((liveObj->flags2 & LIVEOBJ2_UNK_20000000) != LIVEOBJ2_NONE)
    {
        // TODO: Implement LegoObject_Callback_Update
    }

    if ((liveObj->flags1 & LIVEOBJ1_REPAIRDRAINING) != LIVEOBJ1_NONE)
    {
        // TODO: Implement LegoObject_Callback_Update
    }

    if ((liveObj->flags1 & LIVEOBJ1_EATING) != LIVEOBJ1_NONE)
    {
        // TODO: Implement LegoObject_Callback_Update
    }

    if ((liveObj->flags2 & LIVEOBJ2_UNK_40) != LIVEOBJ2_NONE)
    {
        liveObj->flags2 &= (LIVEOBJ2_UNK_80000000|LIVEOBJ2_DAMAGESHAKING|LIVEOBJ2_UNK_20000000|
                            LIVEOBJ2_RECHARGING|LIVEOBJ2_FROZEN|LIVEOBJ2_UNK_4000000|
                            LIVEOBJ2_ACTIVEELECTRICFENCE|LIVEOBJ2_FIRINGFREEZER|LIVEOBJ2_FIRINGPUSHER|
                            LIVEOBJ2_FIRINGLASER|LIVEOBJ2_UNK_200000|LIVEOBJ2_UNK_100000|LIVEOBJ2_UNK_80000|
                            LIVEOBJ2_UNK_40000|LIVEOBJ2_UNK_20000|LIVEOBJ2_TRIGGERFRAMECALLBACK|
                            LIVEOBJ2_UPGRADING|LIVEOBJ2_PUSHED|LIVEOBJ2_SHOWDAMAGENUMBERS|
                            LIVEOBJ2_DAMAGE_UNK_1000|LIVEOBJ2_UNK_800|LIVEOBJ2_TRAINING|LIVEOBJ2_BUILDPATH|
                            LIVEOBJ2_UNK_100|LIVEOBJ2_UNK_80|LIVEOBJ2_UNK_20|LIVEOBJ2_UNK_10|
                            LIVEOBJ2_DRIVING|LIVEOBJ2_UNK_4|LIVEOBJ2_THROWN|LIVEOBJ2_THROWING);

        if (liveObj->aiTask != NULL)
        {
            LegoObject_MiniFigure_EquipTool(liveObj, liveObj->aiTask->toolType);
            AITask_LiveObject_SetAITaskUnk(liveObj, AITask_Type_GetTool, NULL, TRUE);
        }

        goto objectupdate_end;
    }

    if ((liveObj->flags2 & LIVEOBJ2_TRAINING) != LIVEOBJ2_NONE)
    {
        if (liveObj->animTime > 0.0f)
        {
            liveObj->flags2 &= (LIVEOBJ2_UNK_80000000|LIVEOBJ2_DAMAGESHAKING|LIVEOBJ2_UNK_20000000|
                                LIVEOBJ2_RECHARGING|LIVEOBJ2_FROZEN|LIVEOBJ2_UNK_4000000|
                                LIVEOBJ2_ACTIVEELECTRICFENCE|LIVEOBJ2_FIRINGFREEZER|LIVEOBJ2_FIRINGPUSHER|
                                LIVEOBJ2_FIRINGLASER|LIVEOBJ2_UNK_200000|LIVEOBJ2_UNK_100000|
                                LIVEOBJ2_UNK_80000|LIVEOBJ2_UNK_40000|LIVEOBJ2_UNK_20000|
                                LIVEOBJ2_TRIGGERFRAMECALLBACK|LIVEOBJ2_UPGRADING|LIVEOBJ2_PUSHED|
                                LIVEOBJ2_SHOWDAMAGENUMBERS|LIVEOBJ2_DAMAGE_UNK_1000|LIVEOBJ2_UNK_800|
                                LIVEOBJ2_BUILDPATH|LIVEOBJ2_UNK_100|LIVEOBJ2_UNK_80|LIVEOBJ2_UNK_40|
                                LIVEOBJ2_UNK_20|LIVEOBJ2_UNK_10|LIVEOBJ2_DRIVING|LIVEOBJ2_UNK_4|
                                LIVEOBJ2_THROWN|LIVEOBJ2_THROWING);

            AITask_LiveObject_SetAITaskUnk(liveObj, AITask_Type_Train, NULL, TRUE);
        }

        goto objectupdate_end;
    }

    if ((liveObj->flags2 & LIVEOBJ2_UPGRADING) == LIVEOBJ2_NONE)
    {
        if ((liveObj->flags1 & LIVEOBJ1_PLACING) != LIVEOBJ1_NONE)
        {
            // TODO: Implement LegoObject_Callback_Update
        }

        if ((liveObj->flags1 & LIVEOBJ1_GETTINGHIT) != LIVEOBJ1_NONE)
        {
            if (liveObj->animTime > 0.0f)
                liveObj->flags1 &= ~LIVEOBJ1_GETTINGHIT;

            goto objectupdate_end;
        }

        if (liveObj->carryingThisObject != NULL)
            goto objectupdate_end;

        if ((liveObj->flags1 & LIVEOBJ1_MOVING) != LIVEOBJ1_NONE &&
            LegoObject_FUN_0043c6a0(liveObj))
        {
            LegoObject_Route_End(liveObj, TRUE);
            LegoObject_Proc_FUN_0043c7f0(liveObj);

            goto objectupdate_end;
        }

        if ((liveObj->flags1 & LIVEOBJ1_MOVING) != LIVEOBJ1_NONE)
        {
            LegoObject_Route_UpdateMovement(liveObj, elapsed);
            LegoObject_UpdateCarryingEnergy(liveObj, elapsed);
            LegoObject_RockMonster_FUN_0043ad70(liveObj);

            if ((liveObj->flags2 & LIVEOBJ2_UNK_4000000) != LIVEOBJ2_NONE)
            {
                // TODO: Implement LegoObject_Callback_Update
            }

            if ((liveObj->flags1 & LIVEOBJ1_DRILLING) != LIVEOBJ1_NONE)
            {
                LegoObject_SetActivity(liveObj, Activity_Drill, 0);
                Point2I pos;
                pos.x = liveObj->targetBlockPos.x;
                pos.y = liveObj->targetBlockPos.y;
                Level_Block_SetBusy(&pos, TRUE);
                goto objectupdate_end;
            }

            if ((liveObj->flags1 & LIVEOBJ1_LIFTING) != LIVEOBJ1_NONE)
            {
                LegoObject_SetActivity(liveObj, Activity_Reverse, 0);
                goto objectupdate_end;
            }

            if ((liveObj->flags2 & LIVEOBJ2_TRAINING) != LIVEOBJ2_NONE)
            {
                LegoObject_SetActivity(liveObj, Activity_Train, 0);
                goto objectupdate_end;
            }
            // TODO: Implement LegoObject_Callback_Update
        }

        if ((liveObj->flags1 & LIVEOBJ1_TELEPORTINGDOWN) != LIVEOBJ1_NONE)
        {
            // TODO: Implement LegoObject_Callback_Update
        }

        // TODO: Implement LegoObject_Callback_Update
    }

    if (liveObj->type == LegoObject_MiniFigure)
    {
        // TODO: Implement LegoObject_Callback_Update
    }

    if (liveObj->animTime <= 0.0f)
        goto objectupdate_end;

    if (liveObj->routeToObject == NULL || (StatsObject_GetStatsFlags2(liveObj->routeToObject) & STATS2_UPGRADEBUILDING) == STATS2_NONE)
    {
        liveObj->flags2 &= (LIVEOBJ2_UNK_80000000|LIVEOBJ2_DAMAGESHAKING|LIVEOBJ2_UNK_20000000|LIVEOBJ2_RECHARGING|
                            LIVEOBJ2_FROZEN|LIVEOBJ2_UNK_4000000|LIVEOBJ2_ACTIVEELECTRICFENCE|LIVEOBJ2_FIRINGFREEZER|
                            LIVEOBJ2_FIRINGPUSHER|LIVEOBJ2_FIRINGLASER|LIVEOBJ2_UNK_200000|LIVEOBJ2_UNK_100000|
                            LIVEOBJ2_UNK_80000|LIVEOBJ2_UNK_40000|LIVEOBJ2_UNK_20000|LIVEOBJ2_TRIGGERFRAMECALLBACK|
                            LIVEOBJ2_PUSHED|LIVEOBJ2_SHOWDAMAGENUMBERS|LIVEOBJ2_DAMAGE_UNK_1000|LIVEOBJ2_UNK_800|
                            LIVEOBJ2_TRAINING|LIVEOBJ2_BUILDPATH|LIVEOBJ2_UNK_100|LIVEOBJ2_UNK_80|LIVEOBJ2_UNK_40|
                            LIVEOBJ2_UNK_20|LIVEOBJ2_UNK_10|LIVEOBJ2_DRIVING|LIVEOBJ2_UNK_4|LIVEOBJ2_THROWN|
                            LIVEOBJ2_THROWING);

        goto objectupdate_end;
    }

    if ((liveObj->routeToObject->flags2 & LIVEOBJ2_UPGRADING) != LIVEOBJ2_NONE)
        goto objectupdate_end;



    // TODO: Implement LegoObject_Callback_Update

objectupdate_end:

    // TODO: Implement LegoObject_Callback_Update

    return FALSE;
}

B32 LegoObject_RunThroughLists(LegoObject_RunThroughListsCallback callback, void* search, B32 skipUpgradeParts)
{
    for (U32 tblIndex = 0; tblIndex < objectGlobs.listCount; tblIndex++)
    {
        if (objectGlobs.listSet[tblIndex] != NULL)
        {
            U32 size = 1 << ((U8)tblIndex & 0x1f);
            for (U32 i = 0; i < size; i++)
            {

                lpLegoObject liveObj = &objectGlobs.listSet[tblIndex][i];
                if (liveObj != NULL && liveObj->nextFree == liveObj &&
                    (skipUpgradeParts == FALSE || (liveObj->flags3 & LIVEOBJ3_UPGRADEPART) == LIVEOBJ3_NONE) &&
                    callback(liveObj, search))
                {
                    return TRUE;
                }
            }
        }
    }

    return FALSE;
}

void LegoObject_Flocks_Update_FUN_0044c1c0(F32* pElapsed)
{
    // TODO: Implement LegoObject_Flocks_Update_FUN_0044c1c0
}

// Marks blocks in the first PowerGrid list as unpowered and moves them to the second list. Then
// clears the first list.
void Level_PowerGrid_ClearBlockPowered_100_Points28C()
{
    // TODO: Implement Level_PowerGrid_ClearBlockPowered_100_Points28C
}

// Updates the block surfaces for unpowered blocks in the second PowerGrid list. Then clears the
// list.
void Level_PowerGrid_UpdateLevelBlocks_PointsAAC()
{
    // TODO: Implement Level_PowerGrid_UpdateLevelBlocks_PointsAAC
}

// If we're currently in the update loop, then delay recalculations until the end of the update loop/next tick(?).
//
// Otherwise, we can turn on power updating mode right now.
void LegoObject_RequestPowerGridUpdate()
{
    // TODO: Implement LegoObject_RequestPowerGridUpdate
}

F32 LegoObject_GetWorldZCallback(F32 xPos, F32 yPos, struct Map3D* map)
{
    return Map3D_GetWorldZ(map, xPos, yPos);
}

// The same as `LegoObject_GetWorldZCallback`, but returns a lower Z value with over Lake terrain.
// Objects wading in a lake (aka, not sailing) will have their Z lowered a bit, and have it at the lowest near the center of a lake BLOCK.
F32 LegoObject_GetWorldZCallback_Lake(F32 xPos, F32 yPos, struct Map3D* map)
{
    Point2I blockPos;

    F32 zModifier = 0.0f;
    if (Map3D_WorldToBlockPos(map, xPos, yPos, &blockPos.x, &blockPos.y, &zModifier) != 0)
    {
        if (legoGlobs.currLevel->blocks[(legoGlobs.currLevel->width * blockPos.y + blockPos.x)].terrain == Lego_SurfaceType_Lake)
        {
            zModifier *= 8.0f;
        }
        else
        {
            zModifier = 0.0f;
        }
    }

    return Map3D_GetWorldZ(map, xPos, yPos) + zModifier;
}

const char* Object_GetLangName(LegoObject_Type objType, LegoObject_ID objID)
{
    // TODO: Implement Object_GetLangName

    return NULL;
}

void HiddenObject_Add(void* objModel, LegoObject_Type objType, LegoObject_ID objID, Point2F* worldPos, F32 heading, F32 health, const char* thisOLName, const char* drivingOLName)
{
    // TODO: Implement HiddenObject_Add
}

lpMeshLOD LegoObject_LoadMeshLOD(lpConfig act, const char* gameName, const char* dirname, LOD_PolyLevel polyLOD, U32 numCameraFrames)
{
    char buff[256];

    lpMeshLOD firstMeshLOD = NULL;
    lpMeshLOD lastMeshLOD = NULL;

    if (polyLOD != LOD_FPPoly)
    {
        numCameraFrames = 1;
    }

    for (U32 setID = 0; setID < numCameraFrames; setID++)
    {
        if (polyLOD == LOD_FPPoly)
        {
            sprintf(buff, "FPPoly::Camera%i", setID + 1);
        }
        else if (polyLOD == LOD_HighPoly)
        {
            sprintf(buff, "HighPoly");
        }
        else if (polyLOD == LOD_MediumPoly)
        {
            sprintf(buff, "MediumPoly");
        }

        for (lpConfig conf = Config_FindArray(act, Config_BuildStringID(gameName, buff, 0)); conf != NULL; conf = Config_GetNextItem(conf))
        {
            char* partName = conf->itemName;
            if (*partName == '!')
            {
                if ((mainGlobs.flags & MAIN_FLAG_REDUCEPROMESHES) == MAIN_FLAG_NONE)
                {
                    partName++;
                    goto somelabel;
                }
            }
            else
            {
somelabel:
                if (_stricmp("NULL", conf->dataString) == 0)
                {
                    lastMeshLOD = MeshLOD_CreateEmpty(lastMeshLOD, partName, setID);
                }
                else
                {
                    lastMeshLOD = MeshLOD_Create(lastMeshLOD, partName, dirname, conf->dataString, setID);
                }

                if (firstMeshLOD == NULL)
                    firstMeshLOD = lastMeshLOD;
            }
        }
    }

    return firstMeshLOD;
}

B32 LegoObject_Route_AllocPtr_FUN_004419c0(lpLegoObject liveObj, U32 count, S32* newX, S32* newY, Point2F* pos2D)
{
    if (LegoObject_Check_LotsOfFlags1AndFlags2_FUN_0043bdb0(liveObj))
        return FALSE;

    S32 someNum = -1;
    B32 theBool = FALSE;
    Point2F somePos;
    somePos.x = 0.5f;
    somePos.y = 0.5f;

    if ((liveObj->flags4 & LIVEOBJ4_DOCKOCCUPIED) != LIVEOBJ4_NONE)
    {
        liveObj->routeToObject = NULL;
        liveObj->flags4 &= ~LIVEOBJ4_DOCKOCCUPIED;
    }

    RoutingBlock* routingBlock = Mem_Alloc(count * sizeof(RoutingBlock));

    if (routingBlock != NULL && count != 0)
    {
        //S32 someNum3 = newY - newX; // TODO: What?
        for (U32 i = 0; i < count; i++)
        {
            routingBlock[i].blockPos.x = newX[i];
            routingBlock[i].blockPos.y = newY[i];

            if (pos2D == NULL || i != count - 1)
            {
                if (((liveObj->flags3 & LIVEOBJ3_CENTERBLOCKIDLE) == LIVEOBJ3_NONE) &&
                    (StatsObject_GetStatsFlags1(liveObj) & STATS1_ROUTEAVOIDANCE) != STATS1_NONE)
                {
                    routingBlock[i].blockOffset.x = Maths_RandRange(0.3f, 0.7f);
                    routingBlock[i].blockOffset.y = Maths_RandRange(0.3f, 0.7f);
                }
                else
                {
                    routingBlock[i].blockOffset.x = 0.5f;
                    routingBlock[i].blockOffset.y = 0.5f;
                }
            }
            else
            {
                Point2F* centerDir = &somePos;
                Point2F* blockPosF = NULL;
                Point2F* wPos2D = pos2D;
                Map3D_FUN_0044fb30(Lego_GetMap(), wPos2D, blockPosF, centerDir);
                routingBlock[i].blockOffset.x = somePos.x;
                routingBlock[i].blockOffset.y = somePos.y;
            }

            routingBlock[i].flagsByte = ROUTE_FLAG_NONE;
            routingBlock[i].actionByte = ROUTE_ACTION_NONE;

            S32 someNum2 = someNum;
            if (i < count - 1)
            {
                // TODO: What even are the arguments to this function???
                someNum2 = Map3D_CheckRoutingComparison_FUN_00450b60(newX[i + 1], newY[i + 1], newX[i], newY[i]);
                if (someNum2 == -1)
                {
                    theBool = TRUE;
                    break;
                }

                // remember there is a hack here
                if (i != 0 && someNum != someNum2 &&
                    Lego_GetCrossTerrainType(liveObj, newX[i + 1], newY[i + 1], newX[i - 1], newY[i - 1], FALSE) == CrossTerrainType_Diagonal)
                {
                    routingBlock[i].flagsByte |= ROUTE_FLAG_UNK_10;
                }
            }

            someNum = someNum2;
        }
    }

    if (!theBool)
    {
        LegoObject_Route_End(liveObj, FALSE);
        // TODO: This seems wrong?
        liveObj->flags3 &= (LIVEOBJ3_POWEROFF|LIVEOBJ3_UNK_40000000|LIVEOBJ3_HASPOWER|LIVEOBJ3_CANROUTERUBBLE|
                          LIVEOBJ3_MONSTER_UNK_8000000|LIVEOBJ3_CANGATHER|LIVEOBJ3_UNK_2000000|
                          LIVEOBJ3_UNK_1000000|LIVEOBJ3_REMOVING|LIVEOBJ3_AITASK_UNK_400000|LIVEOBJ3_SELECTED|
                          LIVEOBJ3_ALLOWCULLING_UNK|LIVEOBJ3_UPGRADEPART|LIVEOBJ3_CANDAMAGE|LIVEOBJ3_SIMPLEOBJECT|
                          LIVEOBJ3_UNK_10000|LIVEOBJ3_CANDYNAMITE|LIVEOBJ3_UNK_4000|LIVEOBJ3_UNK_2000|
                          LIVEOBJ3_CENTERBLOCKIDLE|LIVEOBJ3_UNUSED_800|LIVEOBJ3_UNK_200|LIVEOBJ3_CANSELECT|
                          LIVEOBJ3_CANYESSIR|LIVEOBJ3_CANPICKUP|LIVEOBJ3_CANCARRY|LIVEOBJ3_CANFIRSTPERSON|
                          LIVEOBJ3_CANTURN|LIVEOBJ3_CANREINFORCE|LIVEOBJ3_CANDIG|LIVEOBJ3_UNK_1);
        liveObj->flags1 |= LIVEOBJ1_MOVING;
        liveObj->routeBlocks = routingBlock;
        liveObj->routeBlocksTotal = count;
        liveObj->routeBlocksCurrent = 0;
        liveObj->routeCurveCurrDist = 0.0f;
        liveObj->routeCurveTotalDist = 0.0f;
        return TRUE;
    }
    Mem_Free(routingBlock);

    return FALSE;
}

void LegoObject_Route_End(lpLegoObject liveObj, B32 completed)
{
    if (liveObj->routeBlocks != NULL)
    {
        if (!completed)
        {
            // Loop from current block to total blocks (ignoring 0-current)
            for (U32 i = liveObj->routeBlocksCurrent; i < liveObj->routeBlocksTotal; i++)
            {
                if (liveObj->routeBlocks[i].actionByte == ROUTE_ACTION_GATHERROCK)
                {
                    // TODO: Implement LegoObject_Route_End
                    if (liveObj->routeToObject != NULL)
                    {
                        if (liveObj->routeToObject->type == LegoObject_Boulder)
                        {
                            LegoObject_DestroyBoulder_AndCreateExplode(liveObj->routeToObject);
                        }
                        else
                        {
                            liveObj->routeToObject->interactObject = NULL;
                        }

                        liveObj->routeToObject = NULL;
                    }
                    break;
                }

                if (liveObj->routeBlocks[i].actionByte == ROUTE_ACTION_STORE)
                {
                    liveObj->routeToObject = NULL;
                    break;
                }
            }

            if (liveObj->type == LegoObject_RockMonster)
            {
                if (((liveObj->flags1 & LIVEOBJ1_CARRYING) != LIVEOBJ1_NONE) &&
                    (liveObj->carriedObjects[0]->type == LegoObject_Boulder))
                {
                    LegoObject_DropCarriedObject(liveObj, FALSE);
                }
                AITask_LiveObject_FUN_00403b30(liveObj, AITask_Type_Gather, NULL);
                AITask_LiveObject_FUN_00403b30(liveObj, AITask_Type_Repair, NULL);
            }

            if ((liveObj->flags1 & LIVEOBJ1_DRILLING) != LIVEOBJ1_NONE)
            {
                Point2I blockPos;
                blockPos.x = (S32)liveObj->targetBlockPos.x;
                blockPos.y = (S32)liveObj->targetBlockPos.y;
                Level_Block_SetBusy(&blockPos, FALSE);
            }
        }
        else if (liveObj->type == LegoObject_MiniFigure)
        {
            Creature_SetOrientation(liveObj->miniFigure, liveObj->point_298.x, liveObj->point_298.y);
            liveObj->faceDirection.x = liveObj->point_298.x;
            liveObj->faceDirection.y = liveObj->point_298.y;
        }
        Mem_Free(liveObj->routeBlocks);
        liveObj->routeBlocks = NULL;
        AITask_Route_End(liveObj, completed);
    }
    liveObj->routeBlocksTotal = 0;
    liveObj->routeBlocksCurrent = 0;
    liveObj->routeCurveCurrDist = 0.0f;
    liveObj->routeCurveTotalDist = 0.0f;
    liveObj->routeCurveInitialDist = 0.0f;
    liveObj->flags1 &= ~(LIVEOBJ1_RUNNINGAWAY | LIVEOBJ1_MOVING);
    liveObj->flags3 &= (LIVEOBJ3_POWEROFF|LIVEOBJ3_UNK_40000000|LIVEOBJ3_HASPOWER|LIVEOBJ3_CANROUTERUBBLE|
        LIVEOBJ3_MONSTER_UNK_8000000|LIVEOBJ3_CANGATHER|LIVEOBJ3_UNK_2000000|LIVEOBJ3_UNK_1000000|
        LIVEOBJ3_REMOVING|LIVEOBJ3_AITASK_UNK_400000|LIVEOBJ3_SELECTED|LIVEOBJ3_ALLOWCULLING_UNK|
        LIVEOBJ3_UPGRADEPART|LIVEOBJ3_CANDAMAGE|LIVEOBJ3_SIMPLEOBJECT|LIVEOBJ3_UNK_10000|
        LIVEOBJ3_CANDYNAMITE|LIVEOBJ3_UNK_2000|LIVEOBJ3_CENTERBLOCKIDLE|LIVEOBJ3_UNUSED_800|
        LIVEOBJ3_UNK_200|LIVEOBJ3_CANSELECT|LIVEOBJ3_CANYESSIR|LIVEOBJ3_CANPICKUP|LIVEOBJ3_CANCARRY|
        LIVEOBJ3_CANFIRSTPERSON|LIVEOBJ3_CANTURN|LIVEOBJ3_CANREINFORCE|LIVEOBJ3_CANDIG|
        LIVEOBJ3_UNK_1);

    if (liveObj->routeToObject != NULL)
    {
        liveObj->routeToObject->flags4 &= ~LIVEOBJ4_UNK_10;
    }

    if ((liveObj->flags1 & LIVEOBJ1_LIFTING) != LIVEOBJ1_NONE)
    {
        liveObj->faceDirection.x *= -1.0f;
        liveObj->faceDirection.y *= -1.0f;
        liveObj->faceDirection.z *= -1.0f;
        liveObj->flags1 &= ~LIVEOBJ1_LIFTING;
    }

    LegoObject_SetActivity(liveObj, Activity_Stand, 0);
}

void LegoObject_DropCarriedObject(lpLegoObject liveObj, B32 putAway)
{
    // TODO: Implement LegoObject_DropCarriedObject
}

void LegoObject_DestroyBoulder_AndCreateExplode(lpLegoObject liveObj)
{
    // TODO: Implement LegoObject_DestroyBoulder_AndCreateExplode
}

F32 LegoObject_MoveAnimation(lpLegoObject liveObj, F32 elapsed)
{
    F32 routeSpeed;
    F32 transSpeed;
    LegoObject_CalculateSpeeds(liveObj, elapsed, &routeSpeed, &transSpeed);

    if ((liveObj->flags1 & LIVEOBJ1_UNK_4000) != LIVEOBJ1_NONE)
        goto endFunc;

    LegoObject_Type objType = liveObj->type;

    if (objType == LegoObject_Vehicle)
    {
        liveObj->animTime = Vehicle_MoveAnimation(liveObj->vehicle, transSpeed, elapsed, liveObj->animRepeat);
    }
    else if (objType == LegoObject_MiniFigure)
    {
        liveObj->animTime = Creature_MoveAnimation(liveObj->miniFigure, transSpeed, liveObj->animRepeat);
    }
    else if (objType == LegoObject_RockMonster)
    {
        liveObj->animTime = Creature_MoveAnimation(liveObj->rockMonster, transSpeed, liveObj->animRepeat);
    }
    else if (objType == LegoObject_Building)
    {
        liveObj->animTime = Building_MoveAnimation(liveObj->building, transSpeed, liveObj->animRepeat);
    }
    else
    {
        if ((liveObj->flags3 & LIVEOBJ3_SIMPLEOBJECT) != LIVEOBJ3_NONE)
        {
            LegoObject_SimpleObject_FUN_00448160(liveObj, elapsed);
            goto endFunc;
        }

        if (objType != LegoObject_UpgradePart)
            goto endFunc;

        liveObj->animTime = Upgrade_Part_MoveAnimation(liveObj->upgradePart, transSpeed, liveObj->animRepeat);
    }

endFunc:
    if (liveObj->animTime > 0.0f)
        AITask_LiveObject_Unk_UpdateAITask_AnimationWait(liveObj);

    return elapsed;
}

void LegoObject_CalculateSpeeds(lpLegoObject liveObj, F32 elapsed, F32* outRouteSpeed, F32* outTransSpeed)
{
    // TODO: Implement LegoObject_CalculateSpeeds

    // TEMP: Set to 1.0f for now
    *outRouteSpeed = 1.0f;
    *outTransSpeed = 1.0f;
}

B32 LegoObject_TryDepart_FUN_004499c0(lpLegoObject liveObj)
{
    // TODO: Implement LegoObject_TryDepart_FUN_004499c0

    return FALSE;
}

void LegoObject_SimpleObject_FUN_00448160(lpLegoObject liveObj, F32 elapsed)
{
    // TODO: Implement LegoObject_SimpleObject_FUN_00448160
}

lpContainer LegoObject_GetDepositNull(lpLegoObject liveObj)
{
    // TODO: Implement LegoObject_GetDepositNull

    return NULL;
}

B32 LegoObject_DoOpeningClosing(lpLegoObject liveObj, B32 open)
{
    // TODO: Implement LegoObject_DoOpeningClosing

    return FALSE;
}

B32 LegoObject_FUN_0043c6a0(lpLegoObject liveObj)
{
    // TODO: Implement LegoObject_FUN_0043c6a0

    return FALSE;
}

void LegoObject_Proc_FUN_0043c7f0(lpLegoObject liveObj)
{
    // TODO: Implement LegoObject_Proc_FUN_0043c7f0
}

void LegoObject_Route_UpdateMovement(lpLegoObject liveObj, F32 elapsed)
{
    if (liveObj->routeBlocks == NULL)
    {
        LegoObject_Route_End(liveObj, FALSE);
        return;
    }

    if (liveObj->type == LegoObject_Vehicle)
    {
        liveObj->elapsedTime1 = 0.0f;
    }

    B32 theBool = TRUE;
    if (liveObj->routeCurveTotalDist <= liveObj->routeCurveCurrDist)
    {
        LiveFlags3 lflags3 = liveObj->flags3;
        if ((lflags3 & LIVEOBJ3_UNK_400) != LIVEOBJ3_NONE)
        {
            liveObj->flags3 &= (LIVEOBJ3_POWEROFF|LIVEOBJ3_UNK_40000000|LIVEOBJ3_HASPOWER|
                        LIVEOBJ3_CANROUTERUBBLE|LIVEOBJ3_MONSTER_UNK_8000000|LIVEOBJ3_CANGATHER|
                        LIVEOBJ3_UNK_2000000|LIVEOBJ3_UNK_1000000|LIVEOBJ3_REMOVING|
                        LIVEOBJ3_AITASK_UNK_400000|LIVEOBJ3_SELECTED|LIVEOBJ3_ALLOWCULLING_UNK|
                        LIVEOBJ3_UPGRADEPART|LIVEOBJ3_CANDAMAGE|LIVEOBJ3_SIMPLEOBJECT|
                        LIVEOBJ3_UNK_10000|LIVEOBJ3_CANDYNAMITE|LIVEOBJ3_UNK_4000|LIVEOBJ3_UNK_2000|
                        LIVEOBJ3_CENTERBLOCKIDLE|LIVEOBJ3_UNUSED_800|LIVEOBJ3_UNK_200|
                        LIVEOBJ3_CANSELECT|LIVEOBJ3_CANYESSIR|LIVEOBJ3_CANPICKUP|LIVEOBJ3_CANCARRY|
                        LIVEOBJ3_CANFIRSTPERSON|LIVEOBJ3_CANTURN|LIVEOBJ3_CANREINFORCE|
                        LIVEOBJ3_CANDIG|LIVEOBJ3_UNK_1);

            switch (liveObj->routeBlocks[liveObj->routeBlocksCurrent].actionByte)
            {
                // TODO: Implement LegoObject_Route_UpdateMovement
            }

            if ((liveObj->routeBlocks[liveObj->routeBlocksCurrent].flagsByte & ROUTE_FLAG_UNK_20) != ROUTE_FLAG_NONE)
            {
                liveObj->flags2 |= LIVEOBJ2_UNK_80000;
            }

            LegoObject_Route_End(liveObj, TRUE);

            if ((liveObj->flags1 & LIVEOBJ1_SCAREDBYPLAYER) == LIVEOBJ1_NONE)
                return;

            if (Maths_Rand() % (U32)(S64)(10.0f / elapsed))
            {
                LiveObject_FUN_00433b40(liveObj, 150.0f, TRUE);
            }
            else
            {
                liveObj->flags1 &= ~LIVEOBJ1_SCAREDBYPLAYER;
            }

            return;
        }

        B32 useRoutingPos = FALSE;
        if (liveObj->routeBlocksTotal < 2)
        {
            liveObj->flags3 = lflags3 | LIVEOBJ3_UNK_400;
            liveObj->routeBlocks[liveObj->routeBlocksCurrent].flagsByte |= ROUTE_FLAG_UNK_8;
        }
        else
        {
            liveObj->routeBlocksCurrent++;
            if (((liveObj->routeBlocks[liveObj->routeBlocksCurrent].flagsByte & ROUTE_FLAG_UNK_10) != ROUTE_FLAG_NONE) &&
                (liveObj->routeBlocks[liveObj->routeBlocksCurrent].actionByte != ROUTE_ACTION_UNK_1))
            {
                useRoutingPos = TRUE;
            }
        }

        U32 current = liveObj->routeBlocksCurrent;
        if (useRoutingPos)
            current++;

        if (liveObj->routeBlocks[current].actionByte == ROUTE_ACTION_UNK_5)
        {
            // TODO: Implement LegoObject_Route_UpdateMovement
        }

        theBool = LegoObject_RoutingUnk_SetupCurve_FUN_00444940(liveObj, useRoutingPos,
            liveObj->flags3 & LIVEOBJ3_CANTURN,
            (liveObj->flags1 & LIVEOBJ1_CARRYING) == LIVEOBJ1_NONE
            );

        if (liveObj->routeBlocksTotal - 1 <= liveObj->routeBlocksCurrent)
            liveObj->flags3 |= LIVEOBJ3_UNK_400;
    }

    if (!theBool)
        return;

    Point2F drillPos;
    Point2I drillBlockPos;

    if (((liveObj->flags1 & LIVEOBJ1_DRILLING) == LIVEOBJ1_NONE
        && liveObj->routeBlocks[liveObj->routeBlocksCurrent].actionByte == ROUTE_ACTION_UNK_1) &&
        (theBool = LegoObject_GetDrillNullPosition(liveObj, &drillPos.x, &drillPos.y), theBool != 0) &&
        (theBool = Lego_WorldToBlockPos_NoZ(drillPos.x, drillPos.y, &drillBlockPos.x, &drillBlockPos.y), theBool != 0 &&
        (theBool = Level_Block_IsWall(drillBlockPos.x, drillBlockPos.y), theBool != 0) &&
        (drillBlockPos.x == liveObj->routeBlocks[liveObj->routeBlocksCurrent].blockPos.x && drillBlockPos.y == liveObj->routeBlocks[liveObj->routeBlocksCurrent].blockPos.y)))
    {
        theBool = TRUE;

        theBool = LiveObject_BlockCheck_FUN_004326a0(liveObj, liveObj->targetBlockPos.x, liveObj->targetBlockPos.y, liveObj->flags3 & LIVEOBJ3_UNK_2000000, theBool);
        if (theBool)
            liveObj->flags1 |= (LIVEOBJ1_DRILLINGSTART | LIVEOBJ1_DRILLING);
        else
            liveObj->flags1 |= LIVEOBJ1_CANTDO;
    }

    F32 routeSpeed;
    F32 transSpeed;
    LegoObject_CalculateSpeeds(liveObj, elapsed, &routeSpeed, &transSpeed);

    if ((liveObj->flags3 & LIVEOBJ3_UNK_4000) == LIVEOBJ3_NONE)
    {
        liveObj->routeCurveCurrDist += routeSpeed;
    }
    if (liveObj->routeCurveTotalDist < liveObj->routeCurveCurrDist)
    {
        F32 temp = liveObj->routeCurveCurrDist;
        liveObj->routeCurveCurrDist = liveObj->routeCurveTotalDist;
        liveObj->routeCurveInitialDist = temp - liveObj->routeCurveTotalDist;
    }

    Point2F curveInter;
    BezierCurve_Interpolate(&liveObj->routeCurve, liveObj->routeCurveCurrDist, &curveInter);

    RouteAction action = liveObj->routeBlocks[liveObj->routeBlocksCurrent].actionByte;

    if (action == ROUTE_ACTION_GATHERROCK)
    {
        // TODO: Implement LegoObject_Route_UpdateMovement
    }
    else if (action == ROUTE_ACTION_UNK_12)
    {
        // TODO: Implement LegoObject_Route_UpdateMovement
    }
    else if (action == ROUTE_ACTION_PLACE &&
            (liveObj->routeBlocks[liveObj->routeBlocksCurrent].flagsByte & ROUTE_UNK_MASK_c) == ROUTE_FLAG_NONE)
    {
        // TODO: Implement LegoObject_Route_UpdateMovement
    }

    lpLegoObject lobj;
    if (((((liveObj->flags3 & LIVEOBJ3_UNUSED_800) == LIVEOBJ3_NONE) &&
        (action = liveObj->routeBlocks[liveObj->routeBlocksCurrent].actionByte,
            action != ROUTE_ACTION_STORE)) && (action != ROUTE_ACTION_REPAIRDRAIN)) &&
            (((action != ROUTE_ACTION_GATHERROCK && (action != ROUTE_ACTION_UNK_5)) &&
                (lobj = LegoObject_DoCollisionCallbacks_FUN_00446030(liveObj, &curveInter, 0.0f, FALSE),
                    lobj != NULL))))
    {
        StatsFlags1 sflags1 = StatsObject_GetStatsFlags1(liveObj);
        if ((sflags1 & STATS1_BUMPDAMAGE) != STATS1_NONE && lobj->type == LegoObject_Vehicle)
        {
            LegoObject_Route_End(liveObj, FALSE);
            LegoObject_SetActivity(liveObj, Activity_Repair, 0);
            LegoObject_UpdateActivityChange(liveObj);

            liveObj->routeToObject = lobj;
            liveObj->flags2 |= LIVEOBJ2_UNK_40000;

            return;
        }

        // stop moving because we've collided with something
        // so edit our curve back to be empty
        LegoObject_RoutingUnk_SetupCurve_FUN_00444940(liveObj, FALSE, FALSE, FALSE);
        liveObj->flags3 |= LIVEOBJ3_UNUSED_800;
        liveObj->routeCurveCurrDist += routeSpeed;
    }

    if ((liveObj->flags3 & LIVEOBJ3_UNK_4000) == LIVEOBJ3_NONE)
    {
        LegoObject_UpdateRoutingVectors_SetPosition_FUN_004428b0(liveObj, curveInter.x, curveInter.y);
    }

    if (liveObj->routeBlocks[liveObj->routeBlocksTotal - 1].actionByte == ROUTE_ACTION_CLEAR &&
        liveObj->type == LegoObject_Vehicle)
    {
        // TODO: Implement LegoObject_Route_UpdateMovement
    }
}

B32 LegoObject_RoutingUnk_SetupCurve_FUN_00444940(lpLegoObject liveObj, B32 useRoutingPos, B32 flags3_8, B32 notCarrying)
{
    B32 result = TRUE;

    Point2I objBlockPos;
    LegoObject_GetBlockPos(liveObj, &objBlockPos.x, &objBlockPos.y);

    Point2F objWorldPos;
    LegoObject_GetPosition(liveObj, &objWorldPos.x, &objWorldPos.y);

    Point2I blockPos = liveObj->routeBlocks[liveObj->routeBlocksCurrent].blockPos;

    Point2F faceDir;
    faceDir.x = liveObj->faceDirection.x;
    faceDir.y = liveObj->faceDirection.y;

    Point2F blockWorldPos;
    if (useRoutingPos)
    {
        // this is unused in the final version of the game
        Point2F unusedBlockWorldPos;
        Map3D_BlockToWorldPos(Lego_GetMap(), blockPos.x, blockPos.y, &unusedBlockWorldPos.x, &unusedBlockWorldPos.y);

        // but this is used
        liveObj->routeBlocksCurrent++;

        Point2I nextBlockPos;
        nextBlockPos.x = liveObj->routeBlocks[liveObj->routeBlocksCurrent].blockPos.x;
        nextBlockPos.y = liveObj->routeBlocks[liveObj->routeBlocksCurrent].blockPos.y;

        Map3D_BlockToWorldPos(Lego_GetMap(), nextBlockPos.x, nextBlockPos.y, &blockWorldPos.x, &blockWorldPos.y);
    }
    else
    {
        Map3D_BlockToWorldPos(Lego_GetMap(), blockPos.x, blockPos.y, &blockWorldPos.x, &blockWorldPos.y);
        blockPos.x = objBlockPos.x;
        blockPos.y = objBlockPos.y;
    }

    Point2F finalPos;
    Map3D_BlockToWorldPos(Lego_GetMap(), blockPos.x, blockPos.y, &finalPos.x, &finalPos.y);

    Point2F somePoint;
    somePoint.x = 0.0f;
    somePoint.y = 0.0f;

    F32 someValY = somePoint.y;
    F32 someValX = somePoint.x;

    Point2F blockOffset;
    blockOffset.x = somePoint.x;
    blockOffset.y = somePoint.y;
    if (liveObj->routeBlocks[liveObj->routeBlocksCurrent].actionByte != ROUTE_ACTION_UNK_1)
    {
        // TODO: Implement LegoObject_RoutingUnk_SetupCurve_FUN_00444940
    }

    if (someValY == 0.0f)
    {
        // TODO: Implement LegoObject_RoutingUnk_SetupCurve_FUN_00444940
    }

    RouteFlags rflags = liveObj->routeBlocks[liveObj->routeBlocksCurrent].flagsByte;
    if ((rflags & ROUTE_FLAG_GOTOBUILDING) != ROUTE_FLAG_NONE)
    {
        switch (rflags & ROUTE_DIRECTION_MASK)
        {
            case ROUTE_FLAG_NONE:
                finalPos.x = 0.0f;
                finalPos.y = 1.0f;
                flags3_8 = FALSE;
                break;
            case ROUTE_DIRECTION_1:
                finalPos.x = 1.0f;
                finalPos.y = 0.0f;
                flags3_8 = FALSE;
                break;
            case ROUTE_DIRECTION_2:
                finalPos.x = 0.0f;
                finalPos.y = -1.0f;
                flags3_8 = FALSE;
                break;
            case ROUTE_DIRECTION_MASK:
                finalPos.x = -1.0f;
                finalPos.y = 0.0f;
                flags3_8 = FALSE;
                break;
            default:
                flags3_8 = FALSE;
                break;
        }
    } else if ((rflags & ROUTE_FLAG_RUNAWAY) != ROUTE_FLAG_NONE)
    {
        finalPos.x = blockWorldPos.x - objWorldPos.x;
        finalPos.y = blockWorldPos.y - objWorldPos.y;
    } else if (((rflags & ROUTE_FLAG_UNK_8) != ROUTE_FLAG_NONE) &&
               ((blockOffset.x != 0.0f || blockOffset.y != 0.0f)))
    {
        finalPos.x = blockOffset.x;
        finalPos.y = blockOffset.y;
    }
    else
    {
        finalPos.x = blockWorldPos.x - finalPos.x;
        finalPos.y = blockWorldPos.y - finalPos.y;

        if (someValY != 0.0f && someValX == 0.0f)
        {
            if (fabsf(finalPos.x) <= fabsf(finalPos.y))
                finalPos.x = 0.0f;
            else
                finalPos.y = 0.0f;
        }
        else if (finalPos.x == 0.0f || finalPos.y == 0.0f)
        {
            finalPos.x = blockWorldPos.x - objWorldPos.x;
            finalPos.y = blockWorldPos.y - objWorldPos.y;
        }
    }

    if (flags3_8)
    {
        // TODO: Implement LegoObject_RoutingUnk_SetupCurve_FUN_00444940
    }

    F32 lenThing = sqrtf((objWorldPos.y - blockWorldPos.y) * (objWorldPos.y - blockWorldPos.y) +
                       (objWorldPos.x - blockWorldPos.x) * (objWorldPos.x - blockWorldPos.x)) * 0.2f;
    if (lenThing <= 0.1f)
    {
        liveObj->routeCurve.points[0].x = objWorldPos.x;
        liveObj->routeCurve.points[0].y = objWorldPos.y;
        liveObj->routeCurve.count = 0;
    }
    else
    {
        if (_finite(faceDir.x) == 0)
        {
            faceDir.x = 1.0f;
            faceDir.y = 0.0f;
        }

        BezierCurve_Vector2DChangeLength(&faceDir, lenThing);
        BezierCurve_Vector2DChangeLength(&finalPos, lenThing * 1.6f);
        BezierCurve_BuildPoints(&liveObj->routeCurve, &objWorldPos, &faceDir, &blockWorldPos, &finalPos, 50);

        LegoObject_Route_CurveSolid_FUN_004454a0(liveObj);
    }

    liveObj->routeCurveTotalDist = BezierCurve_UpdateDistances(&liveObj->routeCurve);
    liveObj->flags3 &= (LIVEOBJ3_POWEROFF|LIVEOBJ3_UNK_40000000|LIVEOBJ3_HASPOWER|LIVEOBJ3_CANROUTERUBBLE|
                        LIVEOBJ3_MONSTER_UNK_8000000|LIVEOBJ3_CANGATHER|LIVEOBJ3_UNK_2000000|
                        LIVEOBJ3_UNK_1000000|LIVEOBJ3_REMOVING|LIVEOBJ3_AITASK_UNK_400000|
                        LIVEOBJ3_SELECTED|LIVEOBJ3_ALLOWCULLING_UNK|LIVEOBJ3_UPGRADEPART|
                        LIVEOBJ3_CANDAMAGE|LIVEOBJ3_SIMPLEOBJECT|LIVEOBJ3_UNK_10000|LIVEOBJ3_CANDYNAMITE|
                        LIVEOBJ3_UNK_4000|LIVEOBJ3_UNK_2000|LIVEOBJ3_CENTERBLOCKIDLE|LIVEOBJ3_UNK_400|
                        LIVEOBJ3_UNK_200|LIVEOBJ3_CANSELECT|LIVEOBJ3_CANYESSIR|LIVEOBJ3_CANPICKUP|
                        LIVEOBJ3_CANCARRY|LIVEOBJ3_CANFIRSTPERSON|LIVEOBJ3_CANTURN|LIVEOBJ3_CANREINFORCE|
                        LIVEOBJ3_CANDIG|LIVEOBJ3_UNK_1);

    liveObj->routeCurveCurrDist = liveObj->routeCurveInitialDist;
    liveObj->point_298.x = finalPos.x;
    liveObj->point_298.y = finalPos.y;

    return result;
}

void LegoObject_Route_CurveSolid_FUN_004454a0(lpLegoObject liveObj)
{
    // TODO: Implement LegoObject_Route_CurveSolid_FUN_004454a0
}

lpLegoObject LegoObject_DoCollisionCallbacks_FUN_00446030(lpLegoObject liveObj, Point2F* param2, F32 param3, B32 param4)
{
    // TODO: Implement LegoObject_DoCollisionCallbacks_FUN_00446030

    return NULL;
}

void LegoObject_UpdateRoutingVectors_SetPosition_FUN_004428b0(lpLegoObject liveObj, F32 xPos, F32 yPos)
{
    lpContainer cont;
    if (liveObj->type == LegoObject_Vehicle)
    {
        cont = Vehicle_GetActivityContainer(liveObj->vehicle);
    }
    else if (liveObj->type == LegoObject_MiniFigure)
    {
        cont = Object_GetActivityContainer((lpBasicObjectModel)liveObj->miniFigure);
    }
    else if (liveObj->type == LegoObject_RockMonster)
    {
        cont = Object_GetActivityContainer((lpBasicObjectModel)liveObj->rockMonster);
    }
    else
    {
        cont = NULL;

        // TODO: Show Error?
    }

    Point3F currentPos;
    Container_GetPosition(cont, NULL, &currentPos);

    if (xPos != currentPos.x || yPos != currentPos.y)
    {
        liveObj->faceDirection.x = xPos;
        liveObj->faceDirection.y = yPos;
        liveObj->faceDirection.z = 0.0f;

        liveObj->tempPosition.x = currentPos.x;
        liveObj->tempPosition.y = currentPos.y;
        liveObj->tempPosition.z = currentPos.z;

        liveObj->faceDirection.x -= liveObj->tempPosition.x;
        liveObj->faceDirection.y -= liveObj->tempPosition.y;
        liveObj->faceDirection.z -= liveObj->tempPosition.z;
        liveObj->faceDirection.z = 0.0f; // uh okay?

        liveObj->faceDirectionLength = 1.0f / sqrtf(liveObj->faceDirection.x * liveObj->faceDirection.x +
                                                       liveObj->faceDirection.y * liveObj->faceDirection.y +
                                                       liveObj->faceDirection.z * liveObj->faceDirection.z);

        liveObj->faceDirection.x *= liveObj->faceDirectionLength;
        liveObj->faceDirection.y *= liveObj->faceDirectionLength;
        liveObj->faceDirection.z *= liveObj->faceDirectionLength;
    }

    if (liveObj->type == LegoObject_Vehicle)
    {
        Point3F faceDir;
        if ((liveObj->flags1 & LIVEOBJ1_LIFTING) == LIVEOBJ1_NONE)
        {
            faceDir.x = liveObj->faceDirection.x;
            faceDir.y = liveObj->faceDirection.y;
            faceDir.z = liveObj->faceDirection.z;
        }
        else
        {
            faceDir.x = -liveObj->faceDirection.x;
            faceDir.y = -liveObj->faceDirection.y;
            faceDir.z = -liveObj->faceDirection.z;
        }

        Vehicle_SetOrientation(liveObj->vehicle, faceDir.x, faceDir.y, faceDir.z);
        Vehicle_SetPosition(liveObj->vehicle, xPos, yPos, LegoObject_GetWorldZCallback, Lego_GetMap());
    } else if (liveObj->type == LegoObject_MiniFigure)
    {
        Point2F faceDir;
        if ((liveObj->flags1 & LIVEOBJ1_LIFTING) == LIVEOBJ1_NONE)
        {
            faceDir.x = liveObj->faceDirection.x;
            faceDir.y = liveObj->faceDirection.y;
        }
        else
        {
            faceDir.x = -liveObj->faceDirection.x;
            faceDir.y = -liveObj->faceDirection.y;
        }

        Creature_SetOrientation(liveObj->miniFigure, faceDir.x, faceDir.y);
        Creature_SetPosition(liveObj->miniFigure, xPos, yPos, LegoObject_GetWorldZCallback_Lake, Lego_GetMap());
    } else if (liveObj->type == LegoObject_RockMonster)
    {
        Creature_SetOrientation(liveObj->rockMonster, liveObj->faceDirection.x, liveObj->faceDirection.y);
        Creature_SetPosition(liveObj->rockMonster, xPos, yPos, LegoObject_GetWorldZCallback, Lego_GetMap());
    }
}

B32 LegoObject_BlockRoute_FUN_00446c80(lpLegoObject liveObj, U32 bx, U32 by, B32 useWideRange, Direction* optout_direction, B32 dontCheckWithinSingleBlockDist)
{
    B32 someBools[8];
    memset(someBools, 0, sizeof(someBools));

    //  \X -2 -1  0  1
    //  Y\ __ __ __ __
    // -2 |__|_0|_1|__|
    // -1 |_7|__|__|_2|
    //  0 |_6|__|__|_3|
    //  1 |__|_5|_4|__|

    Point2F DIRECTIONS_THIN[4];
    DIRECTIONS_THIN[0].x = 0.0f;
    DIRECTIONS_THIN[0].y = -1.0f;
    DIRECTIONS_THIN[1].x = 1.0f;
    DIRECTIONS_THIN[1].y = 0.0f;
    DIRECTIONS_THIN[2].x = 0.0f;
    DIRECTIONS_THIN[2].y = 1.0f;
    DIRECTIONS_THIN[3].x = -1.0f;
    DIRECTIONS_THIN[3].y = 0.0f;

    Point2F DIRECTIONS_WIDE[8];
    DIRECTIONS_WIDE[0].x = -1.0f;
    DIRECTIONS_WIDE[0].y = -2.0f;
    DIRECTIONS_WIDE[1].x = 0.0f;
    DIRECTIONS_WIDE[1].y = -2.0f;
    DIRECTIONS_WIDE[2].x = 1.0f;
    DIRECTIONS_WIDE[2].y = -1.0f;
    DIRECTIONS_WIDE[3].x = 1.0f;
    DIRECTIONS_WIDE[3].y = 0.0f;
    DIRECTIONS_WIDE[4].x = 0.0f;
    DIRECTIONS_WIDE[4].y = 1.0f;
    DIRECTIONS_WIDE[5].x = -1.0f;
    DIRECTIONS_WIDE[5].y = 1.0f;
    DIRECTIONS_WIDE[6].x = -2.0f;
    DIRECTIONS_WIDE[6].y = 0.0f;
    DIRECTIONS_WIDE[7].x = -2.0f;
    DIRECTIONS_WIDE[7].y = -1.0f;

    B32 bVar2 = TRUE;

    Point2F* directions;
    U32 count;

    if (useWideRange)
    {
        directions = DIRECTIONS_WIDE;
        count = 8;
    }
    else
    {
        directions = DIRECTIONS_THIN;
        count = 4;
    }

    Point2I objBlockPos;
    LegoObject_GetBlockPos(liveObj, &objBlockPos.x, &objBlockPos.y);

    if (!dontCheckWithinSingleBlockDist)
    {
        Point2F blockWorldPos;
        Map3D_BlockToWorldPos(Lego_GetMap(), bx, by, &blockWorldPos.x, &blockWorldPos.y);
        Point2F objWorldPos;
        LegoObject_GetPosition(liveObj, &objWorldPos.x, &objWorldPos.y);
        F32 dist = sqrtf((blockWorldPos.y - objWorldPos.y) * (blockWorldPos.y - objWorldPos.y) +
                           (blockWorldPos.x - objWorldPos.x) * (blockWorldPos.x - objWorldPos.x));
        if (dist < Lego_GetMap()->blockSize)
            bVar2 = FALSE;
    }

    // get the direction from the player to the wall on a compass rose
    S32 dir = -1;
    for (S32 i = 0; i < count; i++)
    {
        directions[i].x += (F32)bx;
        directions[i].y += (F32)by;
        if (directions[i].x == (F32)objBlockPos.x && directions[i].y == (F32)objBlockPos.y && bVar2)
        {
            dir = i;

            if (useWideRange != 0 || !Level_Block_IsWall(objBlockPos.x, objBlockPos.y)) {
                break;
            }
        }
        dir = -1;
    }

    if (dir != -1)
    {
        if (!LegoObject_Route_AllocPtr_FUN_004419c0(liveObj, 1, &objBlockPos.x, &objBlockPos.y, NULL))
        {
            return FALSE;
        }

        if (optout_direction != NULL)
        {
            *optout_direction = (Direction)dir;
        }

        return TRUE;
    }

    // if we didn't find it
    S32* outNewBxs[8];
    S32* outNewBys[8];
    U32 outCount[8];

    for (U32 i = 0; i < count; i++)
    {
        if ((F32)objBlockPos.x != directions[i].x || (F32)objBlockPos.y != directions[i].y)
        {
            if (LegoObject_Route_Score_FUN_004413b0(liveObj, objBlockPos.x, objBlockPos.y, directions[i].x, directions[i].y, outNewBxs[i], outNewBys[i], outCount, NULL, NULL))
            {
                someBools[i] = TRUE;
            }
        }
    }

    S32 local_108 = -1;
    U32 local_f8 = 0xffffffff;
    for (U32 i = 0; i < count; i++)
    {
        if (local_108 != -1 && i == count)
            break;

        S32 num = i % count;
        if (someBools[num] != 0)
        {
            if (i < count)
            {
                if (Level_Block_IsWall(directions[num].x, directions[num].y))
                    continue;
            }

            if (outCount[num] < local_f8)
            {
                local_108 = num;
                local_f8 = outCount[num];
            }
        }
    }

    B32 result = FALSE;

    if (local_108 != -1)
    {
        if (LegoObject_Route_AllocPtr_FUN_004419c0(liveObj, outCount[local_108], outNewBxs[local_108], outNewBys[local_108], NULL))
        {
            if (optout_direction != NULL)
            {
                *optout_direction = (Direction)local_108;
            }
            result = TRUE;
        }
        else
        {
            result = FALSE;
        }
    }

    for (U32 i = 0; i < count; i++)
    {
        if (someBools[i] != 0)
        {
            Mem_Free(outNewBxs[i]);
            Mem_Free(outNewBys[i]);
        }
    }

    return result;
}

B32 LegoObject_RoutingPtr_Realloc_FUN_00446b80(lpLegoObject liveObj, U32 bx, U32 by)
{
    if ((liveObj->flags1 & LIVEOBJ1_MOVING) == LIVEOBJ1_NONE)
        return FALSE;

    U32 oldCount = liveObj->routeBlocksTotal;
    RoutingBlock* routeBlocks = liveObj->routeBlocks;
    U32 x = routeBlocks[oldCount - 1].blockPos.x;
    U32 y = routeBlocks[oldCount - 1].blockPos.y;
    // if going in straight line, not diagonal
    if (((bx == x && ((by == y - 1 || (by == y + 1)))) ||
        ((by == y && ((bx == x - 1 || (bx == x + 1)))))) &&
        (routeBlocks = (RoutingBlock*)Mem_ReAlloc(routeBlocks, (oldCount + 1) * sizeof(RoutingBlock)),
            routeBlocks != NULL))
    {
        liveObj->routeBlocks = routeBlocks;
        routeBlocks[liveObj->routeBlocksTotal].blockPos.x = bx;
        liveObj->routeBlocks[liveObj->routeBlocksTotal].blockPos.y = by;
        liveObj->routeBlocks[liveObj->routeBlocksTotal].blockOffset.x = 0.5f;
        liveObj->routeBlocks[liveObj->routeBlocksTotal].blockOffset.y = 0.5f;
        liveObj->routeBlocks[liveObj->routeBlocksTotal].flagsByte = ROUTE_FLAG_NONE;
        liveObj->routeBlocks[liveObj->routeBlocksTotal].actionByte = ROUTE_ACTION_UNK_1;
        liveObj->routeBlocksTotal++;
        liveObj->flags3 &= (LIVEOBJ3_POWEROFF|LIVEOBJ3_UNK_40000000|LIVEOBJ3_HASPOWER|LIVEOBJ3_CANROUTERUBBLE|
                            LIVEOBJ3_MONSTER_UNK_8000000|LIVEOBJ3_CANGATHER|LIVEOBJ3_UNK_2000000|
                            LIVEOBJ3_UNK_1000000|LIVEOBJ3_REMOVING|LIVEOBJ3_AITASK_UNK_400000|LIVEOBJ3_SELECTED|
                            LIVEOBJ3_ALLOWCULLING_UNK|LIVEOBJ3_UPGRADEPART|LIVEOBJ3_CANDAMAGE|LIVEOBJ3_SIMPLEOBJECT|
                            LIVEOBJ3_UNK_10000|LIVEOBJ3_CANDYNAMITE|LIVEOBJ3_UNK_4000|LIVEOBJ3_UNK_2000|
                            LIVEOBJ3_CENTERBLOCKIDLE|LIVEOBJ3_UNUSED_800|LIVEOBJ3_UNK_200|LIVEOBJ3_CANSELECT|
                            LIVEOBJ3_CANYESSIR|LIVEOBJ3_CANPICKUP|LIVEOBJ3_CANCARRY|LIVEOBJ3_CANFIRSTPERSON|
                            LIVEOBJ3_CANTURN|LIVEOBJ3_CANREINFORCE|LIVEOBJ3_CANDIG|LIVEOBJ3_UNK_1);

        return TRUE;
    }
    return FALSE;
}

B32 LegoObject_RouteToDig_FUN_00447100(lpLegoObject liveObj, U32 bx, U32 by, B32 tunnelDig)
{
    // check a bunch of conditions to verify
    // that this is a very ordinary wall
    // if it is a strange wall, return FALSE
    if (!LiveObject_BlockCheck_FUN_004326a0(liveObj, bx, by, tunnelDig, TRUE))
        return FALSE;

    Direction directions[18];

    if (!LegoObject_BlockRoute_FUN_00446c80(liveObj, bx, by, tunnelDig, directions, TRUE))
        return FALSE;

    if (tunnelDig)
    {
        Error_Warn(TRUE, "Tunnel Dig not yet implemented in LegoObject_RouteToDig_FUN_00447100");
        /*directions[1] = -1;
        directions[2] = -1;
        directions[3] = 0;
        directions[4] = -1;
        directions[5] = 0;
        directions[6] = 0;
        directions[7] = -1;
        directions[8] = 0;
        directions[9] = 0;
        directions[10] = 1;
        directions[11] = 1;
        directions[12] = 2;
        directions[13] = 2;
        directions[14] = 3;
        directions[15] = 3;
        directions[16] = 0;
        directions[17] = 0.9f;*/
        // TODO: Implement LegoObject_RouteToDig_FUN_00447100
    }
    else
    {
        LegoObject_RoutingPtr_Realloc_FUN_00446b80(liveObj, bx, by);
        liveObj->routeBlocks[liveObj->routeBlocksTotal - 1].blockOffset.x = Maths_RandRange(0.3f, 0.7f);
        liveObj->routeBlocks[liveObj->routeBlocksTotal - 1].blockOffset.y = Maths_RandRange(0.3f, 0.7f);
        liveObj->targetBlockPos.x = bx;
        liveObj->targetBlockPos.y = by;
        liveObj->flags3 &= (LIVEOBJ3_POWEROFF|LIVEOBJ3_UNK_40000000|LIVEOBJ3_HASPOWER|
                            LIVEOBJ3_CANROUTERUBBLE|LIVEOBJ3_MONSTER_UNK_8000000|LIVEOBJ3_CANGATHER|
                            LIVEOBJ3_UNK_1000000|LIVEOBJ3_REMOVING|LIVEOBJ3_AITASK_UNK_400000|
                            LIVEOBJ3_SELECTED|LIVEOBJ3_ALLOWCULLING_UNK|LIVEOBJ3_UPGRADEPART|
                            LIVEOBJ3_CANDAMAGE|LIVEOBJ3_SIMPLEOBJECT|LIVEOBJ3_UNK_10000|LIVEOBJ3_CANDYNAMITE
                            |LIVEOBJ3_UNK_4000|LIVEOBJ3_UNK_2000|LIVEOBJ3_CENTERBLOCKIDLE|
                            LIVEOBJ3_UNUSED_800|LIVEOBJ3_UNK_400|LIVEOBJ3_UNK_200|LIVEOBJ3_CANSELECT|
                            LIVEOBJ3_CANYESSIR|LIVEOBJ3_CANPICKUP|LIVEOBJ3_CANCARRY|LIVEOBJ3_CANFIRSTPERSON|
                            LIVEOBJ3_CANTURN|LIVEOBJ3_CANREINFORCE|LIVEOBJ3_CANDIG|LIVEOBJ3_UNK_1);
    }

    return TRUE;
}

B32 LegoObject_CheckBlock_FUN_00443b00(lpLegoObject liveObj, Point2I* blockPos, void* data)
{
    B32* pAllowWall = (B32*)data;
    B32 allowWall = *pAllowWall;

    if (!allowWall && Level_Block_IsWall(blockPos->x, blockPos->y))
        return FALSE;

    CrossTerrainType type = Lego_GetCrossTerrainType(liveObj, blockPos->x, blockPos->y, blockPos->x, blockPos->y, TRUE);
    if (type != CrossTerrainType_CantCross &&
        !LiveObject_GetDamageFromSurface(liveObj, blockPos->x, blockPos->y, 0.0f, NULL) &&
        !Level_Block_IsFoundationOrBusyFloor(blockPos))
    {
        return TRUE;
    }

    return FALSE;
}

B32 LegoObject_Route_Score_FUN_004413b0(lpLegoObject liveObj, U32 bx, U32 by, U32 bx2, U32 by2, S32** outNewBXs, S32** outNewBYs, U32* outCount, LegoObject_CheckBlock_Callback callback, void* data)
{
    // TODO: Cleanup this decompiled code

    S32 DIRECTIONS[8];
    DIRECTIONS[0] = 0;
    DIRECTIONS[1] = -1;
    DIRECTIONS[2] = 1;
    DIRECTIONS[3] = 0;
    DIRECTIONS[4] = 0;
    DIRECTIONS[5] = 1;
    DIRECTIONS[6] = -1;
    DIRECTIONS[7] = 0;

    lpMap3D map = Lego_GetMap();
    U32 blockWidth = map->blockWidth;
    U32 blockHeight = map->blockHeight;

    // TODO: double check this is correct interpretation of OR and AND
    if (bx >= blockWidth || by >= blockHeight)
        return FALSE;

    if (callback == NULL)
    {
        if (bx2 >= blockWidth || by2 >= blockHeight)
            return FALSE;

        if (Lego_GetCrossTerrainType(liveObj, bx2, by2, bx2, by2, FALSE) == CrossTerrainType_CantCross)
            return FALSE;
    }

    if (objectGlobs.UnkSurfaceGrid_1_TABLE != NULL)
    {
        if (objectGlobs.UnkSurfaceGrid_COUNT == blockWidth * blockHeight)
            goto someLabel;

        //if (objectGlobs.UnkSurfaceGrid_1_TABLE != NULL)
        //{
            Mem_Free(objectGlobs.UnkSurfaceGrid_1_TABLE);
        //}
    }

    objectGlobs.UnkSurfaceGrid_COUNT = blockWidth * blockHeight;
    objectGlobs.UnkSurfaceGrid_1_TABLE = Mem_Alloc(objectGlobs.UnkSurfaceGrid_COUNT * sizeof(F32));

someLabel:
    if (objectGlobs.UnkSurfaceGrid_1_TABLE != NULL)
    {
        B32 thereWasNoCallback = FALSE;
        S32 status = 0;
        F64 weightDifference = 0;
        memset(objectGlobs.UnkSurfaceGrid_1_TABLE, 0, objectGlobs.UnkSurfaceGrid_COUNT * sizeof(F32));

        S32 count = 1;

        U32 coords[2];
        coords[0] = 1;
        coords[1] = 0;

        U32* pCoords = coords;

        B32 currentlyXorYLoop = 0;

        objectGlobs.UnkSurfaceGrid_1_TABLE[(by * blockWidth) + bx] = 1.0f;

        S32 blocksXY[1000];
        blocksXY[0] = bx;
        blocksXY[1] = by;

        Point2I tmpPoint;

        do
        {
            count++;

            U32 i = 0;
            if (pCoords[0] != 0)
            {
                S32* currentBlockXY = &blocksXY[currentlyXorYLoop * 500];
                do
                {
                    // TODO: Is this correct?
                    //F32 distanceAway = *(F32 *)((S32)objectGlobs.UnkSurfaceGrid_1_TABLE + (blockWidth * *newBXs + newBXs[-1]) * 4);
                    F32 distanceAway = objectGlobs.UnkSurfaceGrid_1_TABLE[(blockWidth * currentBlockXY[1]) + currentBlockXY[0]];

                    S32 direction = 0;
                    do
                    {
                        U32 evaluationX = currentBlockXY[0];
                        U32 evaluationY = currentBlockXY[1];

                        if (direction == 0)
                            evaluationY--;
                        else if (direction == 1)
                            evaluationX++;
                        else if (direction == 2)
                            evaluationY++;
                        else if (direction == 3)
                            evaluationX--;

                        if (evaluationX < blockWidth && evaluationY < blockHeight)
                        {
                            F32 howMuchIWantToAvoid = 1.0f;
                            U32 absoluteX = DIRECTIONS[direction * 2] + evaluationX;
                            U32 absoluteY = DIRECTIONS[(direction * 2) + 1] + evaluationY;

                            tmpPoint.x = absoluteX;
                            tmpPoint.y = absoluteY;

                            if (Level_Block_IsPath(&tmpPoint))
                                howMuchIWantToAvoid = 0.5f;

                            CrossTerrainType type = Lego_GetCrossTerrainType(liveObj, evaluationX, evaluationY, absoluteX, absoluteY, FALSE);

                            // in Driller Night, these blocks should be crossable
                            // this hack is to make them crossable
                            // even though GetCrossTerrainType currently returns wrong value
                            /*
                            if ((evaluationX == 14 && evaluationY == 15)
                                || (evaluationX == 10 && evaluationY == 18)) {
                                // syke!
                                type = CrossTerrainType_CanCross;
                            }
                            */

                            // TODO: WTF is this if statement?
                            if (type != CrossTerrainType_CantCross)
                            {
                                F32* weight = &objectGlobs.UnkSurfaceGrid_1_TABLE[(evaluationY * blockWidth) + evaluationX];

                                if (*weight == 0.0f || (*weight > howMuchIWantToAvoid + distanceAway))
                                {
                                    U32 notCurrentlyXorYLoop = (U32)(!currentlyXorYLoop);
                                    *weight = howMuchIWantToAvoid + distanceAway;
                                    U32 coordinate = coords[notCurrentlyXorYLoop];
                                    U32 index = 5 * (coordinate + 100 * notCurrentlyXorYLoop);
                                    blocksXY[index] = evaluationX;
                                    blocksXY[index + 1] = evaluationY;
                                    coords[notCurrentlyXorYLoop]++;

                                    if (callback == NULL)
                                    {
                                        if (evaluationX == bx2 && evaluationY == by2)
                                        {
                                            thereWasNoCallback = TRUE;
                                            weightDifference = (F64)((F32)count);
                                        }
                                    }
                                    else
                                    {
                                        tmpPoint.x = evaluationX;
                                        tmpPoint.y = evaluationY;
                                        if (callback(liveObj, &tmpPoint, data))
                                        {
                                            status = 1;
                                            bx2 = evaluationX;
                                            by2 = evaluationY;
                                        }
                                    }
                                }
                            }
                        }

                        // TODO: confirm
                        direction++;
                    } while (direction < 4);

                    i++;
                    currentBlockXY += 5;
                } while (i < pCoords[0]);
            }

            pCoords[0] = 0;
            currentlyXorYLoop = !currentlyXorYLoop;
            pCoords = coords + currentlyXorYLoop;
            if (!coords[currentlyXorYLoop])
            {
                status = -1;
            }

        } while (!status);

        if (thereWasNoCallback)
        {
            status = 1;
            count = weightDifference;
        }

        if (status == -1)
            return FALSE;

        U32* newBXs = Mem_Alloc(count * sizeof(U32));
        U32* newBYs;
        if (newBXs != NULL && (newBYs = Mem_Alloc(count * sizeof(U32)), newBYs != NULL))
        {
            U32 x = bx2;
            U32 y = by2;
            S32 countMinusOne = count - 1;
            newBXs[countMinusOne] = bx2;
            newBYs[countMinusOne] = by2;
            U32* lastNewBX = &newBXs[countMinusOne];
            U32* lastNewBY = &newBYs[countMinusOne];
            *newBXs = bx;
            *newBYs = by;

            if (countMinusOne != 1)
            {
                U32 i = count - 2;
                do
                {
                    F32 startsWith10000 = 10000.0f;
                    S32 direction2 = 0;

                    // loop four times, one for each direction
                    do
                    {
                        U32 offsetX = x;
                        U32 offsetY = y;

                        if (direction2 == 0)
                            offsetY = y - 1;
                        else if (direction2 == 1)
                            offsetX = x + 1;
                        else if (direction2 == 2)
                            offsetY = y + 1;
                        else if (direction2 == 3)
                            offsetX = x - 1;

                        tmpPoint.x = offsetX;
                        tmpPoint.y = offsetY;

                        // only if the point is within the map
                        if (offsetX < blockWidth && offsetY < blockHeight) {
                            weightDifference = (F64)objectGlobs.UnkSurfaceGrid_1_TABLE[(y * blockWidth) + x]
                                             - (F64)objectGlobs.UnkSurfaceGrid_1_TABLE[(offsetY * blockWidth) + offsetX];
                            
                            if ((weightDifference > 0.0f && weightDifference < startsWith10000)
                                || (weightDifference == startsWith10000 && Level_Block_IsPath(&tmpPoint)))
                            {
                                startsWith10000 = weightDifference;

                                coords[0] = offsetX;
                                coords[1] = offsetY;
                            }
                        }

                        direction2++;
                    } while (direction2 < 4);

                    lastNewBX--;
                    lastNewBY--;
                    x = coords[0];
                    y = coords[1];
                    *lastNewBX = x;
                    *lastNewBY = y;
                    i--;
                } while (i);
            }

            *outNewBXs = (S32*)newBXs;
            *outNewBYs = (S32*)newBYs;
            *outCount = count;

            if ((newBXs[0] != newBXs[1] ||
                abs(newBYs[0] - newBYs[1]) != 1) &&
                (newBYs[0] != newBYs[1] ||
                abs(newBXs[0] - newBXs[1]) != 1))
            {
                Mem_Free(newBXs);
                Mem_Free(newBYs);

                return LegoObject_Route_ScoreSub_FUN_00440f30(liveObj, bx, by, bx2, by2, (U32**) outNewBXs, (U32**) outNewBYs, (U32*) outCount, callback, data);
            }
        }
    }

    return TRUE;
}

B32 LegoObject_Route_ScoreSub_FUN_00440f30(lpLegoObject liveObj, U32 bx, U32 by, U32 bx2, U32 by2, U32** outNewBXs, U32** outNewBYs, U32* outCount, LegoObject_CheckBlock_Callback callback, void* data)
{
    return FALSE;
}

B32 LegoObject_Route_ScoreNoCallback_FUN_00440ef0(lpLegoObject liveObj, U32 bx, U32 by, U32 bx2, U32 by2, S32** outNewBXs, S32** outNewBYs, U32* outCount)
{
    return LegoObject_Route_Score_FUN_004413b0(liveObj, bx, by, bx2, by2, outNewBXs, outNewBYs, outCount, NULL, NULL);
}

B32 LegoObject_FUN_004439d0(lpLegoObject liveObj, Point2I* blockPos, Point2I* outBlockPos, U32 unused)
{
    B32 allowWall = FALSE;

    if (LegoObject_CheckBlock_FUN_00443b00(liveObj, blockPos, &allowWall))
        return FALSE;

    S32* newBXs;
    S32* newBYs;
    S32 newCount;
    // So calling this function a second time with the same arguments seems to have
    // a different result. Most likely because of the utilized global variables
    // within.
    // Note that none of the out variables are read during the function call, only
    // written.
    if (!LegoObject_Route_Score_FUN_004413b0(liveObj, blockPos->x, blockPos->y, 0, 0, &newBXs, &newBYs, &newCount, LegoObject_CheckBlock_FUN_00443b00, &allowWall) &&
        !LegoObject_Route_Score_FUN_004413b0(liveObj, blockPos->x, blockPos->y, 0, 0, &newBXs, &newBYs, &newCount, LegoObject_CheckBlock_FUN_00443b00, &allowWall))
    {
        return FALSE;
    }

    // TODO: Verify that this is correct
    outBlockPos->x = newBXs[newCount * 4 - 4];
    outBlockPos->y = newBYs[newCount * 4 - 4];

    Mem_Free(newBXs);
    Mem_Free(newBYs);
    return TRUE;
}

B32 LegoObject_Check_LotsOfFlags1AndFlags2_FUN_0043bdb0(lpLegoObject liveObj)
{
    if ((liveObj->flags1 & (LIVEOBJ1_UNUSED_10000000|LIVEOBJ1_TELEPORTINGUP|LIVEOBJ1_TELEPORTINGDOWN|LIVEOBJ1_CRUMBLING|
       LIVEOBJ1_PLACING|LIVEOBJ1_CLEARING|LIVEOBJ1_STORING|LIVEOBJ1_GETTINGHIT|
       LIVEOBJ1_GATHERINGROCK|LIVEOBJ1_EXPANDING|LIVEOBJ1_TURNRIGHT|LIVEOBJ1_REINFORCING|
       LIVEOBJ1_DRILLINGSTART|LIVEOBJ1_DRILLING|LIVEOBJ1_TURNING|LIVEOBJ1_LIFTING)) != LIVEOBJ1_NONE)
    {
        return TRUE;
    }

    return (liveObj->flags2 & (LIVEOBJ2_FIRINGFREEZER|LIVEOBJ2_FIRINGPUSHER|LIVEOBJ2_FIRINGLASER|
                 LIVEOBJ2_BUILDPATH|LIVEOBJ2_THROWN|LIVEOBJ2_THROWING)) != LIVEOBJ2_NONE;
}

B32 LegoObject_TryWaiting(lpLegoObject liveObj)
{
    if ((liveObj->flags1 & LIVEOBJ1_CARRYING) != LIVEOBJ1_NONE)
        return FALSE;

    if (LegoObject_Check_LotsOfFlags1AndFlags2_FUN_0043bdb0(liveObj))
        return FALSE;

    // 5 Attempts to pick a valid waiting animation
    U32 attempt = 0;
    do
    {
        U32 repeatCount = 1;
        S16 rng = Maths_Rand();

        // there are 21 different waiting activities, they are all in a row
        // starting from Activity_Waiting1
        // so we can just pick a random number between 0 and 20 and add it to Activity_Waiting1
        Activity_Type activity = (S32)rng % 21 + Activity_Waiting1;
        LegoObject_SetActivity(liveObj, activity, repeatCount);
        if (LegoObject_UpdateActivityChange(liveObj))
        {
            liveObj->flags1 |= LIVEOBJ1_WAITING;
            return TRUE;
        }
        attempt++;
    } while (attempt < 5);

    return FALSE;
}

// Seems to be completely identical to LiveObject_MiniFigureHasBeamEquipped @ 004496a0
B32 LegoObject_MiniFigureHasBeamEquipped2(lpLegoObject liveObj)
{
    // TODO: Implement LegoObject_MiniFigureHasBeamEquipped2

    return FALSE;
}

B32 LiveObject_BlockCheck_FUN_004326a0(lpLegoObject liveObj, U32 bx, U32 by, B32 param4, B32 param5)
{
    Lego_Level* level = legoGlobs.currLevel;
    Lego_Block* curBlock = &level->blocks[by * level->width + bx];
    if (((StatsObject_GetStatsFlags1(liveObj) & STATS1_CANBEDRIVEN) != STATS1_NONE && liveObj->driveObject == NULL))
        return FALSE;

    if (param4 && ((StatsObject_GetStatsFlags1(liveObj) & STATS1_SINGLEWIDTHDIG) == STATS1_NONE))
        return FALSE;

    if ((!param5 || (liveObj->flags1 & LIVEOBJ1_CARRYING) == LIVEOBJ1_NONE) &&
        (liveObj->flags3 & LIVEOBJ3_CANDIG) != LIVEOBJ3_NONE &&
        StatsObject_GetDrillTimeType(liveObj, (U32)curBlock->terrain) != 0.0f &&
            bx < level->width - 2 && by < level->height - 2)
    {
        if (!param4)
        {
            if ((curBlock->flags1 & BLOCK1_WALL) != BLOCK1_NONE &&
                (curBlock->flags1 & BLOCK1_INCORNER) == BLOCK1_NONE &&
                curBlock->terrain != Lego_SurfaceType_Immovable &&
                curBlock->terrain != Lego_SurfaceType_Water &&
                curBlock->terrain != Lego_SurfaceType_RechargeSeam)
            {
                // this is a very ordinary wall
                return TRUE;
            }
        }
        else if (curBlock->predug == Lego_PredugType_Wall)
        {
            // TODO: Implement LiveObject_BlockCheck_FUN_004326a0
        }
    }

    return FALSE;
}

B32 LegoObject_TaskHasTool_FUN_0044b780(lpLegoObject liveObj, AITask_Type taskType)
{
    // TODO: Implement LegoObject_TaskHasTool_FUN_0044b780

    return TRUE;
}

// Update energy drain while carrying and attempt to rest when needed
void LegoObject_UpdateCarryingEnergy(lpLegoObject liveObj, F32 elapsed)
{
    // TODO: Implement LegoObject_UpdateCarryingEnergy
}

void LegoObject_RockMonster_FUN_0043ad70(lpLegoObject liveObj)
{
    // TODO: Implement LegoObject_RockMonster_FUN_0043ad70
}

B32 LegoObject_VehicleMaxCarryChecksTime_FUN_00439c80(lpLegoObject liveObj)
{
    // TODO: Implement LegoObject_VehicleMaxCarryChecksTime_FUN_00439c80

    return FALSE;
}

B32 LegoObject_GetBlockPos(lpLegoObject liveObj, S32* outBx, S32* outBy)
{
    lpContainer cont = LegoObject_GetActivityContainer(liveObj);

    Point3F position;
    Container_GetPosition(cont, NULL, &position);

    return Map3D_WorldToBlockPos_NoZ(Lego_GetMap(), position.x, position.y, outBx, outBy);
}

void LegoObject_GetPosition(lpLegoObject liveObj, F32* outX, F32* outY)
{
    lpContainer cont = LegoObject_GetActivityContainer(liveObj);

    Point3F position;
    Container_GetPosition(cont, NULL, &position);

    *outX = position.x;
    *outY = position.y;
}

void LegoObject_GetFaceDirection(lpLegoObject liveObj, Point2F* outDirection)
{
    lpContainer cont = LegoObject_GetActivityContainer(liveObj);

    Point3F vecDir;
    Container_GetOrientation(cont, NULL, &vecDir, NULL);

    F32 norm = 1.0f / sqrtf(vecDir.x * vecDir.x + vecDir.y * vecDir.y);

    outDirection->x = norm * vecDir.x;
    outDirection->y = norm * vecDir.y;
}

B32 LiveObject_GetDamageFromSurface(lpLegoObject liveObj, S32 bx, S32 by, F32 elapsedGame, F32* optOutDamage)
{
    // TODO: Implement LiveObject_GetDamageFromSurface

    return FALSE;
}

B32 LiveObject_FUN_00431ba0(lpLegoObject liveObj, Point2I* blockPos, Point2I* outBlockOffPos, B32 param4)
{
    Point2I DIRECTIONS[4];
    DIRECTIONS[0].x = 0;
    DIRECTIONS[0].y = -1;
    DIRECTIONS[1].x = 1;
    DIRECTIONS[1].y = 0;
    DIRECTIONS[2].x = 0;
    DIRECTIONS[2].y = 1;
    DIRECTIONS[3].x = -1;
    DIRECTIONS[3].y = 0;

    gamectrlGlobs.direction_178 += DIRECTION_RIGHT;

    U32 i = 0;
    Lego_Level* level = legoGlobs.currLevel;
    do
    {
        Direction dir = gamectrlGlobs.direction_178 + i & 3;
        if (!param4)
        {
somelbl:
            S32 bx1 = blockPos->x + DIRECTIONS[dir].x;
            S32 by1 = blockPos->y + DIRECTIONS[dir].y;
            CrossTerrainType crossType = Lego_GetCrossTerrainType(liveObj, bx1, by1, bx1, by1, FALSE);
            if (crossType != CrossTerrainType_CantCross)
            {
                outBlockOffPos->x = DIRECTIONS[dir].x + blockPos->x;
                outBlockOffPos->y = DIRECTIONS[dir].y + blockPos->y;
                return TRUE;
            }

        }
        else
        {
            B32 bVar1;

            // TODO: Should these be S32?
            U32 theX = DIRECTIONS[dir].x + blockPos->x;
            U32 theY = DIRECTIONS[dir].y + blockPos->y;

            BlockFlags1 bflags1;
            if (theX < level->width - 1 && theY < level->height - 1 &&
                (bflags1 = level->blocks[theY * level->width + theX].flags1,
                (bflags1 & BLOCK1_WALL) != BLOCK1_NONE && (bflags1 & BLOCK1_FLOOR) == BLOCK1_NONE))
            {
                bVar1 = TRUE;
            }
            else
            {
                bVar1 = FALSE;
            }

            if (!bVar1)
                goto somelbl;
        }

        i++;
        if (i > 3)
            return FALSE;
    } while (TRUE);
}

B32 LiveObject_FUN_00433b40(lpLegoObject liveObj, F32 param2, B32 param3)
{
    // TODO: Implement LiveObject_FUN_00433b40

    return FALSE;
}

B32 LegoObject_GetDrillNullPosition(lpLegoObject liveObj, F32 *outXPos, F32* outYPos)
{
    lpContainer cont = NULL;

    LegoObject_Type type = liveObj->type;
    if (type == LegoObject_Vehicle)
    {
        cont = Vehicle_GetDrillNull(liveObj->vehicle);
    }
    else if (type == LegoObject_MiniFigure)
    {
        cont = Creature_GetDrillNull(liveObj->miniFigure);
    }
    else if (type == LegoObject_RockMonster)
    {
        cont = Creature_GetDrillNull(liveObj->rockMonster);
    }

    if (cont == NULL)
        return FALSE;

    Point3F pos;
    Container_GetPosition(cont, NULL, &pos);

    *outXPos = pos.x;
    *outYPos = pos.y;
    return TRUE;
}

#ifdef LEGORR_DEBUG_SHOW_INFO
const char* LegoObject_Debug_GetTypeName(LegoObject_Type objType)
{
    switch (objType)
    {
        case LegoObject_None:
            return "None";
        case LegoObject_Vehicle:
            return "Vehicle";
        case LegoObject_MiniFigure:
            return "MiniFigure";
        case LegoObject_RockMonster:
            return "RockMonster";
        case LegoObject_Building:
            return "Building";
        case LegoObject_Boulder:
            return "Boulder";
        case LegoObject_PowerCrystal:
            return "PowerCrystal";
        case LegoObject_Ore:
            return "Ore";
        case LegoObject_Dynamite:
            return "Dynamite";
        case LegoObject_Barrier:
            return "Barrier";
        case LegoObject_UpgradePart:
            return "UpgradePart";
        case LegoObject_ElectricFence:
            return "ElectricFence";
        case LegoObject_SpiderWeb:
            return "SpiderWeb";
        case LegoObject_OohScary:
            return "OohScary";
        case LegoObject_ElectricFenceStud:
            return "ElectricFenceStud";
        case LegoObject_Path:
            return "Path";
        case LegoObject_Pusher:
            return "Pusher";
        case LegoObject_Freezer:
            return "Freezer";
        case LegoObject_IceCube:
            return "IceCube";
        case LegoObject_LaserShot:
            return "LaserShot";
        case LegoObject_Type_Count:
            return "Type_Count";
        case LegoObject_TVCamera:
            return "TVCamera";
        default:
            return "Unknown";
    }
}
#endif
