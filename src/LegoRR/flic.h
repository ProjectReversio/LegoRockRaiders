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

struct FLICHEADERSTRUCT
{
    S32 size; // Size of FLIC including this header
    U16 magic; // File type [0x1234, 0x9119, 0xaf11, 0xaf12, 0xaf43]
    U16 frames; // Number of frames in first segment
    U16 width; // FLIC width in pixels
    U16 height; // FLIC height in pixels
    U16 depth; // Bits per pixel (usually 8)
    U16 flags; // Set to zero or to three
    U16 speed; // Delay between frames
    U16 padding1;
    S32 next;
    S32 frit;
    char expand[82];
    U16 padding2;
    S32 NewPack;
    S32 SoundID;
    S32 SoundRate;
    char SoundChannels;
    char SoundBPS;
    U16 padding3;
    S32 SoundChunkSize;
    S16 SoundNumPreRead;
    U16 padding4;
};

typedef struct Flic
{
    // TODO: Implement Flic

    S32 fsXc;
    S32 fsYc;
    S32 fsXsize;
    S32 fsYsize;

    // TODO: Implement Flic

    S32 currentframe;

    // TODO: Implement Flic

    struct FLICHEADERSTRUCT fsHeader;

    // TODO: Implement Flic

} Flic, *lpFlic;

extern B32 Flic_Setup(const char* filename, lpFlic* fsp, FlicUserFlags flags);
extern B32 Flic_Close(lpFlic fsp);

extern U32 Flic_GetWidth(lpFlic fsp);
extern U32 Flic_GetHeight(lpFlic fsp);

extern B32 Flic_Animate(lpFlic fsp, Area2F* destArea, B32 advance, B32 trans);
