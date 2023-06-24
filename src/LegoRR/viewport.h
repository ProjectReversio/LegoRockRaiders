#pragma once

#include "platform.h"
#include "container.h"

typedef struct Viewport
{
    void* temp;
    // TODO: Implement Viewport
} Viewport, *lpViewport;

extern void Viewport_Initialize();
extern void Viewport_Shutdown();
extern lpViewport Viewport_Create(F32 xPos, F32 yPos, F32 width, F32 height, lpContainer camera);
extern lpViewport Viewport_CreatePixel(S32 xPos, S32 yPos, U32 width, U32 height, lpContainer camera);

extern void Viewport_SetBackClip(lpViewport viewport, F32 dist);
