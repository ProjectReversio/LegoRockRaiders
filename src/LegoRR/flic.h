#pragma once

#include "platform.h"
#include "maths.h"

typedef enum FlicUserFlags
{
    FLICDISK      = 0,
    FLICMEMORY    = 0x1,
    FLICLOOPINGON = 0x2,
    FLICCATCHUPON = 0x4,
    FLICSOUNDOFF  = 0x8,
} FlicUserFlags;

typedef struct Flic
{
    // TODO: Implement Flic

    S32 fsXc;
    S32 fsYc;
    S32 fsXsize;
    S32 fsYsize;

    // TODO: Implement Flic

} Flic, *lpFlic;

extern B32 Flic_Setup(const char* filename, lpFlic* fsp, FlicUserFlags flags);
extern B32 Flic_Close(lpFlic fsp);

extern U32 Flic_GetWidth(lpFlic fsp);
extern U32 Flic_GetHeight(lpFlic fsp);

extern B32 Flic_Animate(lpFlic fsp, Area2F* destArea, B32 advance, B32 trans);
