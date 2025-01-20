#pragma once

#include "platform.h"
#include "maths.h"
#include "lego_object.h"

typedef struct BuildingModel
{
    LegoObject_ID objID;
    lpContainer contAct; // (ACT, true)

    // TODO: Implement BuildingModel

    Point2I* shapePoints; // (ae: Shape) Point2I[10]
    U32 shapeCount; // (ae: Shape)

    // TODO: Implement BuildingModel
} BuildingModel, *lpBuildingModel;

extern B32 Building_Load(lpBuildingModel building, LegoObject_ID objID, lpContainer root, const char* filename, const char* gameName);

extern void Building_Hide(lpBuildingModel building, B32 hide);

extern Point2I* Building_GetShapePoints(BuildingModel* building, U32* optoutShapeCount);

