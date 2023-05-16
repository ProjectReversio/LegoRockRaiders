#include "draw.h"
#include <ddraw.h>
#include "directdraw.h"
#include "error.h"

Draw_Globs drawGlobs = { NULL };

void Draw_Initialize(lpArea2F window)
{
    drawGlobs.flags |= DRAW_GLOB_FLAG_INITIALIZED;
    Draw_SetClipWindow(window);
}

void Draw_SetClipWindow(Area2F* window)
{
    LPDIRECTDRAWSURFACE4 surf = DirectDraw_bSurf();
    DDSURFACEDESC2 desc;

    Error_Fatal(!(drawGlobs.flags & DRAW_GLOB_FLAG_INITIALIZED), "Draw not initialized");

    drawGlobs.clipStart.x = 0;
    drawGlobs.clipStart.y = 0;

    if (window)
    {
        if (window->x > 0)
            drawGlobs.clipStart.x = window->x;
        if (window->y > 0)
            drawGlobs.clipStart.y = window->y;
    }

    memset(&desc, 0, sizeof(DDSURFACEDESC2));
    desc.dwSize = sizeof(DDSURFACEDESC2);
    if (surf->lpVtbl->GetSurfaceDesc(surf, &desc))
    {
        drawGlobs.clipEnd.x = (F32) desc.dwWidth;
        drawGlobs.clipEnd.y = (F32) desc.dwHeight;
        if (window)
        {
            if (window->x + window->width < desc.dwWidth)
                drawGlobs.clipEnd.x = window->x + window->width;
            if (window->y + window->height < desc.dwHeight)
                drawGlobs.clipEnd.y = window->y + window->height;
        }
    }
}
