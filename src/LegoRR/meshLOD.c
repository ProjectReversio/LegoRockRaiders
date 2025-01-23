#include "meshLOD.h"

lpMeshLOD MeshLOD_Create(lpMeshLOD optPrevMeshLOD, const char* partName, const char* dirname, const char* meshName, U32 setID)
{
    // TODO: Implement MeshLOD_Create

    return NULL;
}

// Creates a MeshLOD struct pointing to a non-existent Mesh.
// AKA, when a part is configured as NULL in the .ae file.
lpMeshLOD MeshLOD_CreateEmpty(lpMeshLOD optPrevMeshLOD, const char* partName, U32 setID)
{
    // TODO: Implement MeshLOD_CreateEmpty

    return NULL;
}

lpMeshLOD MeshLOD_Clone(lpMeshLOD srcMeshLOD)
{
    // TODO: Implement MeshLOD_Clone

    return NULL;
}

void MeshLOD_SwapTarget(lpMeshLOD meshLOD, lpContainer contActTarget, B32 restore, U32 setID)
{
    // TODO: Implement MeshLOD_SwapTarget
}

void MeshLOD_RemoveTargets(lpMeshLOD meshLOD)
{
    // TODO: Implement MeshLOD_RemoveTargets
}

void MeshLOD_Free(lpMeshLOD meshLOD)
{
    // TODO: Implement MeshLOD_Free
}

