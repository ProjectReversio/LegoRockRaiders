#include "images.h"
#include "error.h"
#include "mem.h"
#include "bmp.h"
#include "directdraw.h"
#include <ddraw.h>
#include <d3drm.h>
#include <stdio.h>

Image_Globs imageGlobs = { NULL };

void Image_Initialize()
{
    U32 loop;

    if (imageGlobs.flags & IMAGE_FLAG_INITIALIZED)
        Error_Fatal(TRUE, "Images already initialized");

    for (loop = 0; loop < IMAGE_MAXLISTS; loop++)
        imageGlobs.listSet[loop] = NULL;

    imageGlobs.flags = IMAGE_FLAG_INITIALIZED;
}

void Image_Shutdown()
{
    U32 loop;

    Image_RemoveAll();

    for (loop = 0; loop < IMAGE_MAXLISTS; loop++)
    {
        if (imageGlobs.listSet[loop])
            Mem_Free(imageGlobs.listSet[loop]);
    }

    imageGlobs.freeList = NULL;
    imageGlobs.flags = IMAGE_FLAG_NONE;
}

lpImage Image_Create(LPDIRECTDRAWSURFACE4 surface, U32 width, U32 height, COLORREF penZero, COLORREF pen255)
{
    Image* newImage;

    Image_CheckInit();

    if (imageGlobs.freeList == NULL)
        Image_AddList();

    newImage = imageGlobs.freeList;
    imageGlobs.freeList = newImage->nextFree;
    newImage->nextFree = newImage;

    newImage->flags = IMAGE_FLAG_NONE;
    newImage->width = width;
    newImage->height = height;
    newImage->surface = surface;
    newImage->penZeroRGB = penZero;
    newImage->penZero = Image_DDColorMatch(surface, penZero);
    newImage->pen255 = Image_DDColorMatch(surface, pen255);

    return newImage;
}

void Image_AddList()
{
    Image* list;
    U32 loop, count;

    Image_CheckInit();

    Error_Fatal(imageGlobs.listCount + 1 >= IMAGE_MAXLISTS, "Run out of lists");

    count = 0x00000001 << imageGlobs.listCount;

    list = imageGlobs.listSet[imageGlobs.listCount] = Mem_Alloc(sizeof(Image) * count);
    if (list)
    {
        imageGlobs.listCount++;

        for (loop = 1; loop < count; loop++)
        {
            list[loop - 1].nextFree = &list[loop];
        }

        list[count - 1].nextFree = imageGlobs.freeList;
        imageGlobs.freeList = list;
    } else {
        Error_Fatal(TRUE, Error_Format("Unable to allocate %d bytes of memory for new list.\n", sizeof(Image) * count));
    }
}

lpImage Image_LoadBMPScaled(const char* fileName, U32 width, U32 height)
{
    void* buffer = 0;
    U32 size = 0;
    D3DRMIMAGE image = {0};
    DDSURFACEDESC2 ddsd = {0};
    LPDIRECTDRAWSURFACE4 surface = 0;
    COLORREF penZero, pen255;
    U8 name[1024];

    penZero = pen255 = Image_RGB2CR(0, 0, 0);

    if (buffer = File_LoadBinary(fileName, &size))
    {
        BMP_Parse(buffer, size, &image);
        if (image.rgb == FALSE)
        {
            penZero = Image_RGB2CR(image.palette[0].red, image.palette[0].green, image.palette[0].blue);
            pen255 = Image_RGB2CR(image.palette[255].red, image.palette[255].green, image.palette[255].blue);
        }
    } else {
        return NULL;
    }

    sprintf(name, "Data\\%s", fileName);

    ZeroMemory(&ddsd, sizeof(ddsd));
    ddsd.dwSize = sizeof(ddsd);
    ddsd.dwFlags = DDSD_CAPS | DDSD_HEIGHT | DDSD_WIDTH;
    ddsd.ddsCaps.dwCaps = DDSCAPS_OFFSCREENPLAIN | DDSCAPS_SYSTEMMEMORY;
    ddsd.dwWidth = image.width;
    ddsd.dwHeight = image.height;

    if (DirectDraw()->lpVtbl->CreateSurface(DirectDraw(), &ddsd, &surface, NULL) == DD_OK)
    {
        lpImage newImage;

        if (Image_CopyToDataToSurface(surface, &image))
        {
            newImage = Image_Create(surface, image.width, image.height, penZero, pen255);
            if (newImage)
            {
                BMP_Cleanup(&image);
                if (buffer)
                    Mem_Free(buffer);
                return newImage;
            } else {
                Error_Warn(TRUE, "Could not create image");
            }
        } else {
            Error_Warn(TRUE, "Could not copy image to surface");
        }
    } else {
        Error_Warn(TRUE, "Could not create surface");
    }

    if (surface)
        surface->lpVtbl->Release(surface);
    BMP_Cleanup(&image);
    if (buffer)
        Mem_Free(buffer);
    return NULL;
}

void Image_DisplayScaled(lpImage image, Area2F* src, Point2F* destPos, Size2F* destSize)
{
    RECT r_src, r_dst;

    Error_Fatal(!image, "NULL passed as image to Image_DisplayScaled()");

    if (src)
    {
        r_src.left = (U32) src->x;
        r_src.top = (U32) src->y;
        r_src.right = (U32) (src->x + src->width);
        r_src.bottom = (U32) (src->y + src->height);
    }

    if (destPos)
    {
        r_dst.left = (U32) destPos->x;
        r_dst.top = (U32) destPos->y;
        if (destSize)
        {
            r_dst.right = (U32) (destPos->x + destSize->width);
            r_dst.bottom = (U32) (destPos->y + destSize->height);
        } else if (src)
        {
            r_dst.right = (U32) (destPos->x + src->width);
            r_dst.bottom = (U32) (destPos->y + src->height);
        } else {
            r_dst.right = (U32) (destPos->x + image->width);
            r_dst.bottom = (U32) (destPos->y + image->height);
        }
    }

    if (image->flags & IMAGE_FLAG_TRANS)
        DirectDraw_bSurf()->lpVtbl->Blt(DirectDraw_bSurf(), (destPos) ? &r_dst : NULL, image->surface, (src) ? &r_src : NULL, DDBLT_KEYSRC | DDBLT_WAIT, NULL);
    else
        DirectDraw_bSurf()->lpVtbl->Blt(DirectDraw_bSurf(), (destPos) ? &r_dst : NULL, image->surface, (src) ? &r_src : NULL, DDBLT_WAIT, NULL);
}

void Image_RemoveAll()
{
    U32 list, count, loop;
    lpImage tempImage;

    for (list = 0; list < imageGlobs.listCount; list++)
    {
        if (imageGlobs.listSet[list])
        {
            count = 0x00000001 << list;
            for (loop = 0; loop < count; loop++)
            {
                if (tempImage = &imageGlobs.listSet[list][loop])
                {
                    if (tempImage->nextFree == tempImage)
                    {
                        Image_Remove(tempImage);
                    }
                }
            }
        }
    }
}

void Image_Remove(lpImage dead)
{
    Image_CheckInit();
    Error_Fatal(!dead, "NULL passed to Image_Remove()");

    dead->surface->lpVtbl->Release(dead->surface);

    dead->nextFree = imageGlobs.freeList;
    imageGlobs.freeList = dead;
}

void Image_FlipSurface(LPDDSURFACEDESC2 desc)
{
    U32 h;
    U32 shortsPerLine = desc->lPitch / 2;
    U32 height = desc->dwHeight;
    U16* start = (U16*) desc->lpSurface;
    U16* end = (U16*) desc->lpSurface + ((height - 1) * shortsPerLine);
    void* temp = Mem_Alloc(sizeof(U16) * shortsPerLine);

    for (h = 0; h < desc->dwHeight / 2; h++)
    {
        memcpy(temp, start, sizeof(U16) * shortsPerLine);
        memcpy(start, end, sizeof(U16) * shortsPerLine);
        memcpy(end, temp, sizeof(U16) * shortsPerLine);
        start += shortsPerLine;
        end -= shortsPerLine;
    }

    Mem_Free(temp);
}

B32 Image_CopyToDataToSurface(LPDIRECTDRAWSURFACE4 surface, LPD3DRMIMAGE image)
{
    DDSURFACEDESC2 desc = {0};
    desc.dwSize = sizeof(desc);

    if (surface->lpVtbl->Lock(surface, 0, &desc, DDLOCK_WAIT | DDLOCK_WRITEONLY, 0) == DD_OK)
    {
        if (image->depth < 8)
        {
            Error_Warn(TRUE, "Please use 8/24 bit surfaces");
        } else if (image->depth == 8)
        {
            Image_8BitSourceCopy(&desc, image);
        } else if (image->depth == 16)
        {
            Error_Warn(TRUE, "16 bit image specified - surely BMPs can't do that");
        } else if (image->depth == 24)
        {
            Image_24BitSourceCopy(&desc, image);
        } else if (image->depth == 32)
        {
            Error_Warn(TRUE, "32 bit images are no fun - 8 and 24 only please");
        }

        surface->lpVtbl->Unlock(surface, 0);

        return TRUE;
    } else {
        Error_Warn(TRUE, "Could not lock surface");
    }

    return FALSE;
}

B32 Image_8BitSourceCopy(LPDDSURFACEDESC2 desc, LPD3DRMIMAGE image)
{
    if (desc->ddpfPixelFormat.dwRGBBitCount == 8)
    {
        Error_Warn(TRUE, "Why have I got an 8 bit surface");
        return FALSE;
    } else if (desc->ddpfPixelFormat.dwRGBBitCount == 16)
    {
        U32 bmpLineLength = image->bytes_per_line;
        U32 surfaceLineLength = ((U32) desc->lPitch) / 2;
        U8* bmpPtr;
        U16* surfPtr;
        LPD3DRMPALETTEENTRY pal = image->palette;
        U32 rMask, gMask, bMask;
        U32 rBitCount, gBitCount, bBitCount;
        U32 rBitShift, gBitShift, bBitShift;
        S32 h, w;

        bmpPtr = (U8*) image->buffer1;
        surfPtr = (U16*) desc->lpSurface;

        rMask = desc->ddpfPixelFormat.dwRBitMask;
        gMask = desc->ddpfPixelFormat.dwGBitMask;
        bMask = desc->ddpfPixelFormat.dwBBitMask;

        rBitCount = Image_CountMaskBits(rMask);
        gBitCount = Image_CountMaskBits(gMask);
        bBitCount = Image_CountMaskBits(bMask);

        rBitShift = Image_CountMaskBitShift(rMask);
        gBitShift = Image_CountMaskBitShift(gMask);
        bBitShift = Image_CountMaskBitShift(bMask);

        for (h = 0; h < image->height; h++)
        {
            for (w = 0; w < image->width; w++)
            {
                U8 r, g, b;
                U16 surfaceValue;

                r = pal[*bmpPtr].red;
                g = pal[*bmpPtr].green;
                b = pal[*(bmpPtr++)].blue;

                // Mix the colours
                surfaceValue =
                    ((r >> (8 - rBitCount)) << rBitShift) |
                    ((g >> (8 - gBitCount)) << gBitShift) |
                    ((b >> (8 - bBitCount)) << bBitShift);

                *(surfPtr++) = surfaceValue;
            }

            // Do the pitch thing
            bmpPtr += bmpLineLength - image->width;
            surfPtr += surfaceLineLength - image->width;
        }

        Image_FlipSurface(desc);

    } else if (desc->ddpfPixelFormat.dwRGBBitCount == 24)
    {
        Error_Warn(TRUE, "Why have I got a 24 bit surface");
        return FALSE;
    } else if (desc->ddpfPixelFormat.dwRGBBitCount == 32)
    {
        Error_Warn(TRUE, "Why have I got a 32 bit surface");
        return FALSE;
    }

    return TRUE;
}
B32 Image_24BitSourceCopy(LPDDSURFACEDESC2 desc, LPD3DRMIMAGE image)
{
    if (desc->ddpfPixelFormat.dwRGBBitCount == 8)
    {
        Error_Warn(TRUE, "Why have I got an 8 bit surface");
        return FALSE;
    } else if (desc->ddpfPixelFormat.dwRGBBitCount == 16)
    {
        // Not convinced that this is correct as it does
        // not round up properly unless we have even sizes.
        U32 bmpLineLength = image->bytes_per_line;
        U32 surfaceLineLength = ((U32) desc->lPitch) / 2;
        U8* bmpPtr;
        U16* surfPtr;
        U32 rMask, gMask, bMask;
        U32 rBitCount, gBitCount, bBitCount;
        U32 rBitShift, gBitShift, bBitShift;
        S32 h, w;

        bmpPtr = (U8*) image->buffer1;
        surfPtr = (U16*) desc->lpSurface;

        rMask = desc->ddpfPixelFormat.dwRBitMask;
        gMask = desc->ddpfPixelFormat.dwGBitMask;
        bMask = desc->ddpfPixelFormat.dwBBitMask;

        rBitCount = Image_CountMaskBits(rMask);
        gBitCount = Image_CountMaskBits(gMask);
        bBitCount = Image_CountMaskBits(bMask);

        rBitShift = Image_CountMaskBitShift(rMask);
        gBitShift = Image_CountMaskBitShift(gMask);
        bBitShift = Image_CountMaskBitShift(bMask);

        for (h = 0; h < image->height; h++)
        {
            for (w = 0; w < image->width; w++)
            {
                U8 r, g, b;
                U16 surfaceValue;

                b = *bmpPtr; bmpPtr++;
                g = *bmpPtr; bmpPtr++;
                r = *bmpPtr; bmpPtr++;

                // Mix the colours
                surfaceValue =
                    ((r >> (8 - rBitCount)) << rBitShift) |
                    ((g >> (8 - gBitCount)) << gBitShift) |
                    ((b >> (8 - bBitCount)) << bBitShift);

                *(surfPtr++) = surfaceValue;
            }

            // Do the pitch thing
            bmpPtr += bmpLineLength - (image->width * 3);
            surfPtr += surfaceLineLength - image->width;
        }

        Image_FlipSurface(desc);

    } else if (desc->ddpfPixelFormat.dwRGBBitCount == 24)
    {
        Error_Warn(TRUE, "Why have I got a 24 bit surface");
        return FALSE;
    } else if (desc->ddpfPixelFormat.dwRGBBitCount == 32)
    {
        Error_Warn(TRUE, "Why have I got a 32 bit surface");
        return FALSE;
    }

    return TRUE;
}

U32 Image_CountMaskBits(U32 mask)
{
    U32 i, count = 0;

    for (i = 0; i < (sizeof(mask) * 8); i++)
    {
        if ((mask & (1 << i)) != 0)
            count++;
    }

    return count;
}

U32 Image_CountMaskBitShift(U32 mask)
{
    U32 i;
    for (i = 0; i < (sizeof(mask) * 8); i++)
    {
        if ((mask & (1 << i)) != 0)
            return i;
    }
    return 0xffffffff;
}

COLORREF Image_RGB2CR(U8 r, U8 g, U8 b)
{
    COLORREF cr;
    U8* ptr = (U8*) &cr;
    ptr[0] = r;
    ptr[1] = g;
    ptr[2] = b;
    ptr[3] = 0;
    return cr;
}

void Image_CR2RGB(COLORREF cr, U8* r, U8* g, U8* b)
{
    U8* ptr = (U8*) &cr;
    if (r)
        *r = ptr[0];
    if (g)
        *g = ptr[1];
    if (b)
        *b = ptr[2];
}

U32 Image_DDColorMatch(LPDIRECTDRAWSURFACE4 pdds, U32 rgb)
{
    DDSURFACEDESC2 ddsd = {0};
    U8 r, g, b;
    U32 dw = 0;

    ddsd.dwSize = sizeof(ddsd);

    Image_CR2RGB(rgb, &r, &g, &b);

    if (pdds->lpVtbl->Lock(pdds, NULL, &ddsd, DDLOCK_WAIT, NULL) == DD_OK)
    {
        U32 rMask, gMask, bMask;
        U32 rBitCount, gBitCount, bBitCount;
        U32 rBitShift, gBitShift, bBitShift;

        rMask = ddsd.ddpfPixelFormat.dwRBitMask;
        gMask = ddsd.ddpfPixelFormat.dwGBitMask;
        bMask = ddsd.ddpfPixelFormat.dwBBitMask;

        rBitCount = Image_CountMaskBits(rMask);
        gBitCount = Image_CountMaskBits(gMask);
        bBitCount = Image_CountMaskBits(bMask);

        rBitShift = Image_CountMaskBitShift(rMask);
        gBitShift = Image_CountMaskBitShift(gMask);
        bBitShift = Image_CountMaskBitShift(bMask);

        dw = ((r >> (8 - rBitCount)) << rBitShift) |
             ((g >> (8 - gBitCount)) << gBitShift) |
             ((b >> (8 - bBitCount)) << bBitShift);

        if (ddsd.ddpfPixelFormat.dwRGBBitCount < 32)
        {
            dw &= (1 << ddsd.ddpfPixelFormat.dwRGBBitCount) - 1;
        }

        pdds->lpVtbl->Unlock(pdds, NULL);
    }
    return dw;
}

void Image_SetupTrans(lpImage image, F32 lowr, F32 lowg, F32 lowb, F32 highr, F32 highg, F32 highb)
{
    // TODO: Implement Image_SetupTrans
}

void* Image_LockSurface(lpImage image, U32* pitch, U32* bpp)
{
    DDSURFACEDESC2 desc;

    memset(&desc, 0, sizeof(DDSURFACEDESC2));
    desc.dwSize = sizeof(DDSURFACEDESC2);
    if (image->surface->lpVtbl->Lock(image->surface, NULL, &desc, DDLOCK_WAIT, NULL) == DD_OK)
    {
        *pitch = desc.lPitch;
        *bpp = desc.ddpfPixelFormat.dwRGBBitCount;
        return desc.lpSurface;
    }

    return NULL;
}

void Image_UnlockSurface(lpImage image)
{
    image->surface->lpVtbl->Unlock(image->surface, NULL);
}

U32 Image_GetPen255(lpImage image)
{
    DDPIXELFORMAT pf;
    U32 pen = 0;
    U8* s, *t;

    memset(&pf, 0, sizeof(DDPIXELFORMAT));
    pf.dwSize = sizeof(DDPIXELFORMAT);

    if (image->surface->lpVtbl->GetPixelFormat(image->surface, &pf) == DD_OK)
    {
        s = (U8*) &pen;
        t = (U8*) &image->pen255;

        s[0] = t[3];
        s[1] = t[2];
        s[2] = t[1];
        s[3] = t[0];

        return pen;
    }

    return 0;
}

void Image_SetPenZeroTrans(lpImage image)
{
    DDCOLORKEY ColourKey;

    Error_Fatal(!image, "NULL passed as image to Image_SetupTrans()");

    ColourKey.dwColorSpaceLowValue = image->penZero;
    ColourKey.dwColorSpaceHighValue = image->penZero;

    image->surface->lpVtbl->SetColorKey(image->surface, DDCKEY_SRCBLT, &ColourKey);
    image->flags |= IMAGE_FLAG_TRANS;
}

U32 Image_GetPixelMask(lpImage image)
{
    DDPIXELFORMAT pf;

    memset(&pf, 0, sizeof(DDPIXELFORMAT));
    pf.dwSize = sizeof(DDPIXELFORMAT);

    if (image->surface->lpVtbl->GetPixelFormat(image->surface, &pf) == DD_OK)
    {
        return 0xffffffff << (32 - pf.dwRGBBitCount);
    }

    return 0;
}
