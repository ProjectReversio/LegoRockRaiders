#include "viewport.h"
#include "mem.h"
#include "main.h"
#include "error.h"

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

lpContainer Viewport_GetCamera(lpViewport view)
{
    LPDIRECT3DRMFRAME3 frame;
    lpContainer camera = NULL;

    Viewport_CheckInit();

    Error_Fatal(!view, "NULL passed as viewport or container to Viewport_GetCamera()");

    // Does it matter that a non-camera container can be used as a camera????

    if (view->lpVP->lpVtbl->GetCamera(view->lpVP, &frame) == D3DRM_OK)
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
    // TODO: Implement Viewport_Render
}

void Viewport_Clear(lpViewport vp, B32 full)
{
    // TODO: Implement Viewport_Clear
}
