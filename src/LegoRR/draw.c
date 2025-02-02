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
    if (surf->lpVtbl->GetSurfaceDesc(surf, &desc) == DD_OK)
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

void Draw_GetClipWindow(Area2F* window)
{
    window->x = drawGlobs.clipStart.x;
    window->y = drawGlobs.clipStart.y;
    window->width = drawGlobs.clipEnd.x - drawGlobs.clipStart.x;
    window->height = drawGlobs.clipEnd.y - drawGlobs.clipStart.y;
}

B32 Draw_LockSurface(LPDIRECTDRAWSURFACE4 surf, DrawEffect effect)
{
    DDSURFACEDESC2 desc;
    HRESULT r;

    memset(&desc, 0, sizeof(DDSURFACEDESC2));
    desc.dwSize = sizeof(DDSURFACEDESC2);

    if ((r = surf->lpVtbl->Lock(surf, NULL, &desc, DDLOCK_WAIT, NULL)) == DD_OK)
    {
        U32 loop;

        drawGlobs.buffer = desc.lpSurface;
        drawGlobs.pitch = desc.lPitch;
        drawGlobs.redMask = desc.ddpfPixelFormat.dwRBitMask;
        drawGlobs.greenMask = desc.ddpfPixelFormat.dwGBitMask;
        drawGlobs.blueMask = desc.ddpfPixelFormat.dwBBitMask;
        drawGlobs.bpp = desc.ddpfPixelFormat.dwRGBBitCount;

        drawGlobs.redBits = 0;
        drawGlobs.greenBits = 0;
        drawGlobs.blueBits = 0;

        for (loop = 0; loop < drawGlobs.bpp; loop++)
        {
            if (drawGlobs.redMask & (1 << loop))
                drawGlobs.redBits++;
            if (drawGlobs.greenMask & (1 << loop))
                drawGlobs.greenBits++;
            if (drawGlobs.blueMask & (1 << loop))
                drawGlobs.blueBits++;
        }

        if (Draw_SetDrawPixelFunc(effect))
            return TRUE;

        Draw_UnlockSurface(surf);
    }

    return FALSE;
}

void Draw_UnlockSurface(LPDIRECTDRAWSURFACE4 surf)
{
    surf->lpVtbl->Unlock(surf, NULL);
    drawGlobs.buffer = NULL;
    drawGlobs.pitch = 0;
    drawGlobs.redMask = 0;
    drawGlobs.greenMask = 0;
    drawGlobs.blueMask = 0;
    drawGlobs.drawPixelFunc = NULL;
    drawGlobs.bpp = 0;
}

B32 Draw_SetDrawPixelFunc(DrawEffect effect)
{
    if (drawGlobs.bpp == 8)
    {
        drawGlobs.drawPixelFunc = Draw_Pixel8;
    }
    else if (drawGlobs.bpp == 16)
    {
        if (effect == DrawEffect_XOR)
            drawGlobs.drawPixelFunc = Draw_Pixel16XOR;
        else if (effect == DrawEffect_HalfTrans)
            drawGlobs.drawPixelFunc = Draw_Pixel16HalfTrans;
        else
            drawGlobs.drawPixelFunc = Draw_Pixel16;
    }
    else if (drawGlobs.bpp == 24)
    {
        drawGlobs.drawPixelFunc = Draw_Pixel24;
    }
    else if (drawGlobs.bpp == 32)
    {
        drawGlobs.drawPixelFunc = Draw_Pixel32;
    }
    else
    {
        drawGlobs.drawPixelFunc = NULL;
        return FALSE;
    }

    return TRUE;
}

void Draw_LineListEx(Point2F* fromList, Point2F* toList, U32 count, F32 r, F32 g, F32 b, DrawEffect effect)
{
    LPDIRECTDRAWSURFACE4 surf = DirectDraw_bSurf();
    Point2F *from, *to;
    U32 loop, colour;

    Error_Fatal(!(drawGlobs.flags & DRAW_GLOB_FLAG_INITIALIZED), "Draw not initialized");

    if (Draw_LockSurface(surf, effect))
    {
        colour = Draw_GetColour(r, g, b);

        for (loop = 0; loop < count; loop++)
        {
            from = &fromList[loop];
            to = &toList[loop];
            Draw_LineActual((S32) from->x, (S32) from->y, (S32) to->x, (S32) to->y, colour);
        }
        Draw_UnlockSurface(surf);
    }
}

void Draw_LineActual(S32 x1, S32 y1, S32 x2, S32 y2, U32 colour)
{
    S32 deltax, deltay, numpixels;
    S32 d, dinc1, dinc2;
    S32 x, xinc1, xinc2;
    S32 y, yinc1, yinc2;
    S32 loop;

    deltax = abs(x2 - x1);
    deltay = abs(y2 - y1);

    if (deltax >= deltay)
    {
        numpixels = deltax + 1;
        d = 2 * deltay - deltax;
        dinc1 = deltay << 1;
        dinc2 = (deltay - deltax) << 1;
        xinc1 = 1;
        xinc2 = 1;
        yinc1 = 0;
        yinc2 = 1;
    }
    else
    {
        numpixels = deltay + 1;
        d = (2 * deltax) - deltay;
        dinc1 = deltax << 1;
        dinc2 = (deltax - deltay) << 1;
        xinc1 = 0;
        xinc2 = 1;
        yinc1 = 1;
        yinc2 = 1;
    }

    if (x1 > x2)
    {
        xinc1 = -xinc1;
        xinc2 = -xinc2;
    }

    if (y1 > y2)
    {
        yinc1 = -yinc1;
        yinc2 = -yinc2;
    }

    x = x1;
    y = y1;

    for (loop = 1; loop <= numpixels; loop++)
    {
        if (x >= drawGlobs.clipStart.x && y >= drawGlobs.clipStart.y && x < drawGlobs.clipEnd.x && y < drawGlobs.clipEnd.y)
        {
            drawGlobs.drawPixelFunc(x, y, colour);
        }

        if (d < 0)
        {
            d = d + dinc1;
            x = x + xinc1;
            y = y + yinc1;
        }
        else
        {
            d = d + dinc2;
            x = x + xinc2;
            y = y + yinc2;
        }
    }
}

void Draw_Pixel8(S32 x, S32 y, U32 value)
{
    U8* addr = &((U8*)drawGlobs.buffer)[(y * drawGlobs.pitch) + x];
    *addr = (U8) value;
}

void Draw_Pixel16(S32 x, S32 y, U32 value)
{
    U16* addr = (U16*) &((U8*)drawGlobs.buffer)[(y * drawGlobs.pitch) + (x * 2)];
    *addr = (U16) value;
}

void Draw_Pixel16XOR(S32 x, S32 y, U32 value)
{
    U16* addr = (U16*) &((U8*)drawGlobs.buffer)[(y * drawGlobs.pitch) + (x * 2)];
    *addr = *addr ^((U16) value);
}

void Draw_Pixel16HalfTrans(S32 x, S32 y, U32 value)
{
    U16* addr = (U16*) &((U8*)drawGlobs.buffer)[(y * drawGlobs.pitch) + (x * 2)];
    U16 r, g, b;

    r = (((*addr >> 12) + ((U16) value >> 12)) << 11) & (U16) drawGlobs.redMask;
    g = ((((*addr & (U16) drawGlobs.greenMask) >> 6) + (((U16) value & (U16) drawGlobs.greenMask) >> 6)) << 5) & (U16) drawGlobs.greenMask;
    b = (((*addr & (U16) drawGlobs.blueMask) >> 1) + (((U16) value & (U16) drawGlobs.blueMask) >> 1)) & (U16) drawGlobs.blueMask;

    *addr = (r | g | b);
}

void Draw_Pixel24(S32 x, S32 y, U32 value)
{
    U32* addr = (U32*) &((U8*)drawGlobs.buffer)[(y * drawGlobs.pitch) + (x * 3)];

    *addr &= 0x000000FF;
    *addr |= value << 8;
}

void Draw_Pixel32(S32 x, S32 y, U32 value)
{
    U32* addr = (U32*) &((U8*)drawGlobs.buffer)[(y * drawGlobs.pitch) + (x * 4)];
    *addr = (U32) value;
}

U32 Draw_GetColour(F32 r, F32 g, F32 b)
{
    U32 colour = 0;

    Error_Fatal(drawGlobs.buffer == NULL, "Must be called after Draw_LockSurface()");

    if (drawGlobs.bpp == 8)
    {
        // Empty in original source
    }
    else
    {
        colour |= (U32) (r * 255.0f) >> (8 - drawGlobs.redBits) << (drawGlobs.greenBits + drawGlobs.blueBits);
        colour |= (U32) (g * 255.0f) >> (8 - drawGlobs.greenBits) << drawGlobs.blueBits;
        colour |= (U32) (b * 255.0f) >> (8 - drawGlobs.blueBits);
    }

    return colour;
}
