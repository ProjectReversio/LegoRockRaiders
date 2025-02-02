#include "lego_object.h"

#include <stdio.h>

#include "aitask.h"
#include "creature.h"
#include "dummy.h"
#include "lego.h"
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
    // TODO: Implement LegoObject_RunThroughListsSkipUpgradeParts
    return FALSE;
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
