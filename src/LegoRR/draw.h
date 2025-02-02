#pragma once

#include "platform.h"
#include "maths.h"
#include <ddraw.h>
#include <windows.h>

typedef struct Draw_Rect
{
    Area2F rect;
    F32 r, g, b;
} Draw_Rect, *lpDraw_Rect;

typedef enum DrawEffect
{
    DrawEffect_None = 0,
    DrawEffect_XOR = 1,
    DrawEffect_HalfTrans = 2,
} DrawEffect;

typedef enum Draw_GlobFlags
{
    DRAW_GLOB_FLAG_NONE = 0,
    DRAW_GLOB_FLAG_INITIALIZED = 1,
} Draw_GlobFlags;

typedef struct Draw_Globs
{
    void (*drawPixelFunc)(S32 x, S32 y, U32 value);
    Point2F clipStart, clipEnd;
    RECT lockRect;
    void* buffer;
    U32 pitch, bpp;

    U32 redMask, greenMask, blueMask;
    U32 redBits, greenBits, blueBits;
    Draw_GlobFlags flags;
} Draw_Globs, *lpDraw_Globs;

extern Draw_Globs drawGlobs;

extern void Draw_Initialize(Area2F* window);
extern void Draw_SetClipWindow(Area2F* window);

extern void Draw_GetClipWindow(Area2F* window);

extern B32 Draw_LockSurface(LPDIRECTDRAWSURFACE4 surf, DrawEffect effect);
extern void Draw_UnlockSurface(LPDIRECTDRAWSURFACE4 surf);

extern B32 Draw_SetDrawPixelFunc(DrawEffect effect);

extern void Draw_LineListEx(Point2F* fromList, Point2F* toList, U32 count, F32 r, F32 g, F32 b, DrawEffect effect);
extern void Draw_LineActual(S32 x1, S32 y1, S32 x2, S32 y2, U32 colour);

extern void Draw_Pixel8(S32 x, S32 y, U32 value);
extern void Draw_Pixel16(S32 x, S32 y, U32 value);
extern void Draw_Pixel16XOR(S32 x, S32 y, U32 value);
extern void Draw_Pixel16HalfTrans(S32 x, S32 y, U32 value);
extern void Draw_Pixel24(S32 x, S32 y, U32 value);
extern void Draw_Pixel32(S32 x, S32 y, U32 value);

extern U32 Draw_GetColour(F32 r, F32 g, F32 b);
