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
