#include "lego_object.h"

LegoObject_Globs objectGlobs = { NULL };

void LegoObject_Initialize()
{
    // TODO: Implement LegoObject_Initialize
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

lpLegoObject LegoObject_Create(void** objModel, LegoObject_Type objType, LegoObject_ID objID)
{
    // TODO: Implement LegoObject_Create
    return NULL;
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
    // TODO: Implement LegoObject_LoadMeshLOD
    return NULL;
}
