#pragma once

#include "platform.h"
#include "maths.h"
#include <windows.h>

typedef struct Draw_Rect
{
    Area2F rect;
    F32 r, g, b;
} Draw_Rect, *lpDraw_Rect;

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
