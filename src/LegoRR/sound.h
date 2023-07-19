#pragma once

#include "platform.h"
#include <dsound.h>

#define WAVEVERSION 1

#ifndef ER_MEM
#define ER_MEM              0xE000
#endif

#ifndef ER_CANNOTOPEN
#define ER_CANNOTOPEN       0xE100
#endif

#ifndef ER_NOTWAVEFILE
#define ER_NOTWAVEFILE      0xE101
#endif

#ifndef ER_CANNOTREAD
#define ER_CANNOTREAD       0xE102
#endif

#ifndef ER_CORRUPTWAVEFILE
#define ER_CORRUPTWAVEFILE  0xE103
#endif

#ifndef ER_CANNOTWRITE
#define ER_CANNOTWRITE      0xE104
#endif

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

extern S32 WaveOpenFile(void* fileData, U32 fileSize, HMMIO* phmmioIn, WAVEFORMATEX** ppwfxInfo, MMCKINFO* pckInRIFF);
extern S32 WaveOpenFile2(const char* pszFileName, HMMIO* phmmioIn, WAVEFORMATEX** ppwfxInfo, MMCKINFO* pckInRIFF);
extern S32 WaveStartDataRead(HMMIO* phmmioIn, MMCKINFO* pckIn, MMCKINFO* pckInRIFF);
extern S32 WaveReadFile(HMMIO hmmioIn, U32 cbRead, U8* pbDest, MMCKINFO* pckIn, U32* cbActualRead);
extern U32 GetWaveAvgBytesPerSec(const char* pszFileName);
extern S32 WaveCloseReadFile(HMMIO* phmmio, WAVEFORMATEX **ppwfxSrc);
