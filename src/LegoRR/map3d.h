#pragma once

#include "common.h"
#include "container.h"

#pragma pack(push, 2)
typedef struct MapFileInfo
{
    char Signature[4]; // "MAP "
    U32 fileSize;
    Size2I dimensions;
    U16 blocks[1];
} MapFileInfo, *lpMapFileInfo;
#pragma pack(pop)

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
