#pragma once

#include "platform.h"
#include "container.h"

typedef struct SelectPlace
{
    lpContainer contMesh;
    F32 tileDepth;
} SelectPlace, *lpSelectPlace;

extern lpSelectPlace SelectPlace_Create(lpContainer root, F32 tileDepth);

extern void SelectPlace_Hide(lpSelectPlace selectPlace, B32 hide);

// Translate and rotate a set of integer points (max of 20).
// Returns a pointer to a temporary buffer that will be modified when this function is next called.
//
// DIRECTION_UP is "no rotation"
extern Point2I* SelectPlace_TransformShapePoints(Point2I* translation, Point2I* shapePoints, S32 shapeCount, Direction rotation);
