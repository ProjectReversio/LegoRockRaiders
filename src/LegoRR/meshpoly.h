#pragma once

#include "container.h"
#include "platform.h"

typedef enum MeshLODFlags
{
    MESHLOD_FLAG_NONE     = 0,
    MESHLOD_FLAG_CLONED   = 0x1,
    MESHLOD_FLAG_MEMBLOCK = 0x2,
} MeshLODFlags;

typedef struct MeshLOD
{
    lpContainer contMeshOrigin; // (LWO|MESH, true)
    lpContainer contMeshTarget;
    const char* partName; // name of LoadObject file.lwo
    U32 setID; // MeshLOD's may contain multiple sets of the same parts, this specifies which set it's from.
    MeshLODFlags flags; // (1 = don't free partName/cont_0, 2 = unk dtor behavior)
    struct MeshLOD* next;
} MeshLOD, *lpMeshLOD;


