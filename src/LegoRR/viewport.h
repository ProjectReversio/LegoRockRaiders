#pragma once

#include "platform.h"
#include "container.h"

#define VIEWPORT_MAXLISTS 32

#define VIEWPORT_FLAG_INITIALIZED 0x1

#ifdef _DEBUG
#define Viewport_CheckInit()    if (!(viewportGlobs.flags & VIEWPORT_FLAG_INITIALIZED)) Error_Fatal(TRUE, "Error: Viewport_Initialize() Has Not Been Called");
#else
#define Viewport_CheckInit()
#endif

typedef struct Viewport
{
    F32 xoffset, yoffset; // Position on viewport normalised to between 0.0 and 1.0
    F32 width, height; // Width and height of the viewport normalised as above
    LPDIRECT3DRMVIEWPORT2 lpVP; // Pointer to D3D viewport interface
    F32 smoothFOV;
    B32 rendering;

    struct Viewport* nextFree;
} Viewport, *lpViewport;

typedef struct Viewport_Globs
{
    lpViewport listSet[VIEWPORT_MAXLISTS];
    lpViewport freeList;

    U32 listCount;
    U32 flags;
} Viewport_Globs, *lpViewport_Globs;

extern Viewport_Globs viewportGlobs;



extern void Viewport_Initialize();
extern void Viewport_Shutdown();

extern lpViewport Viewport_Create(F32 xPos, F32 yPos, F32 width, F32 height, lpContainer camera);
extern lpViewport Viewport_CreatePixel(S32 xPos, S32 yPos, U32 width, U32 height, lpContainer camera);

extern void Viewport_SetBackClip(lpViewport viewport, F32 dist);

extern lpContainer Viewport_GetCamera(lpViewport view);

extern void Viewport_AddList();
extern void Viewport_RemoveAll();

extern void Viewport_Render(lpViewport vp, lpContainer root, F32 delta);
extern void Viewport_Clear(lpViewport vp, B32 full);
