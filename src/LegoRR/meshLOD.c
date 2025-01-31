#include "meshLOD.h"

#include <stdio.h>

#include "mem.h"

lpMeshLOD MeshLOD_Create(lpMeshLOD optPrevMeshLOD, const char* partName, const char* dirname, const char* meshName, U32 setID)
{
    char fname[256];
    sprintf(fname, "%s\\%s", dirname, meshName);
    lpContainer cont = Container_Load(NULL, fname, "LWO", TRUE);
    if (!cont)
    {
        cont = Container_Load(NULL, fname, "MESH", TRUE);
        if (!cont)
            return NULL;
    }

    lpMeshLOD newMeshLOD = (lpMeshLOD)Mem_Alloc(sizeof(MeshLOD));
    newMeshLOD->flags = MESHLOD_FLAG_MEMBLOCK;
    if (optPrevMeshLOD != NULL)
        optPrevMeshLOD->next = newMeshLOD;

    newMeshLOD->contMeshOrigin = cont;
    newMeshLOD->next = NULL;
    newMeshLOD->setID = setID;

    newMeshLOD->partName = Mem_Alloc(strlen(partName) + 1);
    strcpy(newMeshLOD->partName, partName);

    newMeshLOD->contMeshTarget = NULL;

    return newMeshLOD;
}

// Creates a MeshLOD struct pointing to a non-existent Mesh.
// AKA, when a part is configured as NULL in the .ae file.
lpMeshLOD MeshLOD_CreateEmpty(lpMeshLOD optPrevMeshLOD, const char* partName, U32 setID)
{
    lpMeshLOD newMeshLOD = (lpMeshLOD)Mem_Alloc(sizeof(MeshLOD));
    newMeshLOD->flags = MESHLOD_FLAG_MEMBLOCK;
    if (optPrevMeshLOD != NULL)
        optPrevMeshLOD->next = newMeshLOD;

    newMeshLOD->setID = setID;
    newMeshLOD->next = NULL;
    newMeshLOD->contMeshOrigin = NULL;

    newMeshLOD->partName = Mem_Alloc(strlen(partName) + 1);
    strcpy(newMeshLOD->partName, partName);

    newMeshLOD->contMeshTarget = NULL;

    return newMeshLOD;
}

lpMeshLOD MeshLOD_Clone(lpMeshLOD srcMeshLOD)
{
    S32 count = 0;
    lpMeshLOD clonedMeshLOD = srcMeshLOD;
    while (clonedMeshLOD != NULL)
    {
        clonedMeshLOD = clonedMeshLOD->next;
        count++;
    }

    if (count == 0)
        return NULL;

    clonedMeshLOD = (lpMeshLOD)Mem_Alloc(sizeof(MeshLOD) * count);

    S32 i = 0;

    while (srcMeshLOD != NULL)
    {
        memcpy(clonedMeshLOD, srcMeshLOD, sizeof(MeshLOD));
        clonedMeshLOD->setID = 0;
        clonedMeshLOD->flags &= ~MESHLOD_FLAG_MEMBLOCK;
        clonedMeshLOD->flags |= MESHLOD_FLAG_CLONED;
        clonedMeshLOD->next = &clonedMeshLOD[i + 1];
        i++;
        srcMeshLOD = srcMeshLOD->next;
    }

    clonedMeshLOD->flags |= MESHLOD_FLAG_MEMBLOCK;
    clonedMeshLOD[i - 1].next = NULL;

    return clonedMeshLOD;
}

void MeshLOD_SwapTarget(lpMeshLOD meshLOD, lpContainer contActTarget, B32 restore, U32 setID)
{
    while (meshLOD != NULL)
    {
        if (meshLOD->setID == setID)
        {
            if (meshLOD->contMeshTarget == NULL)
            {
                const char *name = Container_FormatPartName(contActTarget, meshLOD->partName, NULL);
                Container *part = Container_SearchTree(contActTarget, name, Container_SearchMode_FirstMatch, NULL);
                meshLOD->contMeshTarget = part;
            }
            if (meshLOD->contMeshTarget != NULL)
            {
                Container_Mesh_Swap(meshLOD->contMeshTarget, meshLOD->contMeshOrigin, restore);
            }
        }

        meshLOD = meshLOD->next;
    }
}

void MeshLOD_RemoveTargets(lpMeshLOD meshLOD)
{
    while (meshLOD != NULL)
    {
        meshLOD->contMeshTarget = NULL;
        meshLOD = meshLOD->next;
    }
}

void MeshLOD_Free(lpMeshLOD meshLOD)
{
    lpMeshLOD nextAlloc = NULL;
    lpMeshLOD prevAlloc = NULL;
    lpMeshLOD m = meshLOD;

    while (m != NULL)
    {
        if (!(m->flags & MESHLOD_FLAG_CLONED))
        {
            if (m->partName != NULL)
                Mem_Free(m->partName);
            if (m->contMeshOrigin != NULL)
                Container_Remove(m->contMeshOrigin);
        }

        nextAlloc = m;

        if (!(m->flags & MESHLOD_FLAG_MEMBLOCK) && (nextAlloc = prevAlloc, prevAlloc != NULL))
        {
            // Splice out items that are not the start of a contiguous memory block.
            prevAlloc->next = m->next;
        }

        m = m->next;
        prevAlloc = nextAlloc;

    }

    if (nextAlloc != NULL && meshLOD != NULL)
    {
        do
        {
            m = meshLOD->next;
            Mem_Free(meshLOD);
            meshLOD = m;
        } while (m != NULL);
    }
}

