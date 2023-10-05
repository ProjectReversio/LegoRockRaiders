#include "viewport.h"
#include "mem.h"
#include "main.h"
#include "error.h"
#include "dxbug.h"
#include "mesh.h"

Viewport_Globs viewportGlobs = { NULL };

void Viewport_Initialize()
{
    U32 loop;

    for (loop = 0; loop < VIEWPORT_MAXLISTS; loop++)
    {
        viewportGlobs.listSet[loop] = NULL;
    }

    viewportGlobs.freeList = NULL;
    viewportGlobs.listCount = 0;
    viewportGlobs.flags = VIEWPORT_FLAG_INITIALIZED;
}

void Viewport_Shutdown()
{
    U32 loop;

    Viewport_RemoveAll();

    for (loop = 0; loop < VIEWPORT_MAXLISTS; loop++)
    {
        if (viewportGlobs.listSet[loop])
            Mem_Free(viewportGlobs.listSet[loop]);
    }

    viewportGlobs.freeList = NULL;
    viewportGlobs.flags = 0x0;
}

lpViewport Viewport_Create(F32 xPos, F32 yPos, F32 width, F32 height, lpContainer camera)
{
    U32 actWidth, actHeight, actXPos, actYPos;
    U32 devWidth, devHeight;

    devWidth = lpDevice()->lpVtbl->GetWidth(lpDevice());
    devHeight = lpDevice()->lpVtbl->GetHeight(lpDevice());

    actXPos = (U32) (xPos * devWidth);
    actYPos = (U32) (yPos * devHeight);
    actWidth = (U32) (width * devWidth);
    actHeight = (U32) (height * devHeight);

    return Viewport_CreatePixel(actXPos, actYPos, actWidth, actHeight, camera);
}

lpViewport Viewport_CreatePixel(S32 xPos, S32 yPos, U32 width, U32 height, lpContainer camera)
{
    lpViewport newViewport;
    U32 devWidth = lpDevice()->lpVtbl->GetWidth(lpDevice());
    U32 devHeight = lpDevice()->lpVtbl->GetHeight(lpDevice());

    Viewport_CheckInit();

    if (xPos < 0)
        xPos = devWidth + xPos;
    if (yPos < 0)
        yPos = devHeight + yPos;

    if (xPos + width > devWidth || yPos + height > devHeight)
    {
        Error_Warn(TRUE, "Invalid sizes provided to Viewport_CreatePixel()");
        return NULL;
    }

    if (viewportGlobs.freeList == NULL)
        Viewport_AddList();

    newViewport = viewportGlobs.freeList;
    viewportGlobs.freeList = newViewport->nextFree;
    newViewport->nextFree = NULL;

    if (lpD3DRM()->lpVtbl->CreateViewport(lpD3DRM(), lpDevice(), camera->masterFrame, xPos, yPos, width, height, &newViewport->lpVP) == D3DRM_OK)
    {
        newViewport->lpVP->lpVtbl->SetAppData(newViewport->lpVP, (U32) newViewport);

        newViewport->smoothFOV = 0.0f;
        return newViewport;
    } else
    {
        Error_Warn(TRUE, "CreateViewport() call failed");
    }
    return NULL;
}

void Viewport_SetBackClip(lpViewport viewport, F32 dist)
{
    Viewport_CheckInit();

    if (viewport->lpVP->lpVtbl->SetBack(viewport->lpVP, dist) != D3DRM_OK)
        Error_Warn(TRUE, "Cannot set back clipping plane distance");
}

void Viewport_SetCamera(lpViewport vp, lpContainer cont)
{
    Viewport_CheckInit();

    Error_Fatal(!vp || !cont, "NULL passed as viewport or container to Viewport_SetCamera()");

    // Does it matter that a non-camera container can be used as a camera????

    if (vp->lpVP->lpVtbl->SetCamera(vp->lpVP, cont->masterFrame) != D3DRM_OK)
        Error_Warn(TRUE, "Cannot set container as camera");
}

lpContainer Viewport_GetCamera(lpViewport vp)
{
    LPDIRECT3DRMFRAME3 frame;
    lpContainer camera = NULL;

    Viewport_CheckInit();

    Error_Fatal(!vp, "NULL passed as viewport or container to Viewport_GetCamera()");

    // Does it matter that a non-camera container can be used as a camera????

    if (vp->lpVP->lpVtbl->GetCamera(vp->lpVP, &frame) == D3DRM_OK)
    {
        lpContainer_AppData appData = (lpContainer_AppData) frame->lpVtbl->GetAppData(frame);
        if (appData)
            camera = appData->ownerContainer;

        frame->lpVtbl->Release(frame);
    }
    else
    {
        Error_Warn(TRUE, "Cannot get camera from viewport");
    }

    return camera;
}

void Viewport_SetField(lpViewport vp, F32 fov)
{
    Viewport_CheckInit();
    Error_Fatal(!vp, "NULL passed to Viewport_SetField()");

    vp->lpVP->lpVtbl->SetField(vp->lpVP, fov);
    vp->smoothFOV = 0.0f;
}

F32 Viewport_GetField(lpViewport vp)
{
    Viewport_CheckInit();
    Error_Fatal(!vp, "NULL passed to Viewport_GetField()");

    return vp->lpVP->lpVtbl->GetField(vp->lpVP);
}

LPDIRECT3DRMFRAME3 Viewport_GetScene(lpViewport vp)
{
    LPDIRECT3DRMFRAME3 camera = NULL, scene = NULL;

    vp->lpVP->lpVtbl->GetCamera(vp->lpVP, &camera);

    if (camera)
    {
        camera->lpVtbl->Release(camera);
        camera->lpVtbl->GetScene(camera, &scene);
        scene->lpVtbl->Release(scene);
    }

    return scene;
}

void Viewport_AddList()
{
    lpViewport list;
    U32 loop, count;

    Viewport_CheckInit();

    Error_Fatal(viewportGlobs.listCount + 1 >= VIEWPORT_MAXLISTS, "Run out of lists");

    count = 0x00000001 << viewportGlobs.listCount;

    list = viewportGlobs.listSet[viewportGlobs.listCount] = Mem_Alloc(sizeof(Viewport) * count);
    if (list)
    {
        viewportGlobs.listCount++;

        for (loop = 1; loop < count; loop++)
        {
            list[loop - 1].nextFree = &list[loop];
        }

        list[count - 1].nextFree = viewportGlobs.freeList;
        viewportGlobs.freeList = list;
    }
    else
    {
        Error_Fatal(TRUE, Error_Format("Unable to allocate %d bytes of memory for new list.\n", sizeof(Viewport) * count));
    }
}

void Viewport_RemoveAll()
{
    U32 list, count, loop;
    lpViewport tempViewport;

    for (list = 0; list < viewportGlobs.listCount; list++)
    {
        if (viewportGlobs.listSet[list])
        {
            count = 0x00000001 << list;
            for (loop = 0; loop < count; loop++)
            {
                if ((tempViewport = &viewportGlobs.listSet[list][loop]))
                {
                    if (tempViewport->nextFree == tempViewport)
                    {
                        tempViewport->lpVP->lpVtbl->Release(tempViewport->lpVP);
                    }
                }
            }
        }
    }
}

void Viewport_Render(lpViewport vp, lpContainer root, F32 delta)
{
    HRESULT r;

    Viewport_CheckInit();

    Error_Fatal(!vp || !root, "NULL passed as viewport or container to Viewport_Render()");

    vp->rendering = TRUE;

    if (vp->smoothFOV != 0.0f)
    {
        F32 coef, fov = vp->lpVP->lpVtbl->GetField(vp->lpVP);

        coef = 4.0f * (1.0f / delta);
        fov *= coef;
        fov += vp->smoothFOV;
        fov *= 1.0f / (1.0f + coef);

        vp->lpVP->lpVtbl->SetField(vp->lpVP, fov);

        if (fov == vp->smoothFOV)
            vp->smoothFOV = 0.0f;
    }

    if ((r = vp->lpVP->lpVtbl->Render(vp->lpVP, root->masterFrame)) == D3DRM_OK)
    {
        Mesh_PostRenderAll(vp);
    } else {
        Error_Warn(TRUE, "Cannot render viewport");
        CHKRM(r);
    }

    vp->rendering = FALSE;
}

void Viewport_Clear(lpViewport vp, B32 full)
{
    Viewport_CheckInit();

    if (full)
    {
        LPDIRECT3DRMFRAME3 scene = Viewport_GetScene(vp);
        D3DCOLOR colour = 0;
        Area2F area = {
            (F32) vp->lpVP->lpVtbl->GetX(vp->lpVP),
            (F32) vp->lpVP->lpVtbl->GetY(vp->lpVP),
            (F32) vp->lpVP->lpVtbl->GetWidth(vp->lpVP),
            (F32) vp->lpVP->lpVtbl->GetHeight(vp->lpVP)
        };

        if (scene)
            colour = scene->lpVtbl->GetSceneBackground(scene);

        {
            LPDIRECT3DVIEWPORT view1;
            LPDIRECT3DVIEWPORT3 view3;
            HRESULT r;
            D3DRECT rect = {(U32) area.x, (U32) area.y, (U32) (area.x + area.width), (U32) (area.y + area.height)};
            r = vp->lpVP->lpVtbl->GetDirect3DViewport(vp->lpVP, &view1);
            r = view1->lpVtbl->QueryInterface(view1, &IID_IDirect3DViewport3, &view3);
            view1->lpVtbl->Release(view1);
            r = view3->lpVtbl->Clear2(view3, 1, &rect, D3DCLEAR_ZBUFFER | D3DCLEAR_TARGET, colour, 1.0f, 0);
            view3->lpVtbl->Release(view3);
        }
    } else
    {
        vp->lpVP->lpVtbl->Clear(vp->lpVP, D3DRMCLEAR_ALL);
    }
}
