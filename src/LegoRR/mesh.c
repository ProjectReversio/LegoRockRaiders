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
    U32 face, vertex;
    Point3F pos[3];
    U32 faceDataQuad[] = { 0, 1, 2, 0, 2, 3 };
    F32 textCoords[][2] = {
        {0, 1}, {0, 0}, {1, 0}, {1, 1}
    };
    U32 group, loop, newFlags;
    U32* surfVertexRef;
    lpPoint3F surfVertices;
    lpPoint3F *surfVerticesNorm;
    U32* surfFaceData;
    U32* surfVertexCount;
    U32* surfFaceDataCount;
    lpPoint3F faceNorm;
    lpPoint3F vertexNorm;
    lpPoint2F vertexUVs = NULL;
    lpPoint2F surfTextCoords;
    U32 size = lightWaveObject->aoSize.lwVertCount * 2;
    U32 fDSize = lightWaveObject->aoSize.lwPolyCount * 6;
    U32 vertexIndex, surfArraySize, surfFDArraySize;

    Error_Fatal(!lightWaveObject->aoSize.lwSurfaceCount, "LightWave object must contain at least one surface.");

#define MESH_MAXPOLYSPERVERTEX 20

    // GET TEXTURE AND TEXTURE COORDINATES FOR VERTICES IF UVIEW FILE EXISTS
    // NEED TO CALL THIS FIRST BECAUSE THE LIGHTWAVE STRUCTURE MAY BE ALTERED
    if (lightWaveObject->aoFileUV)
    {
        vertexUVs = Mem_Alloc(sizeof(Point2F) * size * MESH_MAXPOLYSPERVERTEX);
        Mesh_UViewMesh(lightWaveObject, vertexUVs);
        Error_Fatal(size * MESH_MAXPOLYSPERVERTEX < lightWaveObject->aoSize.lwVertCount, "MESH_MAXPOLYSPERVERTEX too small");

        size = lightWaveObject->aoSize.lwVertCount * 2;
        surfTextCoords = Mem_Alloc(sizeof(Point2F) * lightWaveObject->aoSize.lwSurfaceCount * size);
    } else
    {
        surfTextCoords = Mem_Alloc(sizeof(Point2F) * size);
    }

    // ALLOCATE MEMORY FOR OBJECT
    surfArraySize = lightWaveObject->aoSize.lwSurfaceCount * size;

    surfVertices = Mem_Alloc(sizeof(Point3F) * surfArraySize);
    surfVerticesNorm = Mem_Alloc(sizeof(lpPoint3F) * surfArraySize);
    surfVertexCount = Mem_Alloc(sizeof(U32) * lightWaveObject->aoSize.lwSurfaceCount);
    surfVertexRef = Mem_Alloc(sizeof(U32) * surfArraySize);
    surfFaceData = Mem_Alloc(sizeof(U32) * (lightWaveObject->aoSize.lwSurfaceCount * fDSize));
    surfFaceDataCount = Mem_Alloc(sizeof(U32) * lightWaveObject->aoSize.lwSurfaceCount);
    faceNorm = Mem_Alloc(sizeof(Point3F) * lightWaveObject->aoSize.lwPolyCount);
    vertexNorm = Mem_Alloc(sizeof(Point3F) * size);

    memset(surfVertexCount, 0, sizeof(U32) * lightWaveObject->aoSize.lwSurfaceCount);
    memset(surfVertexRef, -1, sizeof(U32) * surfArraySize);
    memset(surfFaceDataCount, 0, sizeof(U32) * lightWaveObject->aoSize.lwSurfaceCount);
    memset(vertexNorm, 0, sizeof(Point3F) * size);

    mesh->lightWaveSurf = Mem_Alloc(sizeof(Mesh_LightWave_Surface) * lightWaveObject->aoSize.lwSurfaceCount);
    memset(mesh->lightWaveSurf, 0, sizeof(Mesh_LightWave_Surface) * lightWaveObject->aoSize.lwSurfaceCount);

    // STORE FACE NORMALS FOR GOURAUD SHADING - ASSUMES ALL POLYS ARE PLANAR
    for (face = 0; face < lightWaveObject->aoSize.lwPolyCount; face++)
    {
        for (vertex = 0; vertex < 3; vertex++)
        {
            vertexIndex = lightWaveObject->aoPoly[face].plyData[vertex] * 3;

            pos[vertex].x = lightWaveObject->aoVerts[vertexIndex];
            pos[vertex].y = lightWaveObject->aoVerts[vertexIndex + 1];
            pos[vertex].z = lightWaveObject->aoVerts[vertexIndex + 2];
        }

        Maths_PlaneNormal(&faceNorm[face], &pos[0], &pos[1], &pos[2]);
    }

    // READ SURFACE INFORMATION
    Mesh_GetSurfInfo(basePath, lightWaveObject, mesh->lightWaveSurf, noTextures);

    // READ VERTEX INFORMATION
    for (face = 0; face < lightWaveObject->aoSize.lwPolyCount; face++)
    {
        surfArraySize = lightWaveObject->aoPoly[face].plySurface * size;
        surfFDArraySize = lightWaveObject->aoPoly[face].plySurface * fDSize;

        if (lightWaveObject->aoPoly[face].plyCount == 3)
        {
            for (vertex = 0; vertex < 3; vertex++)
            {
                vertexIndex = lightWaveObject->aoPoly[face].plyData[vertex] * 3;

                if (surfVertexRef[surfArraySize + lightWaveObject->aoPoly[face].plyData[vertex]] == -1)
                {
                    // STORE NEW VERTEX IN SURFACE LIST
                    surfVertices[surfArraySize + surfVertexCount[lightWaveObject->aoPoly[face].plySurface]].x
                        = lightWaveObject->aoVerts[vertexIndex];
                    surfVertices[surfArraySize + surfVertexCount[lightWaveObject->aoPoly[face].plySurface]].y
                        = lightWaveObject->aoVerts[vertexIndex + 1];
                    surfVertices[surfArraySize + surfVertexCount[lightWaveObject->aoPoly[face].plySurface]].z
                        = lightWaveObject->aoVerts[vertexIndex + 2];

                    if (lightWaveObject->aoFileUV)
                        surfTextCoords[surfArraySize + surfVertexCount[lightWaveObject->aoPoly[face].plySurface]] = vertexUVs[lightWaveObject->aoPoly[face].plyData[vertex]];

                    // ADD VERTEX FACE NORMAL TO CURRENT VERTEX NORMAL
                    Maths_Vector3DAdd(&vertexNorm[lightWaveObject->aoPoly[face].plyData[vertex]],
                                      &vertexNorm[lightWaveObject->aoPoly[face].plyData[vertex]],
                                      &faceNorm[face]);
                    Maths_Vector3DNormalize(&vertexNorm[lightWaveObject->aoPoly[face].plyData[vertex]]);
                    surfVerticesNorm[surfArraySize + surfVertexCount[lightWaveObject->aoPoly[face].plySurface]]
                        = &vertexNorm[lightWaveObject->aoPoly[face].plyData[vertex]];

                    // STORE NEW VERTEX IN FACE DATA
                    surfFaceData[surfFDArraySize + surfFaceDataCount[lightWaveObject->aoPoly[face].plySurface]++]
                        = surfVertexCount[lightWaveObject->aoPoly[face].plySurface];

                    // STORE NEW VERTEX REFERENCE IN VERTEX LIST
                    surfVertexRef[surfArraySize + lightWaveObject->aoPoly[face].plyData[vertex]]
                        = surfVertexCount[lightWaveObject->aoPoly[face].plySurface]++;
                } else {
                    // ADD VERTEX FACE NORMAL TO CURRENT VERTEX NORMAL
                    Maths_Vector3DAdd(&vertexNorm[lightWaveObject->aoPoly[face].plyData[vertex]],
                                      &vertexNorm[lightWaveObject->aoPoly[face].plyData[vertex]],
                                      &faceNorm[face]);
                    Maths_Vector3DNormalize(&vertexNorm[lightWaveObject->aoPoly[face].plyData[vertex]]);

                    // GET VERTEX REFERENCE AND COPY TO FACE DATA
                    surfFaceData[surfFDArraySize + surfFaceDataCount[lightWaveObject->aoPoly[face].plySurface]++]
                        = surfVertexRef[surfArraySize + lightWaveObject->aoPoly[face].plyData[vertex]];
                }
            }
        } else
        {
            Error_Warn(((lightWaveObject->aoPoly[face].plyCount != 4) && (lightWaveObject->aoPoly[face].plyCount != 3)),
                       "Mesh contains polys which are not triples or quads.");

            surfArraySize = lightWaveObject->aoPoly[face].plySurface * size;

            for (vertex = 0; vertex < 6; vertex++)
            {
                if (surfVertexRef[surfArraySize + lightWaveObject->aoPoly[face].plyData[faceDataQuad[vertex]]] == -1)
                {
                    vertexIndex = lightWaveObject->aoPoly[face].plyData[faceDataQuad[vertex]] * 3;

                    // STORE NEW VERTEX IN SURFACE LIST
                    surfVertices[surfArraySize + surfVertexCount[lightWaveObject->aoPoly[face].plySurface]].x
                        = lightWaveObject->aoVerts[vertexIndex];
                    surfVertices[surfArraySize + surfVertexCount[lightWaveObject->aoPoly[face].plySurface]].y
                        = lightWaveObject->aoVerts[vertexIndex + 1];
                    surfVertices[surfArraySize + surfVertexCount[lightWaveObject->aoPoly[face].plySurface]].z
                        = lightWaveObject->aoVerts[vertexIndex + 2];

                    if (lightWaveObject->aoFileUV)
                        surfTextCoords[surfArraySize + surfVertexCount[lightWaveObject->aoPoly[face].plySurface]] = vertexUVs[lightWaveObject->aoPoly[face].plyData[faceDataQuad[vertex]]];

                    // ADD VERTEX FACE NORMAL TO CURRENT VERTEX NORMAL
                    Maths_Vector3DAdd(&vertexNorm[lightWaveObject->aoPoly[face].plyData[faceDataQuad[vertex]]],
                                      &vertexNorm[lightWaveObject->aoPoly[face].plyData[faceDataQuad[vertex]]],
                                      &faceNorm[face]);
                    Maths_Vector3DNormalize(&vertexNorm[lightWaveObject->aoPoly[face].plyData[faceDataQuad[vertex]]]);
                    surfVerticesNorm[surfArraySize = surfVertexCount[lightWaveObject->aoPoly[face].plySurface]]
                        = &vertexNorm[lightWaveObject->aoPoly[face].plyData[faceDataQuad[vertex]]];

                    // STORE NEW VERTEX IN FACE DATA
                    surfFaceData[surfFDArraySize + surfFaceDataCount[lightWaveObject->aoPoly[face].plySurface]++]
                        = surfVertexCount[lightWaveObject->aoPoly[face].plySurface];

                    // STORE NEW VERTEX REFERENCE IN VERTEX LIST
                    surfVertexRef[surfArraySize + lightWaveObject->aoPoly[face].plyData[faceDataQuad[vertex]]]
                        = surfVertexCount[lightWaveObject->aoPoly[face].plySurface]++;
                } else
                {
                    // ADD VERTEX FACE NORMAL TO CURRENT VERTEX NORMAL
                    Maths_Vector3DAdd(&vertexNorm[lightWaveObject->aoPoly[face].plyData[faceDataQuad[vertex]]],
                                      &vertexNorm[lightWaveObject->aoPoly[face].plyData[faceDataQuad[vertex]]],
                                      &faceNorm[face]);
                    Maths_Vector3DNormalize(&vertexNorm[lightWaveObject->aoPoly[face].plyData[faceDataQuad[vertex]]]);

                    // GET VERTEX REFERENCE AND COPY TO FACE DATA
                    surfFaceData[surfFDArraySize + surfFaceDataCount[lightWaveObject->aoPoly[face].plySurface]++]
                        = surfVertexRef[surfArraySize + lightWaveObject->aoPoly[face].plyData[faceDataQuad[vertex]]];
                }
            }
        }
    }

    // ADD A GROUP FOR EACH SURFACE IN OBJECT
    for (loop = 0; loop < lightWaveObject->aoSize.lwSurfaceCount; loop++)
    {
        surfArraySize = loop * size;
        surfFDArraySize = loop * fDSize;

        group = Mesh_AddGroup(mesh, surfFaceDataCount[loop], surfFaceDataCount[loop] / 3, 3, &surfFaceData[surfFDArraySize]);

        // SET NORMALS
        if (!lightWaveObject->aoFileUV)
        {
            Mesh_GetTextureUVsWrap(surfVertexCount[loop], &surfVertices[surfArraySize], surfTextCoords,
                                   lightWaveObject->aoSurface[loop].srfTexSize.tdX, lightWaveObject->aoSurface[loop].srfTexSize.tdY, lightWaveObject->aoSurface[loop].srfTexSize.tdZ,
                                   lightWaveObject->aoSurface[loop].srfTexCentre.tdX, lightWaveObject->aoSurface[loop].srfTexCentre.tdY, lightWaveObject->aoSurface[loop].srfTexCentre.tdZ,
                                   lightWaveObject->aoSurface[loop].srfTexFlags);

            Mesh_SetVertices_VNT(mesh, group, 0, surfVertexCount[loop], &surfVertices[surfArraySize], &surfVerticesNorm[surfArraySize], surfTextCoords);
        }
        else
        {
            Mesh_SetVertices_VNT(mesh, group, 0, surfVertexCount[loop], &surfVertices[surfArraySize], &surfVerticesNorm[surfArraySize], &surfTextCoords[surfArraySize]);
        }

        // ONLY ALTER RENDERING FLAGS FOR GROUP IF NEEDED
        newFlags = MESH_RENDERFLAGS_LWONORM;

        if (mesh->lightWaveSurf[loop].flags & SFM_ADDITIVE)
        {
            newFlags |= MESH_FLAG_RENDER_ALPHASA1;
            Mesh_AlterGroupRenderFlags(mesh, group, newFlags);
        } else
        {
            newFlags |= MESH_FLAG_RENDER_ALPHATRANS;
        }

        if (mesh->lightWaveSurf[loop].flags & SFM_DOUBLESIDED)
        {
            newFlags |= MESH_FLAG_RENDER_DOUBLESIDED;
            Mesh_AlterGroupRenderFlags(mesh, group, newFlags);
        }

        Mesh_AlterGroupRenderFlags(mesh, group, newFlags);

        // SET TEXTURE
        if ((lightWaveObject->aoSurface[loop].srfTexFlags & TFM_SEQUENCE))
        {
            if (mesh->lightWaveSurf[loop].textureSeq)
                Mesh_SetGroupTexture(mesh, group, mesh->lightWaveSurf[loop].textureSeq[0]);
        } else
        {
            if (mesh->lightWaveSurf[loop].texture)
                Mesh_SetGroupTexture(mesh, group, mesh->lightWaveSurf[loop].texture);
        }

        // SET COLOUR
        Mesh_SetGroupDiffuse(mesh, group,
                             mesh->lightWaveSurf[loop].colour.x * mesh->lightWaveSurf[loop].diffuse,
                             mesh->lightWaveSurf[loop].colour.y * mesh->lightWaveSurf[loop].diffuse,
                             mesh->lightWaveSurf[loop].colour.z * mesh->lightWaveSurf[loop].diffuse);
        Mesh_SetGroupEmissive(mesh, group,
                              mesh->lightWaveSurf[loop].colour.x * mesh->lightWaveSurf[loop].emissive,
                              mesh->lightWaveSurf[loop].colour.y * mesh->lightWaveSurf[loop].emissive,
                              mesh->lightWaveSurf[loop].colour.z * mesh->lightWaveSurf[loop].emissive);
        if (mesh->lightWaveSurf[loop].flags & SFM_COLORHIGHLIGHTS)
        {
            Mesh_SetGroupSpecular(mesh, group,
                                  mesh->lightWaveSurf[loop].colour.x * mesh->lightWaveSurf[loop].specular,
                                  mesh->lightWaveSurf[loop].colour.y * mesh->lightWaveSurf[loop].specular,
                                  mesh->lightWaveSurf[loop].colour.z * mesh->lightWaveSurf[loop].specular);
        } else
        {
            Mesh_SetGroupSpecular(mesh, group,
                                  mesh->lightWaveSurf[loop].specular,
                                  mesh->lightWaveSurf[loop].specular,
                                  mesh->lightWaveSurf[loop].specular);
        }

        Mesh_SetGroupPower(mesh, group, mesh->lightWaveSurf[loop].power * 10.0f);
        Mesh_SetGroupAlpha(mesh, group, 1.0f - mesh->lightWaveSurf[loop].transparency);

        // STORE SURFACE INFORMATION FOR GROUP
        mesh->groupList[group].lightWaveSurfaceInfo = &mesh->lightWaveSurf[loop];
    }

    // FREE ALLOCATED MEMORY
    Mem_Free(surfVertices);
    Mem_Free(surfVerticesNorm);
    Mem_Free(surfTextCoords);
    Mem_Free(surfVertexCount);
    Mem_Free(surfVertexRef);
    Mem_Free(surfFaceData);
    Mem_Free(surfFaceDataCount);
    Mem_Free(faceNorm);
    Mem_Free(vertexNorm);
    if (vertexUVs)
        Mem_Free(vertexUVs);

    return TRUE;
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

void Mesh_UViewMesh(APPOBJ* lightWaveObject, Point2F textCoords[])
{
    // TODO: Implement Mesh_UViewMesh
}

void Mesh_GetSurfInfo(const char* basePath, APPOBJ* lightWaveObject, Mesh_LightWave_Surface lightWaveSurf[], B32 noTextures)
{
    // TODO: Implement Mesh_GetSurfInfo
}

S32 Mesh_AddGroup(lpMesh mesh, U32 vertexCount, U32 faceCount, U32 vPerFace, U32* faceData)
{
    U32 size, loop;
    lpMesh_Group group;

    Mesh_Debug_CheckIMDevice_Int();
    Error_Fatal((vPerFace != 3), "Only triangles supported so far.");
    Error_Fatal((mesh->clonedFrom != NULL) || (mesh->flags & MESH_FLAG_HASBEENCLONED), "Cannot AddGroup() to a cloned mesh");

    if (mesh->groupCount == mesh->listSize)
    {
        mesh->listSize = max(MESH_DEFLISTSIZE, (mesh->listSize * MESH_LISTINCREASE) / 100);
        mesh->groupList = Mem_ReAlloc(mesh->groupList, sizeof(mesh->groupList[0]) * mesh->listSize);
    }

    group = &mesh->groupList[mesh->groupCount++];
    memset(group, 0, sizeof(&group));

    size = sizeof(U16) * faceCount * vPerFace;
    group->faceData = Mem_Alloc(size);
    group->faceDataSize = faceCount * vPerFace;
    for (loop = 0; loop < group->faceDataSize; loop++)
        group->faceData[loop] = (U16) faceData[loop];

    size = sizeof(Mesh_Vertex) * vertexCount;
    group->vertices = Mem_Alloc(size);
    memset(group->vertices, 0, size);

    group->vertexCount = vertexCount;
    group->flags = 0x00000000;

    Mesh_CreateGroupMaterial(mesh, (mesh->groupCount - 1));

    return mesh->groupCount - 1;
}

void Mesh_GetTextureUVsWrap(U32 vertexCount, Point3F vertices[], Point2F coords[], F32 sx, F32 sy, F32 sz, F32 px, F32 py, F32 pz, U32 flags)
{
    // TODO: Implement Mesh_GetTextureUVsWrap
}

void Mesh_SetVertices_VNT(lpMesh mesh, U32 groupID, U32 index, U32 count, Point3F vertices[], lpPoint3F normal[], Point2F textCoords[])
{
    // TODO: Implement Mesh_SetVertices_VNT
}

void Mesh_AlterGroupRenderFlags(lpMesh mesh, U32 groupID, U32 newFlags)
{
    if (groupID < mesh->groupCount)
        mesh->groupList[groupID].renderFlags = newFlags;
}

void Mesh_SetGroupTexture(lpMesh mesh, U32 groupID, lpMesh_Texture mt)
{
    // TODO: Implement Mesh_SetGroupTexture
}

B32 Mesh_SetGroupColour(lpMesh mesh, U32 groupID, F32 r, F32 g, F32 b, U32 type)
{
    // TODO: Implement Mesh_SetGroupColour
    return FALSE;
}

void Mesh_GetGroupColour(lpMesh mesh, U32 groupID, F32* r, F32* g, F32* b, U32 type)
{
    // TODO: Implement Mesh_GetGroupColour
}

B32 Mesh_SetGroupMaterialValues(lpMesh mesh, U32 groupID, F32 value, U32 type)
{
    // TODO: Implement Mesh_SetGroupMaterialValues
    return FALSE;
}

void Mesh_GetGroupMaterialValues(lpMesh mesh, U32 groupID, F32* value, U32 type)
{
    // TODO: Implement Mesh_GetGroupMaterialValues
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

B32 Mesh_SetTransform(D3DTRANSFORMSTATETYPE type, Matrix4F* matrix)
{
    // TODO: Implement Mesh_SetTransform
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

void Mesh_Remove(lpMesh mesh, LPDIRECT3DRMFRAME3 frame)
{
    // TODO: Implement Mesh_Remove
}
