#include "lego_object.h"

#include <stdio.h>

#include "creature.h"
#include "dummy.h"
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
    // TODO: Implement LegoObject_SetPositionAndHeading
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

        // TODO: Implement LegoObject_Create

        if (((CreatureModel*)objModel)->drillNullName != NULL)
        {
            // TODO: Implement LegoObject_Create
        }
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

    // TODO: Implement LegoObject_Create

    Container_EnableSoundTriggers(TRUE);
    if (soundOn)
        SFX_SetSoundOn(TRUE, FALSE);

    // TODO: Implement LegoObject_Create

    return liveObj;
}

B32 LegoObject_Remove(lpLegoObject liveObj)
{
    // TODO: Implement LegoObject_Remove
    return FALSE;
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
