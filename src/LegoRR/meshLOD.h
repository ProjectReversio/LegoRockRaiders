#pragma once

#include "platform.h"
#include "container.h"

typedef enum MeshLODFlags
{
    MESHLOD_FLAG_NONE     = 0,
    MESHLOD_FLAG_CLONED   = 0x1,
    MESHLOD_FLAG_MEMBLOCK = 0x2,
} MeshLODFlags;

typedef enum LOD_PolyLevel
{
    LOD_LowPoly    = 0,
    LOD_MediumPoly = 1,
    LOD_HighPoly   = 2,
    LOD_FPPoly     = 3,
} LOD_PolyLevel;

typedef struct MeshLOD
{
    lpContainer contMeshOrigin; // (LWO|MESH, true)
    lpContainer contMeshTarget;
    const char* partName; // name of LoadObject file.lwo
    U32 setID; // MeshLOD's may contain multiple sets of the same parts, this specifies which set it's from.
    MeshLODFlags flags; // (1 = don't free partName/cont_0, 2 = unk dtor behavior)
    struct MeshLOD* next;
} MeshLOD, *lpMeshLOD;

// Creates a MeshLOD struct pointing to a non-existent Mesh.
// AKA, when a part is configured as NULL in the .ae file.
extern lpMeshLOD MeshLOD_CreateEmpty(lpMeshLOD optPrevMeshLOD, const char* partName, U32 setID);
extern lpMeshLOD MeshLOD_Create(lpMeshLOD optPrevMeshLOD, const char* partName, const char* dirname, const char* meshName, U32 setID);
