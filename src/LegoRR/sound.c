#include "sound.h"
#include "main.h"
#include "3DSound.h"

Sound_Globs soundGlobs = { 0 };

B32 Sound_Initialize(B32 nosound)
{
    soundGlobs.initialized = (nosound == FALSE);
    soundGlobs.loopCDTrack = -1;

    if (nosound)
        return TRUE;

    if (Sound3D_Initialize(Main_hWnd()))
        return TRUE;

    return FALSE;
}

B32 Sound_IsInitialized()
{
    return (soundGlobs.initialized && Sound3D_Initialized());
}

S32 WaveCloseReadFile(HMMIO* phmmio, WAVEFORMATEX **ppwfxSrc)
{
    if (*ppwfxSrc != NULL)
    {
        GlobalFree(*ppwfxSrc);
        *ppwfxSrc = NULL;
    }

    if (*phmmio != NULL)
    {
        mmioClose(*phmmio, 0);
        *phmmio = NULL;
    }

    return 0;
}
