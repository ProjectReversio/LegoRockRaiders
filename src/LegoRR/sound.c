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
