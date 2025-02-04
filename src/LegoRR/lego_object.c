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
    // TODO: Implement LegoObject_Initialize
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

    liveObj->activityName2 = objectGlobs.activityName[0];
    liveObj->activityName1 = objectGlobs.activityName[9];

    if (LegoObject_UpdateActivityChange(liveObj))
        liveObj->flags3 |= LIVEOBJ3_CANTURN;

    liveObj->activityName1 = objectGlobs.activityName[16];
    if (LegoObject_UpdateActivityChange(liveObj))
        liveObj->flags3 |= LIVEOBJ3_CANGATHER;

    liveObj->activityName1 = objectGlobs.activityName[2];
    if (LegoObject_UpdateActivityChange(liveObj))
        liveObj->flags3 |= LIVEOBJ3_CANROUTERUBBLE;

    liveObj->activityName1 = objectGlobs.activityName[0];
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
    // TODO: Implement LegoObject_UpdateActivityChange

    return FALSE;
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
    // TODO: Implement LegoObject_MoveAnimation

    return elapsed;
}

B32 LegoObject_TryDepart_FUN_004499c0(lpLegoObject liveObj)
{
    // TODO: Implement LegoObject_TryDepart_FUN_004499c0

    return FALSE;
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

// Update energy drain while carrying and attempt to rest when needed
void LegoObject_UpdateCarryingEnergy(lpLegoObject liveObj, F32 elapsed)
{
    // TODO: Implement LegoObject_UpdateCarryingEnergy
}

void LegoObject_RockMonster_FUN_0043ad70(lpLegoObject liveObj)
{
    // TODO: Implement LegoObject_RockMonster_FUN_0043ad70
}
