#pragma once

#include "types.h"
#include "container.h"

typedef struct SelectPlace
{
    lpContainer contMesh;
    F32 tileDepth;
} SelectPlace, *lpSelectPlace;

extern lpSelectPlace SelectPlace_Create(lpContainer root, F32 tileDepth);

extern void SelectPlace_Hide(lpSelectPlace selectPlace, B32 hide);
