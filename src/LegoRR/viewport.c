#include "viewport.h"
#include "mem.h"

void Viewport_Initialize()
{
    // TODO: Implement Viewport_Initialize
}

void Viewport_Shutdown()
{
    // TODO: Implement Viewport_Shutdown
}

lpViewport Viewport_Create(F32 xPos, F32 yPos, F32 width, F32 height, lpContainer camera)
{
    // TODO: Implement Viewport_Create
    return Mem_Alloc(sizeof(Viewport));
}

lpViewport Viewport_CreatePixel(S32 xPos, S32 yPos, U32 width, U32 height, lpContainer camera)
{
    // TODO: Implement Viewport_CreatePixel
    return Mem_Alloc(sizeof(Viewport));
}

void Viewport_SetBackClip(lpViewport viewport, F32 dist)
{
    // TODO: Implement Viewport_SetBackClip
}
