#pragma once

#include "common.h"
#include "container.h"

typedef struct Map3D
{
    U32 blockWidth;
    U32 blockHeight;
    U32 gridWidth;
    U32 gridHeight;
    F32 blockSize;
    F32 roughLevel;

    // TODO: Implement Map3D
} Map3D, *lpMap3D;

extern lpMap3D Map3D_Create(lpContainer root, const char* filename, F32 blockSize, F32 roughLevel);
