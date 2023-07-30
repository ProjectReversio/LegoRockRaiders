#pragma once

#include <ddraw.h>
#include "platform.h"
#include "maths.h"
#include "file.h"

#define FLICFINISHED 1
#define FLICNOERROR  0
#define FLICERROR    (-1)

typedef enum FlicUserFlags
{
    FLICDISK      = 0,
    FLICMEMORY    = 0x1,
    FLICLOOPINGON = 0x2,
    FLICCATCHUPON = 0x4,
    FLICSOUNDOFF  = 0x8,
} FlicUserFlags;

typedef enum FlicMode
{
    FLICMODE_BYTEPERPIXEL,
    FLICMODE_MODEX,
    FLICMODE_PLANAR,
    FLICMODE_TRUECOLOR,
    FLICMODE_HICOLOR
} FlicMode;

typedef struct FLICHEADERSTRUCT
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
} FLICHEADERSTRUCT;

typedef struct Flic
{
    FlicUserFlags userFlags;

    S32 fsXc, fsYc;
    S32 fsXsize, fsYsize;
    char* rambufferhandle;
    char* destportalhandle;

    char filename[256];
    U8 fsPalette256[(256 * 3)];
    U16 fsPalette64k[256];

    S32 framerate, lastticks;
    S32 currentframe, overallframe;
    S32 mode;
    S32 ringframe;
    S32 pointerposition;

    U32 fsPitch;

    LPDIRECTDRAWSURFACE4 fsSurface;
    FLICHEADERSTRUCT fsHeader;
    lpFile filehandle;

    void* fsSPtr;
    void* fsSource;
    S32 fsDisplayMode;
    S32 fsBitPlanes;
    S32 fsLoadBufferSize;
    void* fsLoadBuffer;
    B32 is15bit;

} Flic, *lpFlic;

extern B32 Flic_Setup(const char* filename, lpFlic* fsp, FlicUserFlags flags);
extern B32 Flic_LoadHeader(const char* filename, lpFlic* fsp);

extern B32 Flic_Close(lpFlic fsp);

extern U32 Flic_GetWidth(lpFlic fsp);
extern U32 Flic_GetHeight(lpFlic fsp);

extern B32 Flic_Animate(lpFlic fsp, Area2F* destArea, B32 advance, B32 trans);

extern S32 Flic_Memory(lpFlic fsp);
extern S32 Flic_Load(lpFlic fsp);

extern S32 Flic_FindChunk(lpFlic fsp);
