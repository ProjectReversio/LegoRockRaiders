#include "3DSound.h"
#include "error.h"
#include "sound.h"

Sound3D_Globs sound3DGlobs = { NULL };

B32 Sound3D_Initialize(HWND hwndParent)
{
    DSBUFFERDESC dsbd;
    S32 ret = IDRETRY;

    sound3DGlobs.initialized = FALSE;

    sound3DGlobs.listenerFrame = NULL;

    // Try to create the DirectSound objects until we either do it, are told to ignore it, or are told to abort
    while (ret == IDRETRY)
    {
        // Create the DirectSound object
        if (DirectSoundCreate(NULL, &sound3DGlobs.lpDSnd, NULL) == DS_OK)
        {
            // Set cooperative level - NEEDS TO BE 'DSSCL_PRIORITY' OR HIGHER TO DO 'SetFormat'
            if (lpDSnd()->lpVtbl->SetCooperativeLevel(lpDSnd(), hwndParent, DSSCL_PRIORITY) == DS_OK)
            {
                // Create a primary buffer so we can query for a 3D Listener interface
                memset(&dsbd, 0, sizeof(DSBUFFERDESC));
                dsbd.dwSize = sizeof(DSBUFFERDESC);
                dsbd.dwFlags = DSBCAPS_PRIMARYBUFFER | DSBCAPS_CTRL3D | DSBCAPS_CTRLVOLUME;
                if (lpDSnd()->lpVtbl->CreateSoundBuffer(lpDSnd(), &dsbd, &sound3DGlobs.lpDSBuff, NULL) == DS_OK)
                {
                    // Make the primary 44.1 KHz so that it sounds better
                    WAVEFORMATEX wfx;
                    wfx.wFormatTag = WAVE_FORMAT_PCM;
                    wfx.nChannels = 2;
                    wfx.wBitsPerSample = 16;
                    wfx.nSamplesPerSec = 22050;
                    wfx.nBlockAlign = wfx.wBitsPerSample / 8 * wfx.nChannels;
                    wfx.nAvgBytesPerSec = wfx.nSamplesPerSec * wfx.nBlockAlign;
                    wfx.cbSize = 0;
                    if (lpDSBuff()->lpVtbl->SetFormat(lpDSBuff(), &wfx) != DS_OK)
                        Error_Warn(TRUE, "Could not set format on primary buffer - low quality output will be used.");

                    // Get the 3D listener information
                    if (lpDSBuff()->lpVtbl->QueryInterface(lpDSBuff(), &IID_IDirectSound3DListener, (void**) &sound3DGlobs.lp3DListenerInfo) == DS_OK)
                    {
                        sound3DGlobs.lp3DListenerInfo->lpVtbl->SetDopplerFactor(sound3DGlobs.lp3DListenerInfo, D3DVAL(100.0), DS3D_IMMEDIATE);
                    } else {
                        Error_Warn(TRUE, "DirectSound 3D failed to get listener info.");
                        lpDSBuff()->lpVtbl->Release(lpDSBuff());
                        lpDSnd()->lpVtbl->Release(lpDSnd());
                        sound3DGlobs.lpDSnd = NULL;
                    }
                } else {
                    Error_Warn(TRUE, "DirectSound 3D failed to create primary buffer.");
                    lpDSnd()->lpVtbl->Release(lpDSnd());
                    sound3DGlobs.lpDSnd = NULL;
                }
            } else {
                Error_Warn(TRUE, "DirectSound 3D failed to set cooperative level.");
                lpDSnd()->lpVtbl->Release(lpDSnd());
                sound3DGlobs.lpDSnd = NULL;
            }
        }

        if (!lpDSnd())
        {
            Error_Warn(TRUE, "DirectSound 3D could not initialize.");
            //ret = MessageBox(hwndParent, "DirectSound 3D could not initialize", "Warning", MB_APPLMODAL | MB_ICONWARNING | MB_ABORTRETRYIGNORE);
            //if (ret == IDABORT)
            //    return FALSE;
            ret = IDIGNORE;
        } else {
            ret = IDOK;
            sound3DGlobs.initialized = TRUE;
        }
    }

    if (Sound3D_Initialized())
    {
        // SET THE PRIMARY BUFFER TO CONTINUOUSLY PLAY - OPTIMIZATION FOR FREQUENT STARTING AND STOPPING OF PLAYBACK
        lpDSBuff()->lpVtbl->Play(lpDSBuff(), 0, 0, DSBPLAY_LOOPING);

        // DEFAULT VALUES
        lp3DListenerInfo()->lpVtbl->SetRolloffFactor(lp3DListenerInfo(), 1, DS3D_DEFERRED);

        // DISABLE DOPPLER - DONT NEED TO SET VELOCITY ON SOUNDS
        lp3DListenerInfo()->lpVtbl->SetDopplerFactor(lp3DListenerInfo(), 0, DS3D_DEFERRED);

        if (lp3DListenerInfo()->lpVtbl->CommitDeferredSettings(lp3DListenerInfo()) != DS_OK)
            Error_Warn(TRUE, "DirectSound 3D failed to commit listener info settings.");

        sound3DGlobs.minDistanceForAttenuation = 1; // DS3D_DEFAULTMINDISTANCE;
        sound3DGlobs.maxDistance = 800; // DS3D_DEFAULTMAXDISTANCE;

        lpDSBuff()->lpVtbl->GetVolume(lpDSBuff(), &sound3DGlobs.windowsVolume);

        Sound3D_SetVolumeToDefault();
    }

    return TRUE;
}

void Sound3D_SetMaxDist(F32 dist)
{
    lpSound3D_SoundRecord temp = sound3DGlobs.soundRecord;

    sound3DGlobs.maxDistance = dist;

    while (temp)
    {
        temp->sound3DBuff->lpVtbl->SetMaxDistance(temp->sound3DBuff, dist, DS3D_IMMEDIATE);

        temp = temp->next;
    }
}

void Sound3D_SetMinDistForAtten(F32 dist)
{
    lpSound3D_SoundRecord temp = sound3DGlobs.soundRecord;

    sound3DGlobs.minDistanceForAttenuation = dist;

    while (temp)
    {
        temp->sound3DBuff->lpVtbl->SetMinDistance(temp->sound3DBuff, dist, DS3D_IMMEDIATE);

        temp = temp->next;
    }
}

F32 Sound3D_SetRollOffFactor(F32 rollOff)
{
    if (Sound3D_Initialized())
    {
        if (rollOff < DS3D_MINROLLOFFFACTOR)
            rollOff = DS3D_MINROLLOFFFACTOR;

        if (rollOff > DS3D_MAXROLLOFFFACTOR)
            rollOff = DS3D_MAXROLLOFFFACTOR;

        lp3DListenerInfo()->lpVtbl->SetRolloffFactor(lp3DListenerInfo(), rollOff, DS3D_IMMEDIATE);
    }

    return rollOff;
}

void Sound3D_SetVolumeToDefault()
{
    if (Sound3D_Initialized())
    {
        sound3DGlobs.volume = SOUND3D_DEFAULTSOUNDVOLUME;

        lpDSBuff()->lpVtbl->SetVolume(lpDSBuff(), SOUND3D_DEFAULTSOUNDVOLUME);
    }
}

void Sound3D_MakeListener(LPDIRECT3DRMFRAME3 frame)
{
    if (Sound3D_Initialized())
        sound3DGlobs.listenerFrame = frame;
}

B32 Sound3D_Stream_Stop(B32 looping)
{
    lpSound3D_StreamData streamData;

    if (looping)
        streamData = &sound3DGlobs.loopStreamData;
    else
        streamData = &sound3DGlobs.streamData;

    streamData->playing = FALSE;

    if (streamData->fileOpen)
    {
        lpDSStreamBuff(looping)->lpVtbl->Stop(lpDSStreamBuff(looping));
        WaveCloseReadFile(&streamData->wiWave.hmmio, &streamData->wiWave.pwfx);
        lpDSStreamBuff(looping)->lpVtbl->Release(lpDSStreamBuff(looping));

        streamData->fileOpen = FALSE;
    }
    return TRUE;
}