#include "mesh.h"
#include "mem.h"
#include "file.h"

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
    // TODO: Implement Mesh_CreateOnFrame
    return NULL;
}

lpMesh Mesh_Clone(lpMesh mesh, LPDIRECT3DRMFRAME3 frame)
{
    // TODO: Implement Mesh_Clone
    return NULL;
}
