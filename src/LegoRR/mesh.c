#include "mesh.h"
#include "mem.h"
#include "file.h"
#include "main.h"
#include "error.h"

#define RELEASE(x) { if (x != NULL) { x->lpVtbl->Release(x); x = NULL; } }

Mesh_Globs meshGlobs = { NULL };

B32 Mesh_Initialize(const char* sharedTextureDir)
{
    U32 len;
    U32 loop;

    meshGlobs.postRenderMeshList = NULL;

    Mesh_ClearPostRenderList();

    if (sharedTextureDir)
    {
        len = strlen(sharedTextureDir);
        if (sharedTextureDir[len - 1] != '\\')
            len++;
        meshGlobs.sharedTextureDir = Mem_Alloc(len + 1);
        strcpy(meshGlobs.sharedTextureDir, sharedTextureDir);
        meshGlobs.sharedTextureDir[len - 1] = '\\';
        meshGlobs.sharedTextureDir[len] = '\0';
    }

    for (loop = 0; loop < MESH_MAXLISTS; loop++)
    {
        meshGlobs.listSet[loop] = NULL;
    }

    meshGlobs.freeList = NULL;
    meshGlobs.listCount = 0;

    Mesh_CreateGlobalMaterial();

    return TRUE;
}

B32 Mesh_SetMaterial(LPD3DMATERIAL newMaterial)
{
    Mesh_Debug_CheckIMDevice_Int();

    if (meshGlobs.imMat->lpVtbl->SetMaterial(meshGlobs.imMat, newMaterial))
    {
        Error_Warn(TRUE, "Cannot 'SetMaterial'.");

        return FALSE;
    }

    return TRUE;
}

B32 Mesh_CreateGlobalMaterial()
{
    LPDIRECT3D3 imd3d;
    D3DMATERIAL material = {
        sizeof (D3DMATERIAL),
        { 1.0f, 1.0f, 1.0f, 1.0f },
        { 1.0f, 1.0f, 1.0f, 1.0f },
        { 0.0f, 0.0f, 0.0f, 0.0f },
        { 0.0f, 0.0f, 0.0f, 0.0f },
        0.0f,
        0,
        0
    };

    Mesh_Debug_CheckIMDevice_Int();

    lpIMDevice()->lpVtbl->GetDirect3D(lpIMDevice(), &imd3d);

    if (imd3d->lpVtbl->CreateMaterial(imd3d, &meshGlobs.imMat, NULL) != D3D_OK)
    {
        Error_Warn(TRUE, "Cannot 'CreateMaterial'.");
        RELEASE(imd3d);

        return FALSE;
    }

    if (meshGlobs.imMat->lpVtbl->GetHandle(meshGlobs.imMat, lpIMDevice(), &meshGlobs.matHandle) != D3D_OK)
    {
        Error_Warn(TRUE, "Cannot 'GetHandle' for material.");
        RELEASE(imd3d);

        return FALSE;
    }

    // SET UP ANY MATERIAL AS DEFAULT
    Mesh_SetMaterial(&material);

    RELEASE(imd3d);

    return TRUE;
}

void Mesh_ClearPostRenderList()
{
    lpMesh_PostRenderInfo info = meshGlobs.postRenderMeshList;
    lpMesh_PostRenderInfo infoNext;

    while (info)
    {
        infoNext = info->next;

        Mem_Free(info);

        info = infoNext;
    }

    meshGlobs.postRenderMeshList = NULL;
}

void Mesh_PostRenderAll(lpViewport vp)
{
    lpMesh_PostRenderInfo info = meshGlobs.postRenderMeshList;
    lpMesh_Group group;
    U32 loop;

    Mesh_Debug_CheckIMDevice_Void();

    if (info)
    {
        Mesh_SetCurrentGODSViewport(vp);
        Mesh_StoreTextureAndMat();

        while (info)
        {
            if (!(info->mesh->flags & MESH_FLAG_HIDDEN))
            {
                Mesh_SetMeshRenderDesc(info->mesh, vp, &info->matWorld, TRUE);

                if (info->mesh->flags & MESH_FLAG_POSTEFFECT)
                {
                    Mesh_RenderMesh(info->mesh, &info->matWorld, TRUE);
                } else
                {
                    // RUN THROUGH GROUPS AND ONLY RENDER TRANSPARENT GROUPS

                    // RENDER ALL GROUPS WITHOUT FLAGS CHANGED FIRST
                    for (loop = 0; loop < info->mesh->groupCount; loop++)
                    {
                        group = &info->mesh->groupList[loop];

                        if (Mesh_CanRenderGroup(group))
                        {
                            if (group->flags & MESH_FLAG_ALPHAENABLE)
                            {
                                if (group->renderFlags == 0)
                                    Mesh_RenderGroup(info->mesh, group, &info->matWorld, TRUE);
                            }
                        }
                    }

                    // RENDER ALL GROUPS WITH FLAGS CHANGED
                    for (loop = 0; loop < info->mesh->groupCount; loop++)
                    {
                        group = &info->mesh->groupList[loop];

                        if (Mesh_CanRenderGroup(group))
                        {
                            if (group->flags & MESH_FLAG_ALPHAENABLE)
                            {
                                if (group->renderFlags)
                                    Mesh_RenderGroup(info->mesh, group, &info->matWorld, TRUE);
                            }
                        }
                    }
                }
            }

            info = info->next;
        }

        Mesh_ClearPostRenderList();

        // RESTORE STATES
        Main_RestoreStates(FALSE);
        Mesh_RestoreTextureAndMat();
    }
}

lpMesh Mesh_Load(const char* fname, LPDIRECT3DRMFRAME3 frame, B32 noTextures)
{
    APPOBJ *lightWaveObject;
    lpMesh mesh;
    char path[FILE_MAXPATH];
    char* s, *t;

    strcpy(path, fname);

    for (s = t = path; *s != '\0'; s++)
    {
        if (*s == '\\')
            t = s;
    }

    *(t + (t != path ? 1 : 0)) = '\0';

    if (LoadAppObj(fname, &lightWaveObject, FALSE))
    {
        mesh = Mesh_CreateOnFrame(frame, NULL, MESH_RENDERFLAGS_LWOALPHA, NULL, Mesh_Type_LightWaveObject);
        Mesh_ParseLWO(path, mesh, lightWaveObject, noTextures);
        FreeLWOB(lightWaveObject);

        return mesh;
    }

    return NULL;
}

B32 Mesh_ParseLWO(const char* basePath, lpMesh mesh, APPOBJ *lightWaveObject, B32 noTextures)
{
    // TODO: Implement Mesh_ParseLWO
    return FALSE;
}

lpMesh Mesh_CreateOnFrame(LPDIRECT3DRMFRAME3 frame, void(*renderFunc)(lpMesh mesh, void* data, lpViewport vp), U32 renderFlags, void* data, U32 type)
{
    lpMesh mesh = Mesh_ObtainFromList();
    lpContainer rootCont = Container_GetRoot();

    Mesh_Debug_CheckIMDevice_Ptr();

    lpD3DRM()->lpVtbl->CreateUserVisual(lpD3DRM(), Mesh_RenderCallback, mesh, &mesh->uv);
    Container_NoteCreation(mesh->uv);
    frame->lpVtbl->AddVisual(frame, (LPUNKNOWN) mesh->uv);
    mesh->uv->lpVtbl->SetAppData(mesh->uv, (DWORD) mesh);
    mesh->frameCreatedOn = frame;

    mesh->renderDesc.renderCallback = renderFunc;
    mesh->renderDesc.renderCallbackData = data;
    mesh->renderDesc.renderFlags = renderFlags;

    if (type == Mesh_Type_PostEffect)
        mesh->flags |= MESH_FLAG_POSTEFFECT;
    else if (type == Mesh_Type_LightWaveObject)
        mesh->flags |= MESH_FLAG_LWO;

    mesh->numOfRefs = 1;

    mesh->textureRenderCallback = NULL;

    return mesh;
}

BOOL Mesh_RenderCallback(LPDIRECT3DRMUSERVISUAL lpD3DRMUV, LPVOID lpArg, D3DRMUSERVISUALREASON lpD3DRMUVreason, LPDIRECT3DRMDEVICE lpD3DRMDev, LPDIRECT3DRMVIEWPORT lpD3DRMview)
{
    // TODO: Implement Mesh_RenderCallback
    return FALSE;
}

B32 Mesh_RenderMesh(lpMesh mesh, LPD3DMATRIX matWorld, B32 alphaBlend)
{
    // TODO: Implement Mesh_RenderMesh
    return FALSE;
}

lpMesh Mesh_Clone(lpMesh mesh, LPDIRECT3DRMFRAME3 frame)
{
    U32 loop;
    lpMesh newMesh = Mesh_ObtainFromList();
    HRESULT r;

    if (mesh->clonedFrom)
        mesh = mesh->clonedFrom;

    *newMesh = *mesh;
    newMesh->clonedFrom = mesh;

    newMesh->listSize = newMesh->groupCount;
    newMesh->groupList = Mem_Alloc(sizeof(Mesh_Group) * newMesh->groupCount);

    for (loop = 0; loop < newMesh->groupCount; loop++)
    {
        Mesh_CreateGroupMaterial(newMesh, loop);
        Mesh_SetGroupMaterial(newMesh, loop, &mesh->groupList[loop].material);
    }

    r = lpD3DRM()->lpVtbl->CreateUserVisual(lpD3DRM(), Mesh_RenderCallback, newMesh, &newMesh->uv);
    Container_NoteCreation(newMesh->uv);
    frame->lpVtbl->AddVisual(frame, (LPUNKNOWN) newMesh->uv);
    newMesh->uv->lpVtbl->SetAppData(newMesh->uv, (DWORD) newMesh);
    newMesh->frameCreatedOn = frame;

    mesh->numOfRefs++;
    mesh->flags |= MESH_FLAG_HASBEENCLONED;

    newMesh->numOfRefs = 0;

    return newMesh;
}

B32 Mesh_CreateGroupMaterial(lpMesh mesh, U32 groupID)
{
    if (groupID < mesh->groupCount)
    {
        lpMesh_Group group = &mesh->groupList[groupID];
        D3DMATERIAL material = {
            sizeof(D3DMATERIAL),
            { 1.0f, 1.0f, 1.0f, 1.0f },
            { 1.0f, 1.0f, 1.0f, 1.0f },
            { 0.0f, 0.0f, 0.0f, 0.0f },
            { 0.0f, 0.0f, 0.0f, 0.0f },
            0.0f,
            0,
            0
        };

        Mesh_Debug_CheckIMDevice_Int();

        group->material = material;

        return TRUE;
    }
    return FALSE;
}

B32 Mesh_SetGroupMaterial(lpMesh mesh, U32 groupID, LPD3DMATERIAL mat)
{
    if (groupID < mesh->groupCount)
    {
        Mesh_Debug_CheckIMDevice_Int();

        mesh->groupList[groupID].material = *mat;

        return TRUE;
    }
    return FALSE;
}

B32 Mesh_CanRenderGroup(lpMesh_Group group)
{
    return (!(group->flags & MESH_FLAG_HIDDEN) && (!group->flags & MESH_FLAG_ALPHAHIDDEN));
}

B32 Mesh_RenderGroup(lpMesh mesh, lpMesh_Group group, LPD3DMATRIX matWorld, B32 alphaBlend)
{
    // TODO: Implement Mesh_RenderGroup
    return FALSE;
}

void Mesh_SetMeshRenderDesc(lpMesh mesh, lpViewport vp, LPD3DMATRIX matWorld, B32 alphaBlend)
{
    // TODO: Implement Mesh_SetMeshRenderDesc
}

B32 Mesh_SetCurrentGODSViewport(lpViewport vp)
{
    // TODO: Implement Mesh_SetCurrentGODSViewport
    return FALSE;
}

void Mesh_StoreTextureAndMat()
{
    // TODO: Implement Mesh_StoreTextureAndMat
}

void Mesh_RestoreTextureAndMat()
{
    // TODO: Implement Mesh_RestoreTextureAndMat
}

lpMesh Mesh_ObtainFromList()
{
    lpMesh newMesh;

    if (meshGlobs.freeList == NULL)
        Mesh_AddList();

    newMesh = meshGlobs.freeList;
    meshGlobs.freeList = newMesh->nextFree;
    memset(newMesh, 0, sizeof(Mesh));
    newMesh->nextFree = newMesh;

    return newMesh;
}

void Mesh_ReturnToList(lpMesh dead)
{
    Error_Fatal(!dead, "NULL passed to Mesh_Remove()");

    dead->nextFree = meshGlobs.freeList;
    meshGlobs.freeList = dead;
}

void Mesh_AddList()
{
    lpMesh list;
    U32 loop, count;

    Error_Fatal(meshGlobs.listCount + 1 >= MESH_MAXLISTS, "Run out of lists");

    count = 0x00000001 << meshGlobs.listCount;

    if ((list = meshGlobs.listSet[meshGlobs.listCount] = Mem_Alloc(sizeof(Mesh) * count)))
    {
        meshGlobs.listCount++;

        for (loop = 1; loop < count; loop++)
            list[loop - 1].nextFree = &list[loop];

        list[count - 1].nextFree = meshGlobs.freeList;
        meshGlobs.freeList = list;
    } else
    {
        Error_Fatal(TRUE, Error_Format("Unable to allocate %d bytes of memory for new list.\n", sizeof(Mesh) * count));
    }
}
