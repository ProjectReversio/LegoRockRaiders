#include "mesh.h"
#include "mem.h"
#include "file.h"
#include "main.h"
#include "error.h"

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

void Mesh_ClearPostRenderList()
{
    // TODO: Implement Mesh_ClearPostRenderList
}

B32 Mesh_CreateGlobalMaterial()
{
    // TODO: Implement Mesh_CreateGlobalMaterial
    return FALSE;
}

void Mesh_PostRenderAll(lpViewport vp)
{
    // TODO: Implement Mesh_PostRenderAll
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

lpMesh Mesh_Clone(lpMesh mesh, LPDIRECT3DRMFRAME3 frame)
{
    // TODO: Implement Mesh_Clone
    return NULL;
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
