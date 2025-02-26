#include "lego_object.h"

#include <stdio.h>

#include "aitask.h"
#include "creature.h"
#include "dummy.h"
#include "lego.h"
#include "level.h"
#include "mem.h"
#include "sfx.h"

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

// The same as `LegoObject_GetWorldZCallback`, but returns a lower Z value with over Lake terrain.
// Objects wading in a lake (aka, not sailing) will have their Z lowered a bit, and have it at the lowest near the center of a lake BLOCK.
F32 LegoObject_GetWorldZCallback_Lake(F32 xPos, F32 yPos, struct Map3D* map)
{
    Point2I blockPos;

    F32 zModifier = 0.0f;
    if (Map3D_WorldToBlockPos(map, xPos, yPos, &blockPos.x, &blockPos.y, &zModifier) != 0)
    {
        if (legoGlobs.currLevel->blocks[(legoGlobs.currLevel->width * blockPos.y + blockPos.x)].terrain == Lego_SurfaceType8_Lake)
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

void LegoObject_Route_End(lpLegoObject liveObj, B32 completed)
{
    // TODO: Implement LegoObject_Route_End
}

void LegoObject_DropCarriedObject(lpLegoObject liveObj, B32 putAway)
{
    // TODO: Implement LegoObject_DropCarriedObject
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
    // TODO: Implement LegoObject_Route_UpdateMovement
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

    U32 OFFSETS[10];
    OFFSETS[0] = 0;
    OFFSETS[1] = 0xffffffff;
    OFFSETS[2] = 1;
    OFFSETS[3] = 0;
    OFFSETS[4] = 0;
    OFFSETS[5] = 1;
    OFFSETS[6] = 0xffffffff;
    OFFSETS[7] = 0;

    lpMap3D map = Lego_GetMap();
    U32 blockWidth = map->blockWidth;
    U32 blockHeight = map->blockHeight;

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

        if (objectGlobs.UnkSurfaceGrid_1_TABLE != NULL)
        {
            Mem_Free(objectGlobs.UnkSurfaceGrid_1_TABLE);
        }
    }

    objectGlobs.UnkSurfaceGrid_COUNT = blockWidth * blockHeight;
    objectGlobs.UnkSurfaceGrid_1_TABLE = Mem_Alloc(objectGlobs.UnkSurfaceGrid_COUNT * sizeof(F32));

someLabel:
    if (objectGlobs.UnkSurfaceGrid_1_TABLE != NULL)
    {
        B32 someBool = FALSE;
        S32 status = 0;
        S32 theCount;
        memset(objectGlobs.UnkSurfaceGrid_1_TABLE, 0, blockWidth * blockHeight * sizeof(F32));

        S32 count = 1;

        U32 coords[2];
        coords[0] = 1;

        U32* pCoords = coords;

        U32 specialNum = 0;

        objectGlobs.UnkSurfaceGrid_1_TABLE[by * blockWidth + bx] = 1.0f;

        OFFSETS[8] = bx;
        OFFSETS[9] = by;
        coords[1] = 0;

        do
        {
            count++;

            U32 iterator2 = 0;
            if (pCoords[0] != 0)
            {
                U32* dataIterator = OFFSETS + specialNum * 500 + 9;
                do
                {
                    // TODO: Is this correct?
                    //F32 fVar2 = *(F32 *)((S32)objectGlobs.UnkSurfaceGrid_1_TABLE + (blockWidth * *puVar7 + puVar7[-1]) * 4);
                    F32 fVar2 = objectGlobs.UnkSurfaceGrid_1_TABLE[blockWidth * *dataIterator + dataIterator[-1]];

                    S32 iterator3 = 0;
                    do
                    {
                        U32 theX = dataIterator[-1];
                        U32 theY = *dataIterator;

                        if (iterator3 == 0)
                            theY--;
                        else if (iterator3 == 1)
                            theX++;
                        else if (iterator3 == 2)
                            theY++;
                        else if (iterator3 == 3)
                            theX--;

                        if (theX < blockWidth && theY < blockHeight)
                        {
                            F32 someValue = 1.0f;
                            U32 xOffset = OFFSETS[iterator3 * 2];
                            U32 yOffset = OFFSETS[iterator3 * 2 + 1];
                            Point2I PStack_fc8;
                            PStack_fc8.x = xOffset + theX;
                            PStack_fc8.y = yOffset + theY;

                            if (Level_Block_IsPath(&PStack_fc8))
                                someValue = 0.5f;

                            CrossTerrainType type = Lego_GetCrossTerrainType(liveObj, theX, theY, xOffset + theX, yOffset + theY, FALSE);
                            // TODO: WTF is this if statement?
                            if (type != CrossTerrainType_CantCross &&
                                objectGlobs.UnkSurfaceGrid_1_TABLE[theY * blockWidth + theX] == 0.0f ||
                                (someValue + fVar2 < objectGlobs.UnkSurfaceGrid_1_TABLE[theY * blockWidth + theX]))
                            {
                                yOffset = (U32)(specialNum == 0);
                                xOffset = coords[yOffset];
                                objectGlobs.UnkSurfaceGrid_1_TABLE[theY * blockWidth + theX] = someValue + fVar2;
                                OFFSETS[(xOffset + yOffset * 100) * 5 + 8] = theX;
                                OFFSETS[(xOffset + yOffset * 100) * 5 + 9] = theY;
                                coords[yOffset] = xOffset + 1;

                                if (callback == NULL)
                                {
                                    if (theX == bx2 && theY == by2)
                                    {
                                        someBool = TRUE;
                                        theCount = count;
                                    }
                                }
                                else
                                {
                                    Point2I PStack_fd0;
                                    PStack_fd0.x = theX;
                                    PStack_fd0.y = theY;
                                    if (callback(liveObj, &PStack_fd0, data))
                                    {
                                        status = 1;
                                        bx2 = theX;
                                        by2 = theY;
                                    }
                                }
                            }
                        }

                        iterator3++;
                    } while (iterator3 < 4);

                    iterator2++;
                    dataIterator += 5;
                } while (iterator2 < pCoords[0]);
            }

            pCoords[0] = 0;
            specialNum = (U32)(specialNum == 0);
            pCoords = coords + specialNum;
            if (coords[specialNum] == 0)
            {
                status = -1;
            }

        } while (status == 0);

        if (someBool)
        {
            status = 1;
            count = theCount;
        }

        if (status == -1)
            return FALSE;

        U32* puVar7 = Mem_Alloc(count * sizeof(U32));
        U32* buffer;
        if (puVar7 != NULL && (buffer = Mem_Alloc(count * sizeof(U32)), buffer != NULL))
        {
            S32 iVar4 = count - 1;
            puVar7[iVar4] = bx2;
            U32* puStack_ff0 = buffer + iVar4;
            *puStack_ff0 = by2;
            *puVar7 = bx;
            *buffer = by;

            if (iVar4 != 1)
            {
                U32 iterator2 = count - 2;
                U32 theX = bx2;
                U32 theY = by2;
                do
                {
                    F32 fStack_1008 = 10000.0f;
                    S32 iterator3 = 0;
                    do
                    {
                        U32 uVar9 = theX;
                        U32 uVar6;
                        if (iterator3 == 0)
                        {
                            uVar6 = theY - 1;
                        }
                        else
                        {
                            uVar6 = theY;
                            if (iterator3 == 1)
                                uVar9 = theX + 1;
                            else if (iterator3 == 2)
                                uVar6 = theY + 1;
                            else if (iterator3 == 3)
                                uVar9 = theX - 1;
                        }

                        Point2I PStack_fd0;
                        PStack_fd0.x = uVar9;
                        PStack_fd0.y = uVar6;

                        if ((uVar9 < blockWidth && uVar6 < blockHeight) &&
                            (theCount = objectGlobs.UnkSurfaceGrid_1_TABLE[theY * blockWidth + theX] - objectGlobs.UnkSurfaceGrid_1_TABLE[uVar6 * blockWidth + uVar9],
                            theCount > 0.0f && (F32)theCount < fStack_1008) ||
                            ((F32)theCount == fStack_1008 && Level_Block_IsPath(&PStack_fd0)))
                        {
                            fStack_1008 = theCount;
                            coords[0] = uVar9;
                            coords[1] = uVar6;
                        }

                        iterator3++;
                    } while (iterator3 < 4);

                    puStack_ff0--;
                    *(U32*)(((S32)puVar7 - (S32)buffer) + (S32)puStack_ff0) = coords[0];
                    *puStack_ff0 = coords[1];
                    iterator2--;
                    theX = coords[0];
                    theY = coords[1];
                } while (iterator2 != 0);
            }

            *outNewBXs = (S32*)puVar7;
            *outNewBYs = (S32*)buffer;
            *outCount = count;

            U32 uVar8;
            U32 uVar11;
            if ((*puVar7 != puVar7[1] ||
                (uVar8 = (S32)(*buffer - buffer[1]) >> 0x1f, (*buffer - buffer[1] ^ uVar8) - uVar8 != 1)) &&
                ((*buffer != buffer[1] ||
                    (uVar8 = *puVar7 - puVar7[1], uVar11 = (S32)uVar8 >> 0x1f,
                    (uVar8 ^ uVar11) - uVar11 != 1))))
            {
                Mem_Free(puVar7);
                Mem_Free(buffer);

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

B32 LiveObject_GetDamageFromSurface(lpLegoObject liveObj, S32 bx, S32 by, F32 elapsedGame, F32* optOutDamage)
{
    // TODO: Implement LiveObject_GetDamageFromSurface

    return FALSE;
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
