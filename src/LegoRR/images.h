#pragma once

#include "platform.h"
#include "maths.h"
#include <ddraw.h>
#include <d3drm.h>
#include <d3d.h>

#define IMAGE_MAXLISTS 32

#ifdef _DEBUG
#define Image_CheckInit() if (!(imageGlobs.flags & IMAGE_FLAG_INITIALIZED)) Error_Fatal(TRUE, "Error: Image_Initialize() Has Not Been Called")
#else
#define Image_CheckInit()
#endif

typedef enum ImageFlags
{
    IMAGE_FLAG_NONE = 0,
    IMAGE_FLAG_INITIALIZED = 0x1,
    IMAGE_FLAG_TRANS = 0x2,
    IMAGE_FLAG_TEXTURE = 0x4,
} ImageFlags;

typedef enum Image_TextureMode {

    Image_TextureMode_Normal,
    Image_TextureMode_Subtractive,
    Image_TextureMode_Additive,
    Image_TextureMode_Transparent,

    Image_TextureMode_Fixed_Subtractive, // Not affected by the global intensity value
    Image_TextureMode_Fixed_Additive,
    Image_TextureMode_Fixed_Transparent,

    Image_TextureMode_Count
} Image_TextureMode;

typedef struct Image
{
    LPDIRECTDRAWSURFACE4 surface;

    U32 width, height;
    U32 penZero, pen255, penZeroRGB;

    ImageFlags flags;

    struct Image *nextFree;
} Image, *lpImage;

typedef struct Image_Globs
{
    lpImage listSet[IMAGE_MAXLISTS];
    lpImage freeList;
    U32 listCount;
    U32 flags;
} Image_Globs, *lpImage_Globs;

extern Image_Globs imageGlobs;

#define Image_LoadBMP(n) Image_LoadBMPScaled((n), 0, 0)
#define Image_Display(p,l)  Image_DisplayScaled((p), NULL, (l), NULL)

inline S32 Image_GetWidth(lpImage p) { return p->width; }
inline S32 Image_GetHeight(lpImage p) { return p->height; }

extern void Image_Initialize();
extern void Image_Shutdown();

extern lpImage Image_Create(LPDIRECTDRAWSURFACE4 surface, U32 width, U32 height, COLORREF penZero, COLORREF pen255);
extern void Image_AddList();

extern lpImage Image_LoadBMPScaled(const char* fileName, U32 width, U32 height);
extern void Image_DisplayScaled(lpImage image, Area2F* src, Point2F* destPos, Size2F* destSize);

extern void Image_RemoveAll();
extern void Image_Remove(lpImage dead);

extern void Image_FlipSurface(LPDDSURFACEDESC2 desc);

extern B32 Image_CopyToDataToSurface(LPDIRECTDRAWSURFACE4 surface, LPD3DRMIMAGE image);
extern B32 Image_8BitSourceCopy(LPDDSURFACEDESC2 desc, LPD3DRMIMAGE image);
extern B32 Image_24BitSourceCopy(LPDDSURFACEDESC2 desc, LPD3DRMIMAGE image);

extern U32 Image_CountMaskBits(U32 mask);
extern U32 Image_CountMaskBitShift(U32 mask);
extern COLORREF Image_RGB2CR(U8 r, U8 g, U8 b);
extern void Image_CR2RGB(COLORREF cr, U8* r, U8* g, U8* b);
extern U32 Image_DDColorMatch(LPDIRECTDRAWSURFACE4 pdds, U32 rgb);

extern void Image_SetupTrans(lpImage image, F32 lowr, F32 lowg, F32 lowb, F32 highr, F32 highg, F32 highb);

extern void* Image_LockSurface(lpImage image, U32* pitch, U32* bpp);
extern void Image_UnlockSurface(lpImage image);
extern U32 Image_GetPen255(lpImage image);
extern void Image_SetPenZeroTrans(lpImage image);
extern U32 Image_GetPixelMask(lpImage image);

extern B32 Image_SaveBMP(lpImage image, const char* fname);
