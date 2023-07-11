#pragma once

#include "platform.h"
#include <dsound.h>

#define MAX_SAMPLES 100

typedef enum SoundMode
{
    SoundMode_Once  = 0,
    SoundMode_Loop  = 1,
    SoundMode_Multi = 2,
} SoundMode;

typedef struct Sound
{
    U32 handle;
} Sound, *lpSound;

typedef void (*SoundCDStopCallback)(void);

typedef struct Sound_Globs
{
    U32 useSound;
    B32 initialized;
    Sound soundList[MAX_SAMPLES];
    S32 currTrack;
    B32 loopCDTrack;
    SoundCDStopCallback CDStopCallback;
    B32 updateCDTrack;
} Sound_Globs, *lpSound_Globs;

extern Sound_Globs soundGlobs;

extern B32 Sound_Initialize(B32 nosound);
extern B32 Sound_IsInitialized();

extern S32 WaveCloseReadFile(HMMIO* phmmio, WAVEFORMATEX **ppwfxSrc);
