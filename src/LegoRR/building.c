#include "building.h"

#include "maths.h"

B32 Building_Load(lpBuildingModel building, LegoObject_ID objID, lpContainer root, const char* filename, const char* gameName)
{
    // TODO: Implement Building_Load

    return TRUE;
}

void Building_Hide(lpBuildingModel building, B32 hide)
{
    // TODO: Implement Building_Hide
}

Point2I* Building_GetShapePoints(BuildingModel* building, U32* optoutShapeCount)
{
    if (optoutShapeCount != NULL)
        *optoutShapeCount = building->shapeCount;

    return building->shapePoints;
}

F32 Building_MoveAnimation(lpBuildingModel building, F32 elapsed, U32 unkFrameNo)
{
    // TODO: Implement Building_MoveAnimation

    return 0.0f;
}
