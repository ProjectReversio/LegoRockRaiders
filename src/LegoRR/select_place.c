#include "select_place.h"

lpSelectPlace SelectPlace_Create(lpContainer root, F32 tileDepth)
{
    // TODO: Implement SelectPlace_Create
    return NULL;
}

void SelectPlace_Hide(lpSelectPlace selectPlace, B32 hide)
{
    // TODO: Implement SelectPlace_Hide
}

// Translate and rotate a set of integer points (max of 20).
// Returns a pointer to a temporary buffer that will be modified when this function is next called.
//
// DIRECTION_UP is "no rotation"
Point2I* SelectPlace_TransformShapePoints(Point2I* translation, Point2I* shapePoints, S32 shapeCount, Direction rotation)
{
    // TODO: Implement SelectPlace_TransformShapePoints
    return NULL;
}
