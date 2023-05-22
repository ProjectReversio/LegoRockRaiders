#include "images.h"
#include "error.h"
#include "mem.h"
#include <ddraw.h>
#include <d3drm.h>

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
    // TODO: Implement Image_Create
    return NULL;
}

void Image_AddList()
{
    // TODO: Implement Image_AddList
}

lpImage Image_LoadBMPScaled(const char* fileName, U32 width, U32 height)
{
    // TODO: Implement Image_LoadBMPScaled
    return NULL;
}

void Image_DisplayScaled(lpImage image, Area2F* src, Point2F* destPos, Point2F* destSize)
{
    Image_DisplayScaled2(image, src, destPos, destSize, Image_TextureMode_Normal, 0.0f, NULL);
}

void Image_DisplayScaled2(lpImage image, Area2F* src, Point2F* destPos, Point2F* destSize, Image_TextureMode textureMode, F32 level, Point2F uvs[4])
{
    // TODO: Implement Image_DisplayScaled2
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
    // TODO: Implement Image_FlipSurface
}

B32 Image_CopyToDataToSurface(LPDIRECTDRAWSURFACE4 surface, LPD3DRMIMAGE image)
{
    // TODO: Implement Image_CopyToDataToSurface
    return FALSE;
}
B32 Image_8BitSourceCopy(LPDDSURFACEDESC2 desc, LPD3DRMIMAGE image)
{
    // TODO: Implement Image_8BitSourceCopy
    return FALSE;
}
B32 Image_24BitSourceCopy(LPDDSURFACEDESC2 desc, LPD3DRMIMAGE image)
{
    // TODO: Implement Image_24BitSourceCopy
    return FALSE;
}

U32 Image_CountMaskBits(U32 mask)
{
    // TODO: Implement Image_CountMaskBits
    return 0;
}

U32 Image_CountMaskBitShift(U32 mask)
{
    // TODO: Implement Image_CountMaskBitShift
    return 0;
}

COLORREF Image_RGB2CR(U8 r, U8 g, U8 b)
{
    // TODO: Implement Image_RGB2CR
    return 0;
}

void Image_CR2RGB(COLORREF cr, U8* r, U8* g, U8* b)
{
    // TODO: Implement Image_CR2RGB
    *r = 0; // temp
    *g = 0; // temp
    *b = 0; // temp
}

U32 Image_DDColorMatch(LPDIRECTDRAWSURFACE4 pdds, U32 rgb)
{
    // TODO: Implement Image_DDColorMatch
    return 0;
}
