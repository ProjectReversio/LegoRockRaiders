#pragma once

#include "platform.h"
#include "lego_object.h"

typedef struct VehicleModel
{
    LegoObject_ID objID;
    const char* wheelNullName; // (ae: WheelNullName)
    lpContainer contAct1; // (ACT, true)
    lpContainer contAct2; // (ACT, true) Optional second ae file (seen for Granite Grinder legs) This container has priority for finding null frames

    // TODO: Implement VehicleModel

} VehicleModel, *lpVehicleModel;

extern B32 Vehicle_Load(lpVehicleModel vehicle, LegoObject_ID objID, lpContainer root, const char* filename, const char* gameName);

extern void Vehicle_Hide(lpVehicleModel vehicle, B32 hide);

extern lpContainer Vehicle_GetActivityContainer(lpVehicleModel vehicle);
