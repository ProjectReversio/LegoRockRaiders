#include "vehicle.h"

B32 Vehicle_Load(lpVehicleModel vehicle, LegoObject_ID objID, lpContainer root, const char* filename, const char* gameName)
{
    // TODO: Implement Vehicle_Load

    return TRUE;
}

void Vehicle_Hide(lpVehicleModel vehicle, B32 hide)
{
    // TODO: Implement Vehicle_Hide
}

lpContainer Vehicle_GetActivityContainer(lpVehicleModel vehicle)
{
    return vehicle->contAct1;
}

F32 Vehicle_MoveAnimation(lpVehicleModel vehicle, F32 elapsed1, F32 elapsed2, U32 unkFrameNo)
{
    // TODO: Implement Vehicle_MoveAnimation

    return 0.0f;
}

void Vehicle_SetOrientation(lpVehicleModel vehicle, F32 xDir, F32 yDir, F32 zDir)
{
    Container_SetOrientation(vehicle->contAct1, NULL, xDir, yDir, zDir, 0.0f, 0.0f, -1.0f);
}

void Vehicle_SetPosition(lpVehicleModel vehicle, F32 xPos, F32 yPos, GetWorldZCallback zCallback, struct Map3D* map)
{
    // TODO: Implement Vehicle_SetPosition
}

lpContainer Vehicle_GetDrillNull(lpVehicleModel vehicle)
{
    // TODO: Implement Vehicle_GetDrillNull
    return NULL;
}
