#pragma once

#include "platform.h"
#include "lego_object.h"

typedef struct VehicleModel
{
    void* temp;
    // TODO: Implement VehicleModel
} VehicleModel, *lpVehicleModel;

extern B32 Vehicle_Load(lpVehicleModel vehicle, LegoObject_ID objID, lpContainer root, const char* filename, const char* gameName);

extern void Vehicle_Hide(lpVehicleModel vehicle, B32 hide);
