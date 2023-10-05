#include <stdio.h>
#include "mesh.h"
#include "mem.h"
#include "file.h"
#include "main.h"
#include "error.h"
#include "utils.h"

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
        { 0.0f, 0.0f, 0.0f, 0.0f },
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

void Mesh_AddToPostRenderList(lpMesh mesh, LPD3DMATRIX matWorld)
{
    lpMesh_PostRenderInfo info;

    info = Mem_Alloc(sizeof(Mesh_PostRenderInfo));
    memset(info, 0, sizeof(Mesh_PostRenderInfo));

    info->mesh = mesh;
    if (matWorld)
        info->matWorld = (*matWorld);

    // ADD TO LINKED LIST
    info->next = meshGlobs.postRenderMeshList;
    meshGlobs.postRenderMeshList = info;
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
                    surfVerticesNorm[surfArraySize + surfVertexCount[lightWaveObject->aoPoly[face].plySurface]]
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

        if (!(mesh->lightWaveSurf[loop].texFlags & TFM_PIXEL_BLENDING))
        {
            newFlags |= MESH_FLAG_RENDER_FILTERNEAREST;
            Mesh_AlterGroupRenderFlags(mesh, group, newFlags);
        }

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
    lpMesh mesh = lpArg;
    lpViewport vp;
    D3DMATRIX matWorld;

    if (lpD3DRMUVreason == D3DRMUSERVISUAL_CANSEE)
    {
        if (lpIMDevice())
        {
            if (mesh->flags & MESH_FLAG_HIDDEN)
                return FALSE;

            if ((mesh->flags & MESH_FLAG_FACECAMERA) && !(mesh->flags & MESH_FLAG_FACECAMERADONE))
            {
                LPDIRECT3DRMFRAME3 camera, scene;
                vp = (lpViewport) lpD3DRMview->lpVtbl->GetAppData(lpD3DRMview);
                if (vp->rendering)
                {
                    mesh->frameCreatedOn->lpVtbl->GetScene(mesh->frameCreatedOn, &scene);
                    vp->lpVP->lpVtbl->GetCamera(vp->lpVP, &camera);
                    mesh->frameCreatedOn->lpVtbl->LookAt(mesh->frameCreatedOn, camera, scene, 0);
                    mesh->flags |= MESH_FLAG_FACECAMERADONE;
                }
            }

            return TRUE;
        } else
        {
            return FALSE;
        }
    } else if (lpD3DRMUVreason == D3DRMUSERVISUAL_RENDER)
    {
        if (lpIMDevice())
        {
            LPDIRECT3DRMVIEWPORT2 view;
            lpMesh_Group group;
            U32 loop;
            B32 postRender = FALSE;
            B32 renderStateSet = FALSE;

            mesh->flags &= ~MESH_FLAG_FACECAMERADONE;

            // GET THE CURRENT RM WORLD MATRIX FOR POST RENDER
            lpIMDevice()->lpVtbl->GetTransform(lpIMDevice(), D3DTRANSFORMSTATE_WORLD, &matWorld);

            Mesh_SetCurrentViewport(lpD3DRMview);
            lpD3DRMview->lpVtbl->QueryInterface(lpD3DRMview, &IID_IDirect3DRMViewport2, &view);

            vp = (lpViewport) view->lpVtbl->GetAppData(view);

            if (mesh->flags & MESH_FLAG_POSTEFFECT)
            {
                // POST RENDER MESH
                Mesh_AddToPostRenderList(mesh, &matWorld);
            } else if (mesh->renderDesc.renderFlags & MESH_FLAG_RENDER_ALLALPHA)
            {
                // RENDER ALL GROUPS WITHOUT FLAGS CHANGED FIRST
                for (loop = 0; loop < mesh->groupCount; loop++)
                {
                    group = &mesh->groupList[loop];

                    if (Mesh_CanRenderGroup(group))
                    {
                        if (group->flags & MESH_FLAG_ALPHAENABLE)
                        {
                            // POST RENDER MESH
                            // DO NOT RENDER GROUP NOW
                            postRender = TRUE;
                            continue;
                        }

                        if (group->renderFlags == 0)
                        {
                            // GROUP OPAQUE SO RENDER NOW
                            if (!renderStateSet)
                            {
                                Mesh_StoreTextureAndMat();
                                Mesh_SetMeshRenderDesc(mesh, vp, &matWorld, FALSE);
                                renderStateSet = TRUE;
                            }

                            Mesh_RenderGroup(mesh, group, &matWorld, FALSE);
                        }
                    }
                }

                // RENDER ALL GROUPS WITH FLAGS CHANGED FIRST
                for (loop = 0; loop < mesh->groupCount; loop++)
                {
                    group = &mesh->groupList[loop];

                    if (Mesh_CanRenderGroup(group))
                    {
                        if (group->flags & MESH_FLAG_ALPHAENABLE)
                        {
                            // POST RENDER MESH
                            // DO NOT RENDER GROUP NOW
                            postRender = TRUE;
                            continue;
                        }

                        if (group->renderFlags)
                        {
                            // GROUP OPAQUE SO RENDER NOW
                            if (!renderStateSet)
                            {
                                Mesh_StoreTextureAndMat();
                                Mesh_SetMeshRenderDesc(mesh, vp, &matWorld, FALSE);
                                renderStateSet = TRUE;
                            }

                            Mesh_RenderGroup(mesh, group, &matWorld, FALSE);
                        }
                    }
                }

                if (postRender)
                    Mesh_AddToPostRenderList(mesh, &matWorld);
            } else
            {
                // NOTE IS SOME OF THE GROUPS IN THIS MESH USE ALPHA BLENDING THEY WILL NOT APPEAR
                // IF YOU SUSPECT THAT ONE OR MORE GROUPS WILL USE ALPHA BLENDING SET 'MESH_FLAG_RENDER_ALPHATRANS' ON THE MESH RRENDER DESC.
                Mesh_StoreTextureAndMat();
                Mesh_SetMeshRenderDesc(mesh, vp, &matWorld, FALSE);
                renderStateSet = TRUE;

                Mesh_RenderMesh(mesh, &matWorld, FALSE);
            }

            if (renderStateSet)
            {
                // RESTORE STATES
                Main_RestoreStates(FALSE);
                Mesh_RestoreTextureAndMat();
            }

            RELEASE(view);
        }

        return TRUE;
    }

    return FALSE;
}

B32 Mesh_RenderMesh(lpMesh mesh, LPD3DMATRIX matWorld, B32 alphaBlend)
{
    B32 ok = TRUE;
    lpMesh_Group group;
    U32 loop;

    // RENDER ALL GROUPS WITHOUT FLAGS CHANGED FIRST
    for (loop = 0; loop < mesh->groupCount; loop++)
    {
        group = &mesh->groupList[loop];

        if (Mesh_CanRenderGroup(group))
        {
            if (group->renderFlags == 0)
            {
                    if (!Mesh_RenderGroup(mesh, group, matWorld, alphaBlend))
                        ok = FALSE;
            }
        }
    }

    // RENDER ALL GROUPS WITH FLAGS CHANGED
    for (loop = 0; loop < mesh->groupCount; loop++)
    {
        group = &mesh->groupList[loop];

        if (Mesh_CanRenderGroup(group))
        {
            if (group->renderFlags)
            {
                if (!Mesh_RenderGroup(mesh, group, matWorld, alphaBlend))
                    ok = FALSE;
            }
        }
    }

    return ok;
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
    memcpy(newMesh->groupList, mesh->groupList, sizeof(Mesh_Group) * newMesh->groupCount);

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

#define MESH_UVREALLOCSIZE 10

void Mesh_HandleUViewUV(APPOBJ *lwo, Point2F textCoords[], B32* uvSet, U32* addedCount, U32 currPoly, U32 currPolyVertex, F32 u, F32 v)
{
    // Copy of the code from within the original Mesh_UViewMesh()

    U32 vertexIndex, newIndex;

    if (uvSet[lwo->aoPoly[currPoly].plyData[currPolyVertex]])
    {
        textCoords[lwo->aoPoly[currPoly].plyData[currPolyVertex]].x = u;
        textCoords[lwo->aoPoly[currPoly].plyData[currPolyVertex]].y = v;
        uvSet[lwo->aoPoly[currPoly].plyData[currPolyVertex]] = 1;
    } else if ((textCoords[lwo->aoPoly[currPoly].plyData[currPolyVertex]].x != u) ||
               (textCoords[lwo->aoPoly[currPoly].plyData[currPolyVertex]].y != v))
    {
        // THE SHARED VERTEX HAS MORE THAN ONE TEXTURE COORDINATE
        // COPY THE VERTEX AND ALTER THE FACE DATA FOR THE GROUP
        // ADD THE NEW UV INFORMATION TO THE NEW VERTEX

        // THIS ALTERS THE STRUCTURE OF 'lwo'
        // REFERENCES BEFORE THIS FUNCTION WILL BE DIFFERENT TO THOSE AFTER

        if (*addedCount == 0)
            lwo->aoVerts = Mem_ReAlloc(lwo->aoVerts, (sizeof(F32) * 3) * (lwo->aoSize.lwVertCount + MESH_UVREALLOCSIZE + 1));

        vertexIndex = lwo->aoPoly[currPoly].plyData[currPolyVertex] * 3;
        newIndex = lwo->aoSize.lwVertCount * 3;

        // ADD A COPY OF VERTEX TO LIST
        lwo->aoVerts[newIndex + 0] = lwo->aoVerts[vertexIndex + 0];
        lwo->aoVerts[newIndex + 1] = lwo->aoVerts[vertexIndex + 1];
        lwo->aoVerts[newIndex + 2] = lwo->aoVerts[vertexIndex + 2];

        // POINT THE FACE DATA TO POINT TO THE NEW VERTEX
        lwo->aoPoly[currPoly].plyData[currPolyVertex] = (U16)lwo->aoSize.lwVertCount;

        // STORE THE NEW UV INFORMATION
        textCoords[lwo->aoPoly[currPoly].plyData[currPolyVertex]].x = u;
        textCoords[lwo->aoPoly[currPoly].plyData[currPolyVertex]].y = v;

        lwo->aoSize.lwVertCount++;

        if (++(*addedCount) >= MESH_UVREALLOCSIZE)
            *addedCount = 0;
    }
}

void Mesh_UViewMeshV4(APPOBJ *lwo, Point2F textCoords[])
{
    char line[1024];
    char* argv[10];
    U32 argc;
    U32 surfaceCount, currSurface, polyCount, currPoly;
    U32 polyVertexCount, currPolyVertex, currChannel;
    B32 surfaceFileMode = FALSE;
    B32* uvSet;
    U32 addedCount = 0;

    uvSet = Mem_Alloc(sizeof(B32*) * lwo->aoSize.lwVertCount * 2);
    memset(uvSet, 0, sizeof(B32*) * lwo->aoSize.lwVertCount * 2);

    File_GetLine(line, sizeof(line), lwo->aoFileUV);
    while (File_GetLine(line, sizeof(line), lwo->aoFileUV))
    {
        if (surfaceFileMode)
        {
            if (currChannel == 0)
                lwo->aoSurface[currSurface].srfPath = Util_StrCpy(line);
            surfaceFileMode = FALSE;
        } else
        {
            if ((argc = Util_WSTokenize(line, argv)))
            {
                if (strcmp("VERSION", argv[0]) == 0)
                {
                    Error_Fatal(atol(argv[1]) != 4, "Incorrect UView file version");
                } else if (strcmp("SURFACE_COUNT", argv[0]) == 0)
                {
                    surfaceCount = atol(argv[1]);
                    Error_Fatal(lwo->aoSize.lwSurfaceCount != surfaceCount, "Invalid/Incorrect UView file (Surface count mismatch)");
                } else if (strcmp("SURFACE", argv[0]) == 0)
                {
                    currSurface = atol(argv[1]);
                } else if (strcmp("SURFACE_IMAGE_FILE", argv[0]) == 0)
                {
                    surfaceFileMode = TRUE;
                } else if (strcmp("POLY_COUNT", argv[0]))
                {
                    polyCount = atol(argv[1]);
                    Error_Fatal(lwo->aoSize.lwPolyCount != polyCount, "Invalid/Incorrect UView file (Poly count mismatch)");
                } else if (strcmp("POLY", argv[0]) == 0)
                {
                    currPoly = atol(argv[1]);
                } else if (strcmp("POLY_VERT_COUNT", argv[0]) == 0)
                {
                    polyVertexCount = atol(argv[1]);
                    Error_Fatal(lwo->aoPoly[currPoly].plyCount != polyVertexCount, "Invalid/Incorrect UView file (Poly vertex count mismatch)");
                } else if (strcmp("VERT", argv[0]) == 0)
                {
                    currPolyVertex = atol(argv[1]);
                } else if (strcmp("VERT_CUV", argv[0]) == 0)
                {
                    Mesh_HandleUViewUV(lwo, textCoords, uvSet, &addedCount, currPoly, currPolyVertex, (F32) atof(argv[2]), (F32) -atof(argv[3]));
                } else if (strcmp("CHANNEL", argv[0]) == 0)
                {
                    currChannel = atoi(argv[1]);
                }
            }
        }
    }

    Mem_Free(uvSet);
}

void Mesh_UViewMesh(APPOBJ* lightWaveObject, Point2F textCoords[])
{
    char line[1024];
    char lineSplit[1024];
    U32 groupID;
    char* argv[5];
    S32 argc, lineNum = 0;
    S32 polyRead = -2;
    S32 surfRead = -1;
    S32 vertexRead = -1;
    lpFile fileUV = lightWaveObject->aoFileUV;
    U32* uvSet;
    U32 addedCount = 0;
    B32 notSame;
    F32 uvX, uvY;
    U32 vertexIndex, newIndex;

    if (File_GetLine(line, sizeof(line), fileUV))
    {
        if (File_GetLine(line, sizeof(line), fileUV))
        {
            File_Seek(fileUV, 0, File_SeekSet);
            if (strcmp("VERSION 4", line) == 0)
            {
                Mesh_UViewMeshV4(lightWaveObject, textCoords);
                return;
            }
        }
    }

    uvSet = Mem_Alloc(sizeof(U32) * lightWaveObject->aoSize.lwVertCount * 2);
    memset(uvSet, 0, sizeof(U32) * lightWaveObject->aoSize.lwVertCount * 2);

    while (File_GetLine(line, sizeof(line), fileUV))
    {
        strcpy(lineSplit, line);

        if ((argc = Util_WSTokenize(lineSplit, argv)))
        {
            // MAYBE CHECK VERSION NUMBER ON FIRST LINE - SHOULD BE 2

            // CHECK SURFACE COUNT IS SAME AS OBJECT
            Error_Fatal(lineNum == 1 && lightWaveObject->aoSize.lwSurfaceCount != (U32)atoi(argv[0]), "UView file corrupt.");

            if (lineNum == 2)
                surfRead = 0;

            // READ SURFACE INFORMATION
            if (surfRead != -1)
            {
                if (surfRead >= (S32)(lightWaveObject->aoSize.lwSurfaceCount * 2))
                {
                    polyRead = -1;
                    surfRead = -1;
                } else if (surfRead >= (S32)lightWaveObject->aoSize.lwSurfaceCount)
                {
                    // OVERWRITE EXISTING IMAGE FILE NAME ON SURFACE
                    lightWaveObject->aoSurface[surfRead - lightWaveObject->aoSize.lwSurfaceCount].srfPath = Util_StrCpy(argv[0]);

                    surfRead++;
                } else
                {
                    surfRead++;
                }
            }

            // RUN THROUGH POLYGONS
            if (polyRead != -2)
            {
                if (polyRead == -1)
                {
                    // CHECK POLY COUNT IS THE SAME AS OBJECT
                    Error_Fatal((lightWaveObject->aoSize.lwPolyCount != (U32)atoi(argv[0])), "UView file corrupt.");
                    polyRead = 0;
                } else if (polyRead < (S32)lightWaveObject->aoSize.lwPolyCount)
                {
                    if (vertexRead != -1 && vertexRead < (S32)lightWaveObject->aoPoly[groupID].plyCount)
                    {
                        notSame = FALSE;

                        // READ UV INFORMATION FOR EACH VERTEX
                        uvX = (F32)atof(argv[0]);
                        uvY = -(F32)atof(argv[1]);

                        if ((textCoords[lightWaveObject->aoPoly[groupID].plyData[vertexRead]].x != uvX) ||
                            (textCoords[lightWaveObject->aoPoly[groupID].plyData[vertexRead]].y != uvY))
                        {
                            notSame = TRUE;
                        }

                        if (uvSet[lightWaveObject->aoPoly[groupID].plyData[vertexRead]] == 0)
                        {
                            textCoords[lightWaveObject->aoPoly[groupID].plyData[vertexRead]].x = uvX;
                            textCoords[lightWaveObject->aoPoly[groupID].plyData[vertexRead]].y = uvY;

                            uvSet[lightWaveObject->aoPoly[groupID].plyData[vertexRead]] = 1;
                        } else if (notSame)
                        {
                            // THE SHARED VERTEX HAS MORE THAN ONE TEXTURE COORDINATE
                            // COPY THE VERTEX AND ALTER THE FACE DATA FOR THE GROUP
                            // ADD THE NEW UV INFORMATION TO THE NEW VERTEX

                            // THIS ALTERS THE STRUCTURE OF 'lightWaveObject'
                            // REFERENCES BEFORE THIS FUNCTION WILL BE DIFFERENT TO THOSE AFTER

                            if (addedCount == 0)
                                lightWaveObject->aoVerts = Mem_ReAlloc(lightWaveObject->aoVerts, (sizeof(F32) * (lightWaveObject->aoSize.lwVertCount + MESH_UVREALLOCSIZE + 1) * 3));

                            vertexIndex = lightWaveObject->aoPoly[groupID].plyData[vertexRead] * 3;
                            newIndex = lightWaveObject->aoSize.lwVertCount * 3;

                            // ADD A COPY OF VERTEX TO LIST
                            lightWaveObject->aoVerts[newIndex] = lightWaveObject->aoVerts[vertexIndex];
                            lightWaveObject->aoVerts[newIndex + 1] = lightWaveObject->aoVerts[vertexIndex + 1];
                            lightWaveObject->aoVerts[newIndex + 2] = lightWaveObject->aoVerts[vertexIndex + 2];

                            // POINT THE FACE FATA TO POINT TO THE NEW VERTEX
                            lightWaveObject->aoPoly[groupID].plyData[vertexRead] = (U16)lightWaveObject->aoSize.lwVertCount;

                            // STORE THE NEW UV INFORMATION
                            textCoords[lightWaveObject->aoPoly[groupID].plyData[vertexRead]].x = uvX;
                            textCoords[lightWaveObject->aoPoly[groupID].plyData[vertexRead]].y = uvY;

                            lightWaveObject->aoSize.lwVertCount++;

                            if (++addedCount >= MESH_UVREALLOCSIZE)
                                addedCount = 0;
                        }

                        vertexRead++;
                    } else if (polyRead < (S32)lightWaveObject->aoSize.lwPolyCount - 1)
                    {
                        // CHECK POLY VERTEX COUNT IS SAME AS OBJECT
                        groupID = atoi(argv[0]);
                        Error_Fatal((lightWaveObject->aoPoly[groupID].plyCount != (U32)atoi(argv[1])), "UView file corrupt.");

                        if (vertexRead != -1)
                            polyRead++;

                        vertexRead = 0;
                    }
                } else
                {
                    polyRead = -2;

                    // RETURN HERE BECAUSE REST OF FILE IS UNUSED
                    return;
                }
            }

            lineNum++;
        }
    }

    Mem_Free(uvSet);
}

void Mesh_GetSurfInfo(const char* basePath, APPOBJ* lightWaveObject, Mesh_LightWave_Surface lightWaveSurf[], B32 noTextures)
{
    U32 loopSurf;
    char drive[_MAX_DRIVE];
    char dir[_MAX_DIR];
    char fname[_MAX_FNAME];
    char ext[_MAX_EXT];
    char path[_MAX_PATH];
    U32 tempNum;

    for (loopSurf = 0; loopSurf < lightWaveObject->aoSize.lwSurfaceCount; loopSurf++)
    {
        if (lightWaveObject->aoSurface[loopSurf].srfPath && !noTextures)
        {
            _splitpath(lightWaveObject->aoSurface[loopSurf].srfPath, drive, dir, fname, ext);

            if (lightWaveObject->aoSurface[loopSurf].srfTexFlags & TFM_SEQUENCE)
            {
                char baseName[_MAX_FNAME], textName[_MAX_FNAME];
                U32 textNum, numOfDigits, numInTexSeq = 0;

                // FIND NUM OF TEXTURES IN SEQUENCE
                if (Mesh_GetTextureSeqInfo(fname, baseName, &textNum, &numOfDigits))
                {
                    tempNum = textNum;

                    lightWaveSurf[loopSurf].textureSeq = Mem_Alloc(sizeof(lpMesh_Texture) * MESH_MAXTEXTURESEQUENCE);
                    for (numInTexSeq = 0; numInTexSeq < MESH_MAXTEXTURESEQUENCE; numInTexSeq++)
                    {
                        Mesh_GetNextInSequence(baseName, textName, &textNum, numOfDigits);
                        sprintf(path, "%s%s", textName, ext);
                        if ((lightWaveSurf[loopSurf].textureSeq[numInTexSeq] = Mesh_LoadTexture(basePath, path, NULL, NULL)) == NULL)
                            break;
                    }

                    // Mem_Realloc() will set 'textureSeq' to NULL if 'numInTexSeq' is zero...
                    lightWaveSurf[loopSurf].numInTexSeq = numInTexSeq;
                    lightWaveSurf[loopSurf].textureSeq = Mem_ReAlloc(lightWaveSurf[loopSurf].textureSeq, sizeof(lpMesh_Texture) * numInTexSeq);
                } else
                {
                    Error_Fatal(TRUE, Error_Format("Error loading texture sequence \"%s\".", fname));
                }
            } else
            {
                // LOAD SINGLE TEXTURE
                sprintf(path, "%s%s", fname, ext);

                lightWaveSurf[loopSurf].texture = Mesh_LoadTexture(basePath, path, NULL, NULL);
            }

            if (lightWaveObject->aoSurface[loopSurf].srfFlags & SFM_SHARPTERMINATOR)
            {
                // IF SHARP TERMINATOR FLAG IS SET THEN MIX TEXTURE COLOUR WITH DIFFUSE
                lightWaveSurf[loopSurf].colour.x = lightWaveObject->aoSurface[loopSurf].srfCol.colRed / 256.0f;
                lightWaveSurf[loopSurf].colour.y = lightWaveObject->aoSurface[loopSurf].srfCol.colGreen / 256.0f;
                lightWaveSurf[loopSurf].colour.z = lightWaveObject->aoSurface[loopSurf].srfCol.colBlue / 256.0f;
            } else
            {
                lightWaveSurf[loopSurf].colour.x = 1.0f;
                lightWaveSurf[loopSurf].colour.y = 1.0f;
                lightWaveSurf[loopSurf].colour.z = 1.0f;
            }

        } else {
            lightWaveSurf[loopSurf].colour.x = lightWaveObject->aoSurface[loopSurf].srfCol.colRed / 256.0f;
            lightWaveSurf[loopSurf].colour.y = lightWaveObject->aoSurface[loopSurf].srfCol.colGreen / 256.0f;
            lightWaveSurf[loopSurf].colour.z = lightWaveObject->aoSurface[loopSurf].srfCol.colBlue / 256.0f;
        }

        lightWaveSurf[loopSurf].emissive = lightWaveObject->aoSurface[loopSurf].srfLuminous;
        lightWaveSurf[loopSurf].transparency = lightWaveObject->aoSurface[loopSurf].srfTransparent;
        lightWaveSurf[loopSurf].diffuse = lightWaveObject->aoSurface[loopSurf].srfDiffuse;
        lightWaveSurf[loopSurf].specular = lightWaveObject->aoSurface[loopSurf].srfSpecular;
        lightWaveSurf[loopSurf].power = lightWaveObject->aoSurface[loopSurf].srfSpecPower;
        lightWaveSurf[loopSurf].flags = lightWaveObject->aoSurface[loopSurf].srfFlags;
        lightWaveSurf[loopSurf].texFlags = lightWaveObject->aoSurface[loopSurf].srfTexFlags;
    }
}

U32 Mesh_GetGroupCount(lpMesh mesh)
{
    return mesh->groupCount;
}

S32 Mesh_AddGroup(lpMesh mesh, U32 vertexCount, U32 faceCount, U32 vPerFace, U32* faceData)
{
    U32 size, loop;
    lpMesh_Group group;

    Mesh_Debug_CheckIMDevice_Int();
    Error_Fatal((vPerFace != 3), "Only triangles supported so far.");
    Error_Fatal((mesh->clonedFrom != NULL) || (mesh->flags & MESH_FLAG_HASBEENCLONED), "Cannot AddGroup() to a cloned mesh");

//    if (mesh->groupList == NULL)
//    {
//        mesh->listSize = MESH_DEFLISTSIZE;
//        mesh->groupCount = 1;
//        mesh->groupList = Mem_Alloc(mesh->listSize * sizeof(Mesh_Group));
//    } else if (++mesh->groupCount == mesh->listSize)
//    {
//        Mesh_Group* newList;
//        U32 newSize = (mesh->listSize * MESH_LISTINCREASE) / 100;
//
//        if (newList = Mem_ReAlloc(mesh->groupList, newSize * sizeof(Mesh_Group)))
//        {
//            mesh->groupList = newList;
//            mesh->listSize = newSize;
//        } else
//        {
//            Error_Fatal(TRUE, "Memory allocation error");
//
//            return -1;
//        }
//    }
//    group = &mesh->groupList[mesh->groupCount - 1];

    if (mesh->groupCount == mesh->listSize)
    {
        mesh->listSize = max(MESH_DEFLISTSIZE, (mesh->listSize * MESH_LISTINCREASE) / 100);
        mesh->groupList = Mem_ReAlloc(mesh->groupList, sizeof(mesh->groupList[0]) * mesh->listSize);
    }

    group = &mesh->groupList[mesh->groupCount++];
    memset(group, 0, sizeof(*group));

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

void Mesh_HideGroup(lpMesh mesh, U32 groupID, B32 hide)
{
    if (groupID < mesh->groupCount)
    {
        lpMesh_Group group;

        Mesh_Debug_CheckIMDevice_Void();
        group = &mesh->groupList[groupID];

        if (hide)
            group->flags |= MESH_FLAG_HIDDEN;
        else
            group->flags &= ~MESH_FLAG_HIDDEN;
    }
}

//void Mesh_SetAmbientLight(F32 r, F32 g, F32 b)
//{
//    if (r > 1.0f)
//        r = 1.0f;
//    if (g > 1.0f)
//        g = 1.0f;
//    if (b > 1.0f)
//        b = 1.0f;
//    if (r < 0.0f)
//        r = 0.0f;
//    if (g < 0.0f)
//        g = 0.0f;
//    if (b < 0.0f)
//        b = 0.0f;
//
//    meshGlobs.ambientLight = ((U32) (r * 255.0f) << 16) | ((U32) (g * 255.0f) << 8) | ((U32) (b * 255.0f));
//}

void Mesh_GetVertices(lpMesh mesh, U32 groupID, U32 index, U32 count, lpVertex vertices)
{
    if (groupID < mesh->groupCount)
    {
        U32 loop;
        lpMesh_Group group;

        Mesh_Debug_CheckIMDevice_Void();
        group = &mesh->groupList[groupID];

        for (loop = 0; loop < count; loop++)
        {
            vertices[loop].position = group->vertices[loop + index].position;
            vertices[loop].normal = group->vertices[loop + index].normal;
            vertices[loop].tu = group->vertices[loop + index].tu;
            vertices[loop].tv = group->vertices[loop + index].tv;
        }
    }
}

void Mesh_SetVertices(lpMesh mesh, U32 groupID, U32 index, U32 count, lpVertex vertices)
{
    if (groupID < mesh->groupCount)
    {
        U32 loop;
        lpMesh_Group group;

        Mesh_Debug_CheckIMDevice_Void();
        group = &mesh->groupList[groupID];

        for (loop = 0; loop < count; loop++)
        {
            group->vertices[loop + index].position = vertices[loop].position;
            group->vertices[loop + index].normal = vertices[loop].normal;
            group->vertices[loop + index].tu = vertices[loop].tu;
            group->vertices[loop + index].tv = vertices[loop].tv;
        }
    }
}

void Mesh_GetTextureUVsWrap(U32 vertexCount, Point3F vertices[], Point2F coords[], F32 sx, F32 sy, F32 sz, F32 px, F32 py, F32 pz, U32 flags)
{
    F32 x, y, z;
    U32 vertex;

    for (vertex = 0; vertex < vertexCount; vertex++)
    {
        x = vertices[vertex].x - px;
        y = vertices[vertex].y - py;
        z = vertices[vertex].z - pz;

        coords[vertex].x = (flags & TFM_AXIS_X) ? (z / sz) + 0.5f : (x / sx) + 0.5f;
        coords[vertex].y = (flags & TFM_AXIS_Y) ? (z / sz) + 0.5f : (y / sy) + 0.5f;
    }
}

void Mesh_SetVertices_VNT(lpMesh mesh, U32 groupID, U32 index, U32 count, Point3F vertices[], lpPoint3F normal[], Point2F textCoords[])
{
    if (groupID < mesh->groupCount)
    {
        U32 loop;
        lpMesh_Group group;

        Mesh_Debug_CheckIMDevice_Void();

        group = &mesh->groupList[groupID];

        for (loop = 0; loop < count; loop++)
        {
            group->vertices[loop + index].position.x = vertices[loop].x;
            group->vertices[loop + index].position.y = vertices[loop].y;
            group->vertices[loop + index].position.z = vertices[loop].z;
            group->vertices[loop + index].normal.x = normal[loop]->x;
            group->vertices[loop + index].normal.y = normal[loop]->y;
            group->vertices[loop + index].normal.z = normal[loop]->z;
            group->vertices[loop + index].tu = textCoords[loop].x;
            group->vertices[loop + index].tv = textCoords[loop].y;
        }
    }
}

void Mesh_AlterGroupRenderFlags(lpMesh mesh, U32 groupID, U32 newFlags)
{
    if (groupID < mesh->groupCount)
        mesh->groupList[groupID].renderFlags = newFlags;
}

void Mesh_SetGroupTexture(lpMesh mesh, U32 groupID, lpMesh_Texture mt)
{
    if (groupID < mesh->groupCount)
    {
        Mesh_Debug_CheckIMDevice_Void();

        if (mt)
        {
            if (mt->surface)
                mt->surface->lpVtbl->QueryInterface(mt->surface, &IID_IDirect3DTexture2, &mesh->groupList[groupID].imText);
            if (mt->colourKey)
                mesh->groupList[groupID].flags |= MESH_FLAG_TRANSTEXTURE;
            else
                mesh->groupList[groupID].flags &= ~MESH_FLAG_TRANSTEXTURE;
        }
    }
}

LPD3DMATERIAL Mesh_GetGroupMaterial(lpMesh mesh, U32 groupID)
{
    if (groupID < mesh->groupCount)
    {
        Mesh_Debug_CheckIMDevice_Ptr();

        return &mesh->groupList[groupID].material;
    }

    return NULL;
}

B32 Mesh_SetGroupColour(lpMesh mesh, U32 groupID, F32 r, F32 g, F32 b, U32 type)
{
    if (groupID < mesh->groupCount)
    {
        lpMesh_Group group = &mesh->groupList[groupID];
        LPD3DMATERIAL material;

        Mesh_Debug_CheckIMDevice_Int();

        material = Mesh_GetGroupMaterial(mesh, groupID);

        if (r < 0.0f)
            r = 0.0f;
        if (g < 0.0f)
            g = 0.0f;
        if (b < 0.0f)
            b = 0.0f;
        if (r > 1.0f)
            r = 1.0f;
        if (g > 1.0f)
            g = 1.0f;
        if (b > 1.0f)
            b = 1.0f;

        if (type == Mesh_Colour_Diffuse)
        {
            material->diffuse.r = r;
            material->diffuse.g = g;
            material->diffuse.b = b;

//            if (r == 1.0f && g == 1.0f && b == 1.0f)
//                group->flags |= MESH_FLAG_TEXTURECOLOURONLY;
//            else
//                group->flags &= ~MESH_FLAG_TEXTURECOLOURONLY;
        }
        else if (type == Mesh_Colour_Ambient)
        {
            material->ambient.r = r;
            material->ambient.g = g;
            material->ambient.b = b;
        }
        else if (type == Mesh_Colour_Emissive)
        {
            material->emissive.r = r;
            material->emissive.g = g;
            material->emissive.b = b;
        }
        else if (type == Mesh_Colour_Specular)
        {
            material->specular.r = r;
            material->specular.g = g;
            material->specular.b = b;
        }

        return TRUE;
    }

    return FALSE;
}

void Mesh_GetGroupColour(lpMesh mesh, U32 groupID, F32* r, F32* g, F32* b, U32 type)
{
    if (groupID < mesh->groupCount)
    {
        lpMesh_Group group = &mesh->groupList[groupID];
        LPD3DMATERIAL material;

        Mesh_Debug_CheckIMDevice_Void();

        material = Mesh_GetGroupMaterial(mesh, groupID);

        if (type == Mesh_Colour_Diffuse)
        {
            *r = material->diffuse.r;
            *g = material->diffuse.g;
            *b = material->diffuse.b;
        }
        else if (type == Mesh_Colour_Ambient)
        {
            *r = material->ambient.r;
            *g = material->ambient.g;
            *b = material->ambient.b;
        }
        else if (type == Mesh_Colour_Emissive)
        {
            *r = material->emissive.r;
            *g = material->emissive.g;
            *b = material->emissive.b;
        }
        else if (type == Mesh_Colour_Specular)
        {
            *r = material->specular.r;
            *g = material->specular.g;
            *b = material->specular.b;
        }
    }
}

B32 Mesh_SetGroupMaterialValues(lpMesh mesh, U32 groupID, F32 value, U32 type)
{
    if (groupID < mesh->groupCount)
    {
        lpMesh_Group group = &mesh->groupList[groupID];
        LPD3DMATERIAL material;

        Mesh_Debug_CheckIMDevice_Int();

        material = Mesh_GetGroupMaterial(mesh, groupID);

        if (value < 0.0f)
            value = 0.0f;
        if (value > 1.0f)
            value = 1.0f;

        if (type == Mesh_Colour_Alpha)
        {
            group->flags &= ~MESH_FLAG_ALPHAHIDDEN;
            group->flags &= ~MESH_FLAG_ALPHAENABLE;

            if (value == 0.0f)
            {
                if (group->renderFlags)
                {
                    if (group->renderFlags & MESH_FLAG_RENDER_ALLALPHA)
                        group->flags |= MESH_FLAG_ALPHAHIDDEN;
                }
                else if (mesh->renderDesc.renderFlags & MESH_FLAG_RENDER_ALLALPHA)
                    group->flags |= MESH_FLAG_ALPHAHIDDEN;
            }
            else if (value != 1.0f)
            {
                if (group->renderFlags)
                {
                    if (group->renderFlags & MESH_FLAG_RENDER_ALLALPHA)
                        group->flags |= MESH_FLAG_ALPHAENABLE;
                }
                else if (mesh->renderDesc.renderFlags & MESH_FLAG_RENDER_ALLALPHA)
                    group->flags |= MESH_FLAG_ALPHAENABLE;
            }

            // COLOUR ADDITION CAN OCCUR EVEN IF ALPHA VALUE IS 1.0f
            if (group->renderFlags)
            {
                if ((mesh->renderDesc.renderFlags & MESH_FLAG_RENDER_ALPHA11) ||
                    (mesh->renderDesc.renderFlags & MESH_FLAG_RENDER_ALPHASA1) ||
                    (mesh->renderDesc.renderFlags & MESH_FLAG_RENDER_ALPHASA0))
                {
                    group->flags |= MESH_FLAG_ALPHAENABLE;
                }
            }
            else if ((mesh->renderDesc.renderFlags & MESH_FLAG_RENDER_ALPHA11) ||
                     (mesh->renderDesc.renderFlags & MESH_FLAG_RENDER_ALPHASA1) ||
                     (mesh->renderDesc.renderFlags & MESH_FLAG_RENDER_ALPHASA0))
            {
                group->flags |= MESH_FLAG_ALPHAENABLE;
            }

            material->diffuse.a = value;
        }
        else if (type == Mesh_Colour_Power)
        {
            material->power = value;
        }

        return TRUE;
    }

    return FALSE;
}

void Mesh_GetGroupMaterialValues(lpMesh mesh, U32 groupID, F32* value, U32 type)
{
    if (groupID < mesh->groupCount)
    {
        lpMesh_Group group = &mesh->groupList[groupID];

        LPD3DMATERIAL material;

        Mesh_Debug_CheckIMDevice_Void();

        material = Mesh_GetGroupMaterial(mesh, groupID);

        if (type == Mesh_Colour_Alpha)
            *value = material->diffuse.a;
        else if (type == Mesh_Colour_Power)
            *value = material->power;
    }
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

void Mesh_SetIdentityMatrix(Matrix4F m)
{
    m[0][1] = m[0][2] = m[0][3] = m[1][0] = m[1][2] = m[1][3] = 0.0f;
    m[2][0] = m[2][1] = m[2][3] = m[3][0] = m[3][1] = m[3][2] = 0.0f;
    m[0][0] = m[1][1] = m[2][2] = m[3][3] = 1.0f;
}

B32 Mesh_SetTransform(D3DTRANSFORMSTATETYPE state, Matrix4F* matrix)
{
    D3DMATRIX oldMatrix;
    D3DMATRIX newMatrix = *((LPD3DMATRIX)matrix);

    // DIRECTX DOCUMENTATION STATES CALLS TO 'SetTransform' SHOULD BE MINIMIZED
    if (lpIMDevice()->lpVtbl->GetTransform(lpIMDevice(), state, &oldMatrix) != D3D_OK)
        Error_Warn(TRUE, "Cannot 'GetTransform'.");

    // MAYBE PUT != COMPARISON AS A GLOBAL FUNCTION?
    if ((oldMatrix._11 != newMatrix._11) || (oldMatrix._12 != newMatrix._12) || (oldMatrix._13 != newMatrix._13) || (oldMatrix._14 != newMatrix._14) ||
        (oldMatrix._21 != newMatrix._21) || (oldMatrix._22 != newMatrix._22) || (oldMatrix._23 != newMatrix._23) || (oldMatrix._24 != newMatrix._24) ||
        (oldMatrix._31 != newMatrix._31) || (oldMatrix._32 != newMatrix._32) || (oldMatrix._33 != newMatrix._33) || (oldMatrix._34 != newMatrix._34) ||
        (oldMatrix._41 != newMatrix._41) || (oldMatrix._42 != newMatrix._42) || (oldMatrix._43 != newMatrix._43) || (oldMatrix._44 != newMatrix._44))
    {
        if (lpIMDevice()->lpVtbl->SetTransform(lpIMDevice(), state, (LPD3DMATRIX)matrix))
        {
            Error_Warn(TRUE, "Cannot 'SetTransform'.");

            return FALSE;
        }
    }

    return TRUE;
}

B32 Mesh_SetTextureTime(lpMesh mesh, F32 frame)
{
    if (mesh->textureRenderCallback == NULL)
        return Mesh_SetTextureTime2(mesh, frame);

    return FALSE;
}

B32 Mesh_SetTextureTime2(lpMesh mesh, F32 frame)
{
    U32 texNum;
    U32 groupNum;
    S32 groupTexNum;
    lpMesh_Group group;

    texNum = (U32)(frame - (F32)fmod(frame, 1.0f));

    if (mesh->flags & MESH_FLAG_LWO)
    {
        for (groupNum = 0; groupNum < mesh->groupCount; groupNum++)
        {
            group = &mesh->groupList[groupNum];

            if ((group->lightWaveSurfaceInfo->texFlags & TFM_SEQUENCE) && (group->lightWaveSurfaceInfo->numInTexSeq))
            {
                groupTexNum = (group->lightWaveSurfaceInfo->texSeqOffset + (S32)texNum) % (S32) group->lightWaveSurfaceInfo->numInTexSeq;

                if (groupTexNum < 0)
                    groupTexNum = 0;

                Mesh_SetGroupTexture(mesh, groupNum, group->lightWaveSurfaceInfo->textureSeq[groupTexNum]);
            }
        }

        return TRUE;
    } else
    {
        Error_Warn(TRUE, "Cannot set texture time on non LightWave mesh.");
    }

    return FALSE;
}

B32 Mesh_ChangeTextureStageState(D3DTEXTURESTAGESTATETYPE dwRenderStateType, U32 dwRenderState)
{
    lpMesh_TextureStateChangeData data;
    U32 currValue;

    Error_Fatal(dwRenderStateType >= MESH_MAXTEXTURESTAGESTATES, "TextureStageState type is out of range");

    data = &meshGlobs.stateData[dwRenderStateType];
    if (lpIMDevice()->lpVtbl->GetTextureStageState(lpIMDevice(), 0, dwRenderStateType, &currValue) != D3D_OK)
    {
        Error_Warn(TRUE, "Cannot 'GetTextureStageState'.");

        currValue = -1;
    }

    if (currValue != dwRenderState)
    {
        if (lpIMDevice()->lpVtbl->SetTextureStageState(lpIMDevice(), 0, dwRenderStateType, dwRenderState) != D3D_OK)
        {
            Error_Warn(TRUE, "Cannot 'SetTextureStageState'.");

            return FALSE;
        }
        else
        {
            if (data->changed)
            {
                if (data->origValue == currValue)
                    data->changed = FALSE;
            }
            else
            {
                data->origValue = currValue;
                data->changed = TRUE;
            }
        }
    }
    return TRUE;
}

void Mesh_SetAlphaRender(D3DBLEND src, D3DBLEND dest)
{
    Main_ChangeRenderState(D3DRENDERSTATE_ALPHABLENDENABLE, TRUE);
    Main_ChangeRenderState(D3DRENDERSTATE_SRCBLEND, src);
    Main_ChangeRenderState(D3DRENDERSTATE_DESTBLEND, dest);
}

B32 Mesh_CanRenderGroup(lpMesh_Group group)
{
    return (!(group->flags & MESH_FLAG_HIDDEN) && !(group->flags & MESH_FLAG_ALPHAHIDDEN));
}

B32 Mesh_RenderGroup(lpMesh mesh, lpMesh_Group group, LPD3DMATRIX matWorld, B32 alphaBlend)
{
    B32 ok = TRUE;
    S32 frame;

    Mesh_SetGroupRenderDesc(mesh, group, matWorld, alphaBlend);

    if (!Mesh_SetMaterial(&group->material))
        ok = FALSE;

    if (mesh->textureRenderCallback)
    {
        frame = (mesh->textureRenderCallback(mesh->textureRenderCallbackData));
        if (frame != -1)
            Mesh_SetTextureTime2(mesh, (F32) frame);
    }

    if (!Mesh_RenderTriangleList(meshGlobs.matHandle, group->imText, MESH_DEFAULTRENDERFLAGS,
                                 group->vertices, group->vertexCount, group->faceData, group->faceDataSize))
    {
        ok = FALSE;
    }

    return ok;
}

B32 Mesh_SetGroupRenderDesc(lpMesh mesh, lpMesh_Group group, LPD3DMATRIX matWorld, B32 alphaBlend)
{
    if (!(mainGlobs.flags & MAIN_FLAG_DONTMANAGETEXTURES))
    {
        if ((group->flags & MESH_FLAG_TEXTURECOLOURONLY) && group->imText)
            Mesh_ChangeTextureStageState(D3DTSS_COLOROP, D3DTOP_SELECTARG1);
        else
            Mesh_ChangeTextureStageState(D3DTSS_COLOROP, D3DTOP_MODULATE);
    }

    if (group->flags & MESH_FLAG_TRANSTEXTURE)
        Main_ChangeRenderState(D3DRENDERSTATE_COLORKEYENABLE, TRUE);
    else
        Main_ChangeRenderState(D3DRENDERSTATE_COLORKEYENABLE, FALSE);

    if (group->renderFlags)
    {
        Mesh_SetRenderDesc(group->renderFlags, matWorld, alphaBlend);

        return TRUE;
    }

    return FALSE;
}

void Mesh_SetRenderDesc(U32 flags, LPD3DMATRIX matWorld, B32 alphaBlend)
{
    Matrix4F temp;

    // CECK MESH IS ALPHA BLENDED
    if ((flags & MESH_FLAG_RENDER_ALLALPHA) && alphaBlend)
        Main_ChangeRenderState(D3DRENDERSTATE_FOGENABLE, FALSE); // Don't fog alpha effects...

    // ALPHA STATES
    if((flags & MESH_FLAG_RENDER_ALLALPHA) && !alphaBlend)
        Main_ChangeRenderState(D3DRENDERSTATE_ALPHABLENDENABLE, FALSE);
    else if (flags & MESH_FLAG_RENDER_ALPHA11)
        Mesh_SetAlphaRender(D3DBLEND_ONE, D3DBLEND_ONE);
    else if (flags & MESH_FLAG_RENDER_ALPHASA1)
        Mesh_SetAlphaRender(D3DBLEND_SRCALPHA, D3DBLEND_ONE);
    else if (flags & MESH_FLAG_RENDER_ALPHATRANS)
        Mesh_SetAlphaRender(D3DBLEND_SRCALPHA, D3DBLEND_INVSRCALPHA);
    else if (flags & MESH_FLAG_RENDER_ALPHASA0)
        Mesh_SetAlphaRender(D3DBLEND_ZERO, D3DBLEND_INVSRCCOLOR);
    else
        Main_ChangeRenderState(D3DRENDERSTATE_ALPHABLENDENABLE, FALSE);

    if (flags & MESH_FLAG_RENDER_DOUBLESIDED)
        Main_ChangeRenderState(D3DRENDERSTATE_CULLMODE, D3DCULL_NONE);
    else
        Main_ChangeRenderState(D3DRENDERSTATE_CULLMODE, D3DCULL_CCW);

    if (!(mainGlobs.flags & MAIN_FLAG_DONTMANAGETEXTURES))
    {
        // ALPHA CHANNEL
        if (flags & MESH_FLAG_RENDER_ALPHATEX)
            Mesh_ChangeTextureStageState(D3DTSS_ALPHAOP, D3DTOP_SELECTARG1);
        else if (flags & MESH_FLAG_RENDER_ALPHADIFFUSE)
            Mesh_ChangeTextureStageState(D3DTSS_ALPHAOP, D3DTOP_SELECTARG2);
        else
            Mesh_ChangeTextureStageState(D3DTSS_ALPHAOP, D3DTOP_MODULATE);
    }

    // Z BUFFER CHECK
    Main_ChangeRenderState(D3DRENDERSTATE_ZENABLE, TRUE);

    // Z BUFFER WRITE
    if ((flags & MESH_FLAG_RENDER_ALLALPHA) && alphaBlend)
        Main_ChangeRenderState(D3DRENDERSTATE_ZWRITEENABLE, FALSE);
    else
        Main_ChangeRenderState(D3DRENDERSTATE_ZWRITEENABLE, TRUE);

    // WORLD TRANSFORMATION
    if (flags & MESH_FLAG_TRANSFORM_PARENTPOS)
        Mesh_SetTransform(D3DTRANSFORMSTATE_WORLD, (Matrix4F*)matWorld);
    else if (flags & MESH_FLAG_TRANSFORM_IDENTITY)
    {
        Mesh_SetIdentityMatrix(temp);
        Mesh_SetTransform(D3DTRANSFORMSTATE_WORLD, &temp);
    }

    if (flags & MESH_FLAG_RENDER_FILTERNEAREST)
    {
        Main_ChangeRenderState(D3DRENDERSTATE_TEXTUREMAG, D3DFILTER_NEAREST);
        Main_ChangeRenderState(D3DRENDERSTATE_TEXTUREMIN, D3DFILTER_NEAREST);
    } else
    {
        Main_ChangeRenderState(D3DRENDERSTATE_TEXTUREMAG, D3DFILTER_LINEAR);
        Main_ChangeRenderState(D3DRENDERSTATE_TEXTUREMIN, D3DFILTER_MIPLINEAR);
    }
}

B32 Mesh_RenderTriangleList(D3DMATERIALHANDLE matHandle, LPDIRECT3DTEXTURE2 texture, U32 renderFlags, Mesh_Vertex vertices[], U32 vertexCount, U16 faceData[], U32 indexCount)
{
    B32 ok = TRUE;
    U32 textureHandle;
    LPDIRECT3DDEVICE2 device2;
    U32 oldAmbientLightState;

    // TEXTURES
    if (!(mainGlobs.flags & MAIN_FLAG_DONTMANAGETEXTURES))
    {
        if (texture != meshGlobs.currTextureIM)
        {
            // SET NEW TEXTURE
            if (lpIMDevice()->lpVtbl->SetTexture(lpIMDevice(), 0, texture))
            {
                Error_Warn(TRUE, "Cannot 'SetTexture' for new D3DIM texture.");
                ok = FALSE;
            } else
            {
                meshGlobs.currTextureIM = texture;
            }
        }
    } else
    {
        textureHandle = 0;

        if (texture)
        {
            // QUERY INTERFACE FOR OLD DEVICE AND GET HANDLE
            lpIMDevice()->lpVtbl->QueryInterface(lpIMDevice(), &IID_IDirect3DDevice2, &device2);
            texture->lpVtbl->GetHandle(texture, device2, &textureHandle);
            RELEASE(device2);
        }

        if (textureHandle != meshGlobs.currTextureRM)
        {
            // SET NEW TEXTURE
            Main_ChangeRenderState(D3DRENDERSTATE_TEXTUREHANDLE, textureHandle);
            meshGlobs.currTextureRM = textureHandle;
        }
    }

    // MATERIAL
    if (matHandle != meshGlobs.currMatIM)
    {
        // SET NEW MATERIAL
        if (lpIMDevice()->lpVtbl->SetLightState(lpIMDevice(), D3DLIGHTSTATE_MATERIAL, matHandle) != D3D_OK)
        {
            Error_Warn(TRUE, "Cannot 'SetLightState' for new D3DIM material.");
            ok = FALSE;
        } else
        {
            meshGlobs.currMatIM = matHandle;
        }
    }

    //lpIMDevice()->lpVtbl->GetLightState(lpIMDevice(), D3DLIGHTSTATE_AMBIENT, &oldAmbientLightState);
    //lpIMDevice()->lpVtbl->SetLightState(lpIMDevice(), D3DLIGHTSTATE_AMBIENT, meshGlobs.ambientLight);

    // RENDER PRIMITIVE
    if (lpIMDevice()->lpVtbl->DrawIndexedPrimitive(lpIMDevice(), D3DPT_TRIANGLELIST, renderFlags, vertices, vertexCount, faceData, indexCount, 0) != D3D_OK)
    {
        Error_Warn(TRUE, "Cannot 'DrawIndexedPrimitive'.");
        ok = FALSE;
    }

    //lpIMDevice()->lpVtbl->SetLightState(lpIMDevice(), D3DLIGHTSTATE_AMBIENT, oldAmbientLightState);

    return ok;
}

void Mesh_SetMeshRenderDesc(lpMesh mesh, lpViewport vp, LPD3DMATRIX matWorld, B32 alphaBlend)
{
    // CALL MESH SPECIFIC RENDER FUNCTION
    if (mesh->renderDesc.renderCallback)
        mesh->renderDesc.renderCallback(mesh, mesh->renderDesc.renderCallbackData, vp);

    // ADD DEFAULT STATES HERE
    Main_ChangeRenderState(D3DRENDERSTATE_SPECULARENABLE, FALSE);
    Main_ChangeRenderState(D3DRENDERSTATE_SHADEMODE, D3DSHADE_GOURAUD);
    Main_ChangeRenderState(D3DRENDERSTATE_DITHERENABLE, TRUE);

    if (!(mainGlobs.flags & MAIN_FLAG_DONTMANAGETEXTURES))
    {
        Mesh_ChangeTextureStageState(D3DTSS_COLORARG1, D3DTA_TEXTURE);
        Mesh_ChangeTextureStageState(D3DTSS_COLORARG2, D3DTA_DIFFUSE);
        Mesh_ChangeTextureStageState(D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
        Mesh_ChangeTextureStageState(D3DTSS_ALPHAARG2, D3DTA_DIFFUSE);
    }

    Mesh_SetRenderDesc(mesh->renderDesc.renderFlags, matWorld, alphaBlend);
}

B32 Mesh_SetCurrentViewport(LPDIRECT3DRMVIEWPORT view)
{
    LPDIRECT3DVIEWPORT imview1;
    LPDIRECT3DVIEWPORT3 imview;

    if (view->lpVtbl->GetDirect3DViewport(view, &imview1) != D3DRM_OK)
    {
        Error_Warn(TRUE, "Cannot 'GetDirect3DViewport'.");
        return FALSE;
    }

    if (imview1->lpVtbl->QueryInterface(imview1, &IID_IDirect3DViewport3, &imview) != D3DRM_OK)
    {
        Error_Warn(TRUE, "Cannot 'QueryInterface' 'IID_IDirect3DViewport3'.");
        RELEASE(imview1);
        return FALSE;
    }

    RELEASE(imview1);

    if (lpIMDevice()->lpVtbl->SetCurrentViewport(lpIMDevice(), imview) != D3D_OK)
    {
        Error_Warn(TRUE, "Cannot 'SetCurrentViewport'.");
        RELEASE(imview);
        return FALSE;
    }

    RELEASE(imview);

    return TRUE;
}

B32 Mesh_SetCurrentGODSViewport(lpViewport vp)
{
    LPDIRECT3DVIEWPORT imview1;
    LPDIRECT3DVIEWPORT3 imview;

    if (vp->lpVP->lpVtbl->GetDirect3DViewport(vp->lpVP, &imview1) != D3DRM_OK)
    {
        Error_Warn(TRUE, "Cannot 'GetDirect3DViewport'.");
        return FALSE;
    }

    if (imview1->lpVtbl->QueryInterface(imview1, &IID_IDirect3DViewport3, &imview))
    {
        Error_Warn(TRUE, "Cannot 'QueryInterface' 'IID_IDirect3DViewport3'.");
        RELEASE(imview1);
        return FALSE;
    }

    RELEASE(imview1);

    if (lpIMDevice()->lpVtbl->SetCurrentViewport(lpIMDevice(), imview) != D3D_OK)
    {
        Error_Warn(TRUE, "Cannot 'SetCurrentViewport'.");
        RELEASE(imview);
        return FALSE;
    }

    RELEASE(imview);

    return TRUE;
}

void Mesh_StoreTextureAndMat()
{
    B32 ok = TRUE;

    // GET OLD MATERIAL
    if (lpIMDevice()->lpVtbl->GetLightState(lpIMDevice(), D3DLIGHTSTATE_MATERIAL, &meshGlobs.oldMatIM) != D3D_OK)
    {
        Error_Warn(TRUE, "Cannot 'GetLightState' for current D3DIM material.");

        ok = FALSE;
    }

    meshGlobs.currMatIM = meshGlobs.oldMatIM;

    if (!(mainGlobs.flags & MAIN_FLAG_DONTMANAGETEXTURES))
    {
        // GET OLD TEXTURE
        if (lpIMDevice()->lpVtbl->GetTexture(lpIMDevice(), 0, &meshGlobs.oldTextureIM) != D3D_OK)
        {
            Error_Warn(TRUE, "Cannot 'GetTexture' for current D3DIM texture.");

            ok = FALSE;
        }
        meshGlobs.currTextureIM = meshGlobs.oldTextureIM;

        // GET OLD RM TEXTURE
        lpIMDevice()->lpVtbl->GetRenderState(lpIMDevice(), D3DRENDERSTATE_TEXTUREHANDLE, &meshGlobs.oldTextureRM);

        // CLEAR RETAINED MODE TEXTURE HANDLE
        if (meshGlobs.oldTextureIM != 0)
            lpIMDevice()->lpVtbl->SetRenderState(lpIMDevice(), D3DRENDERSTATE_TEXTUREHANDLE, 0);
    } else {
        lpIMDevice()->lpVtbl->GetRenderState(lpIMDevice(), D3DRENDERSTATE_TEXTUREHANDLE, &meshGlobs.oldTextureRM);
        meshGlobs.currTextureRM = meshGlobs.oldTextureRM;
    }
}

void Mesh_RestoreTextureAndMat()
{
    B32 ok = TRUE;

    if (meshGlobs.currMatIM != meshGlobs.oldMatIM)
    {
        // SET OLD MATERIAL
        if (lpIMDevice()->lpVtbl->SetLightState(lpIMDevice(), D3DLIGHTSTATE_MATERIAL, meshGlobs.oldMatIM) != D3D_OK)
        {
            Error_Warn(TRUE, "Cannot 'SetLightState' for old D3DIM material.");

            ok = FALSE;
        }

        if (!(mainGlobs.flags & MAIN_FLAG_DONTMANAGETEXTURES))
        {
            if (meshGlobs.currTextureIM != meshGlobs.oldTextureIM)
            {
                // SET OLD TEXTURE
                if (lpIMDevice()->lpVtbl->SetTexture(lpIMDevice(), 0, meshGlobs.oldTextureIM) != D3D_OK)
                {
                    Error_Warn(TRUE, "Cannot 'SetTexture' for old D3DIM texture.");

                    ok = FALSE;
                }
            }

            // SET OLD RETAINED MODE TEXTURE
            if (meshGlobs.oldTextureRM != 0)
                lpIMDevice()->lpVtbl->SetRenderState(lpIMDevice(), D3DRENDERSTATE_TEXTUREHANDLE, meshGlobs.oldTextureRM);
        } else
        {
            lpIMDevice()->lpVtbl->SetRenderState(lpIMDevice(), D3DRENDERSTATE_TEXTUREHANDLE, meshGlobs.oldTextureRM);
        }
    }
}

B32 Mesh_GetTextureSeqInfo(const char* tname, char* tfname, U32* tstart, U32* tnumlen)
{
    U32 len, n, indx = 1, val = 0;
    char c;

    len = strlen(tname);
    c = tname[len - 1];
    if ((c < '0') || (c > '9'))
        return FALSE;

    for (n = len - 1; n > 0; n--)
    {
        c = tname[n];
        if ((c < '0') || (c > '9'))
            break;
        else
            val += (c - '0') * indx;

        indx *= 10;
    }

    if (n == 0)
        return FALSE;

    strcpy(tfname, tname);
    tfname[n + 1] = 0;
    *tnumlen = len - n - 1;
    *tstart = val;

    return TRUE;
}

void Mesh_GetNextInSequence(const char* baseName, const char* nextTextName, U32* texNum, U32 tnumlen)
{
    char numBuff[16];
    U32 k;

    strcpy(nextTextName, baseName);

    sprintf(numBuff, "%d", (*texNum));
    k = tnumlen - strlen(numBuff);
    while (k >= 1)
    {
        strcat(nextTextName, "0");
        k--;
    }

    (*texNum)++;

    strcat(nextTextName, numBuff);
}

lpMesh_Texture Mesh_LoadTexture(const char* baseDir, const char* name, U32* width, U32* height)
{
    char path[FILE_MAXPATH];
    lpMesh_Texture texture = Mem_Alloc(sizeof(Container_Texture));
    LPDIRECTDRAWSURFACE4 surface = NULL;
    lpMesh_TextureReference ref;
    B32 trans;

    Mesh_Debug_CheckIMDevice_Ptr();

    if (baseDir)
        sprintf(path, "%s%s", baseDir, name);
    else
        strcpy(path, name);

    if ((ref = Mesh_SearchTexturePathList(meshGlobs.textureList, meshGlobs.textureCount, path)))
    {
        if ((surface = ref->surface))
            surface->lpVtbl->AddRef(surface);
        trans = ref->trans;
    } else if ((surface = Container_LoadTextureSurface(path, TRUE, width, height, &trans)))
    {
        Mesh_AddTexturePathEntry(meshGlobs.textureList, &meshGlobs.textureCount, path, surface, trans);
    } else if (meshGlobs.sharedTextureDir)
    {
        sprintf(path, "%s%s", meshGlobs.sharedTextureDir, name);

        if ((ref = Mesh_SearchTexturePathList(meshGlobs.textureListShared, meshGlobs.textureCountShared, path)))
        {
            if ((surface = ref->surface))
                surface->lpVtbl->AddRef(surface);
            trans = ref->trans;
        } else if ((surface = Container_LoadTextureSurface(path, TRUE, width, height, &trans)))
        {
            Mesh_AddTexturePathEntry(meshGlobs.textureListShared, &meshGlobs.textureCountShared, path, surface, trans);
        }
    }

    if (surface)
    {
        texture->texture = NULL;
        texture->surface = surface;
        texture->colourKey = trans;
    } else
    {
        Mem_Free(texture);
        texture = NULL;
        //Error_Fatal(TRUE, Error_Format("Cannot find or load texture >(%s\\)%s<.", baseDir, name));
    }

    return texture;
}

lpMesh_TextureReference Mesh_SearchTexturePathList(lpMesh_TextureReference list, U32 count, const char* path)
{
    U32 loop;

    for (loop = 0; loop < count; loop++)
    {
        if (stricmp(path, list[loop].path) == 0)
            return &list[loop];
    }

    return NULL;
}

void Mesh_AddTexturePathEntry(lpMesh_TextureReference list, U32* count, const char* path, LPDIRECTDRAWSURFACE4 surface, B32 trans)
{
    Error_Fatal(*count == MESH_TEXTURELISTSIZE, "MESH_TEXTURELISTSIZE too small");

    list[*count].path = Util_StrCpy(path);
    list[*count].surface = surface;
    list[*count].trans = trans;
    (*count)++;
}

void Mesh_RemoveGroupTexture(lpMesh mesh, U32 groupID)
{
    if (groupID < mesh->groupCount)
    {
        lpMesh_Group group = &mesh->groupList[groupID];

        Mesh_Debug_CheckIMDevice_Void();

        if (group->imText)
        {
            RELEASE(group->imText);

            group->imText = NULL;
        }
    }
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
    U32 loop;
    lpMesh_Group group;

    Mesh_Debug_CheckIMDevice_Void();

    frame->lpVtbl->DeleteVisual(frame, (struct IUnknown*) mesh->uv);

    if (mesh->clonedFrom)
    {
        lpMesh clonedFrom = mesh->clonedFrom;

        RELEASE(mesh->uv);
        for (loop = 0; loop < mesh->groupCount; loop++)
            group = &mesh->groupList[loop];

        Mem_Free(mesh->groupList);
        Mesh_ReturnToList(mesh);

        mesh = clonedFrom;
    }

    mesh->numOfRefs--;

    if (mesh->numOfRefs == 0)
    {
        RELEASE(mesh->uv);

        for (loop = 0; loop < mesh->groupCount; loop++)
        {
            group = &mesh->groupList[loop];

            Mesh_RemoveGroupTexture(mesh, loop);
            Mem_Free(group->faceData);
            Mem_Free(group->vertices);
        }

        if (mesh->lightWaveSurf)
            Mem_Free(mesh->lightWaveSurf);
        Mem_Free(mesh->groupList);
        Mesh_ReturnToList(mesh);
    }
}
