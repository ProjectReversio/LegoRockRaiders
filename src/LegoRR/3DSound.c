#include "3DSound.h"
#include "error.h"
#include "sound.h"
#include "mem.h"
#include <stdio.h>

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

B32 Sound3D_CheckVolumeLimits(S32 vol)
{
    if (vol > DSBVOLUME_MAX || vol < DSBVOLUME_MIN)
    {
        Error_Warn(TRUE, "Invalid volume specified.");

        return FALSE;
    }

    return TRUE;
}

B32 Sound3D_GetFreeSoundIndex(U32* soundTableIndex)
{
    U32 loopSound;

    for (loopSound = 0; loopSound < SOUND3D_MAXSAMPLES; loopSound++)
    {
        if (!(sound3DGlobs.soundTable[loopSound].flags & SOUND3D_FLAG_USED))
        {
            *soundTableIndex = loopSound;

            return TRUE;
        }
    }

    Error_Fatal(TRUE, "Run out of samples - SOUND3D_MAXSAMPLES too small.");

    return FALSE;
}

S32 Sound3D_Load(const char* fName, B32 stream, B32 simultaneous, S32 volume)
{
    B32 okay = FALSE;
    char buffer[MAX_PATH];
    FILE* file;
    U32 freeSoundIndex;

    if (Sound3D_Initialized())
    {
        if (Sound3D_GetFreeSoundIndex(&freeSoundIndex))
        {
            sound3DGlobs.soundTable[freeSoundIndex].flags = 0;
            sprintf(buffer, "%s.wav", fName);

            if (stream)
            {
                const char* hdFileName = File_VerifyFilename(buffer);
                const char* useFile = NULL;
                char* cdFileName[FILE_MAXPATH];
                FILE* mfp;

                if (mfp = fopen(hdFileName, "r"))
                {
                    useFile = hdFileName;
                    fclose(mfp);
                } else {
                    if (File_GetCDFilePath(cdFileName, buffer))
                        useFile = cdFileName;
                }

                if (useFile)
                {
                    Error_Fatal(simultaneous, "Cannot have a multi streaming sound!");

                    // NEED TO FIND THE SIZE OF THE FILE FOR 'Sound3D_GetSamplePlayTime'
                    if ((file = fopen(useFile, "r")))
                    {
                        fseek(file, 0, SEEK_END);

                        sound3DGlobs.soundTable[freeSoundIndex].size = ftell(file);
                        sound3DGlobs.soundTable[freeSoundIndex].avgBytesPerSec = GetWaveAvgBytesPerSec(useFile);
                        sound3DGlobs.soundTable[freeSoundIndex].flags |= SOUND3D_FLAG_STREAM;

                        fclose(file);

                        okay = TRUE;
                    }
                }
            } else if (Sound3D_LoadSample(&sound3DGlobs.soundTable[freeSoundIndex], buffer, simultaneous))
            {
                sound3DGlobs.soundTable[freeSoundIndex].flags &= ~SOUND3D_FLAG_STREAM;
                if (simultaneous)
                    sound3DGlobs.soundTable[freeSoundIndex].flags |= SOUND3D_FLAG_MULTI;

                okay = TRUE;
            }

            if (okay)
            {
                strcpy(sound3DGlobs.soundTable[freeSoundIndex].fName, buffer);

                if (Sound3D_CheckVolumeLimits(volume))
                    sound3DGlobs.soundTable[freeSoundIndex].volume = volume;
                else
                    sound3DGlobs.soundTable[freeSoundIndex].volume = DSBVOLUME_MAX;

                sound3DGlobs.soundTable[freeSoundIndex].flags |= SOUND3D_FLAG_ACTIVE;
                sound3DGlobs.soundTable[freeSoundIndex].flags |= SOUND3D_FLAG_USED;

                return freeSoundIndex;
            }
        } else
            Error_Warn(TRUE, "Could not get free sound.");

        Error_Warn(TRUE, Error_Format("Cannot load sound \"%s\".", fName));
    }

    return -1;
}

B32 Sound3D_LoadSample(lpSound3D_SoundData sound, const char* fName, B32 simultaneous)
{
    HMMIO hmmioIn;
    MMCKINFO ckInRiff, ckIn;
    S32 nError;
    U32 cbActualRead;
    void* fileData;
    U32 fileSize;

    memset(sound, 0, sizeof(Sound3D_SoundData));

    if ((fileData = File_LoadBinary(fName, &fileSize)))
    {
        if ((nError = WaveOpenFile(fileData, fileSize, &hmmioIn, &sound->pwf, &ckInRiff)) != 0)
            goto err;

        if ((nError = WaveStartDataRead(&hmmioIn, &ckIn, &ckInRiff)) != 0)
            goto err;

        // Ok, size of wave data is in ckIn, allocate that buffer.
        if ((sound->data = GlobalAlloc(GMEM_FIXED, ckIn.cksize)) == NULL)
        {
            nError = ER_MEM;
            goto err;
        }

        if ((nError = WaveReadFile(hmmioIn, ckIn.cksize, sound->data, &ckIn, &cbActualRead)) != 0)
            goto err;

        sound->size = cbActualRead;

        // Close the wave file.
        if (hmmioIn != NULL)
        {
            mmioClose(hmmioIn, 0);
            hmmioIn = NULL;
        }

        if (Sound3D_CreateSoundBuffer(sound, simultaneous) == FALSE)
            goto err;
        if (Sound3D_SendSoundToBuffer(sound) == FALSE)
            goto err;

        if (sound->data != NULL)
        {
            GlobalFree(sound->data);
            sound->data = NULL;
        }

        Mem_Free(fileData);

        return TRUE;
    }

err:
    Error_Warn(TRUE, "Error loading sample.");

    if (sound->data != NULL)
    {
        GlobalFree(sound->data);
        sound->data = NULL;
    }
    return FALSE;
}

B32 Sound3D_CreateSoundBuffer(lpSound3D_SoundData sound, B32 simultaneous)
{
    DSBUFFERDESC dsbuf;
    U32 soundNum;

    memset(&dsbuf, 0, sizeof(DSBUFFERDESC));
    dsbuf.dwSize = sizeof(DSBUFFERDESC);
    dsbuf.dwFlags = DSBCAPS_CTRLPAN | DSBCAPS_CTRLVOLUME | DSBCAPS_CTRLFREQUENCY | DSBCAPS_CTRL3D | DSBCAPS_STATIC | DSBCAPS_MUTE3DATMAXDISTANCE;
    dsbuf.dwBufferBytes = sound->size;
    dsbuf.lpwfxFormat = sound->pwf;

    if (lpDSnd()->lpVtbl->CreateSoundBuffer(lpDSnd(), &dsbuf, &sound->lpDsb3D[0], NULL) != DS_OK)
    {
        Error_Warn(TRUE, "Cannot create sound buffer.");

        return FALSE;
    }

    sound->voice = 0;

    if (simultaneous)
    {
        for(soundNum = 1; soundNum < SOUND3D_MAXSIMULTANEOUS; soundNum++)
        {
            if (lpDSnd()->lpVtbl->DuplicateSoundBuffer(lpDSnd(), sound->lpDsb3D[0], &sound->lpDsb3D[soundNum]) != DS_OK)
            {
                Error_Warn(TRUE, "Cannot duplicate sound buffer.");

                return FALSE;
            }
        }
    }
    return TRUE;
}

B32 Sound3D_SendSoundToBuffer(lpSound3D_SoundData sound)
{
    void* lpvPtr1;
    U32 dwBytes1 = 0;
    void* lpvPtr2;
    U32 dwBytes2 = 0;
    HRESULT hr;

    hr = sound->lpDsb3D[0]->lpVtbl->Lock(sound->lpDsb3D[0], sound->offset, sound->size, &lpvPtr1, &dwBytes1, &lpvPtr2, &dwBytes2, 0);

    if (hr == DSERR_BUFFERLOST)
    {
        sound->lpDsb3D[0]->lpVtbl->Restore(sound->lpDsb3D[0]);
        hr = sound->lpDsb3D[0]->lpVtbl->Lock(sound->lpDsb3D[0], sound->offset, sound->size, &lpvPtr1, &dwBytes1, &lpvPtr2, &dwBytes2, 0);
    } else if (hr != DS_OK)
    {
        Error_Warn(TRUE, "Error locking sound buffer.");

        return FALSE;
    }

    CopyMemory(lpvPtr1, sound->data, dwBytes1);

    if (lpvPtr2 != NULL)
        CopyMemory(lpvPtr2, sound->data + dwBytes1, dwBytes2);

    if (sound->lpDsb3D[0]->lpVtbl->Unlock(sound->lpDsb3D[0], lpvPtr1, dwBytes1, lpvPtr2, dwBytes2) != DS_OK)
    {
        Error_Warn(TRUE, "Error unlocking sound buffer.");

        return FALSE;
    }

    if (sound->lpDsb3D[0]->lpVtbl->GetFrequency(sound->lpDsb3D[0], &sound->freq) != DS_OK)
    {
        Error_Warn(TRUE, "Error getting sound frequency buffer.");

        return FALSE;
    }

    if (sound->lpDsb3D[0]->lpVtbl->GetVolume(sound->lpDsb3D[0], &sound->volume))
    {
        Error_Warn(TRUE, "Error getting sound volume buffer.");

        return FALSE;
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

void Sound3D_Update()
{
    lpSound3D_SoundRecord next, temp = sound3DGlobs.soundRecord;
    U32 status;

    Sound3D_UpdateFrames();

    while (temp)
    {
        next = temp->next;

        temp->soundBuff->lpVtbl->GetStatus(temp->soundBuff, &status);

        if (!(status & DSBSTATUS_PLAYING))
            Sound3D_RemoveSound(temp->frame, temp->sound3DBuff);

        temp = next;
    }

    Sound3D_Stream_CheckPosition(FALSE);
    Sound3D_Stream_CheckPosition(TRUE);
}

inline void Sound3D_UpdateFrames()
{
    // TODO: Implement Sound3D_UpdateFrames
}

void Sound3D_Stream_CheckPosition(B32 looping)
{
    // TODO: Implement Sound3D_Stream_CheckPosition
}

S32 Sound3D_Play2(Sound3D_Play play, LPDIRECT3DRMFRAME3 frame, S32 soundTableIndex, B32 loop, lpPoint3F wPos)
{
    LPDIRECTSOUNDBUFFER soundBuff;
    LPDIRECTSOUND3DBUFFER sound3DBuff;
    LPDIRECT3DRMFRAME3 root;
    Point3F cPos;
    lpSound3D_SoundData sound;

    if (Sound3D_Initialized() && soundTableIndex != -1)
    {
        sound = &sound3DGlobs.soundTable[soundTableIndex];

        if (sound->flags & SOUND3D_FLAG_ACTIVE)
        {
            // Play a streamed sound
            if (sound->flags & SOUND3D_FLAG_STREAM)
            {
                const char* hdFileName = File_VerifyFilename(sound->fName);
                const char* useFile = NULL;
                char cdFileName[FILE_MAXPATH];
                FILE *mfp;

                Error_Fatal(Sound3D_Play_Normal != play, "Can only play a streaming sound normally, not 3D.");

                if ((mfp = fopen(hdFileName, "r")))
                {
                    useFile = hdFileName;
                    fclose(mfp);
                } else
                {
                    if (File_GetCDFilePath(cdFileName, sound->fName))
                        useFile = cdFileName;
                }

                if (useFile)
                {
                    Sound3D_Stream_Play(useFile, loop, sound->volume);
                    // STREAM SOUNDS CANNOT BE PLAYED MULTI SO RETURN FIRST VOICE
                    return soundTableIndex * SOUND3D_MAXSIMULTANEOUS;
                } else
                {
                    return -1;
                }
            }

            // If this is a multisound, select alternate sound buffer from list
            if (sound->flags & SOUND3D_FLAG_MULTI)
            {
                // NOTE A LOOPING/PLAYING SOUND MAY GET OVERWRITTEN IF MORE THAN 'SOUND3D_MAXSIMULTANEOUS', 'soundTableIndex' SOUNDS ARE PLAYED BEFORE THE SOUND IS STOPPED...
                if (++sound->voice >= SOUND3D_MAXSIMULTANEOUS)
                    sound->voice = 0;
                soundBuff = sound->lpDsb3D[sound->voice];
            } else
                soundBuff = sound->lpDsb3D[0];

            soundBuff->lpVtbl->QueryInterface(soundBuff, &IID_IDirectSound3DBuffer, (void**)&sound3DBuff);

            // DEFAULT SETTINGS
            {
                S32 volume = sound->volume;
                if (play == Sound3D_Play_Normal)
                    volume += -800;
                soundBuff->lpVtbl->SetVolume(soundBuff, volume);
            }

            sound3DBuff->lpVtbl->SetMinDistance(sound3DBuff, sound3DGlobs.minDistanceForAttenuation, DS3D_DEFERRED);
            sound3DBuff->lpVtbl->SetMaxDistance(sound3DBuff, sound3DGlobs.maxDistance, DS3D_DEFERRED);

            if (play == Sound3D_Play_OnFrame)
            {
                // TODO: Implement Sound3D_Play2
            } else if (play == Sound3D_Play_OnPos)
            {
                // TODO: Implement Sound3D_Play2
            } else if (play == Sound3D_Play_Normal)
            {
                sound3DBuff->lpVtbl->SetMode(sound3DBuff, DS3DMODE_DISABLE, DS3D_DEFERRED);

                Sound3D_CheckAlreadyExists(NULL, sound3DBuff);

                Sound3D_AddSoundRecord(NULL, soundBuff, sound3DBuff);
            }

            if (lp3DListenerInfo()->lpVtbl->CommitDeferredSettings(lp3DListenerInfo()) != DS_OK)
                Error_Warn(TRUE, "DirectSound 3D failed to commit listener info settings.");

            soundBuff->lpVtbl->SetCurrentPosition(soundBuff, 0);
            if (loop)
                soundBuff->lpVtbl->Play(soundBuff, 0, 0, DSBPLAY_LOOPING);
            else
                soundBuff->lpVtbl->Play(soundBuff, 0, 0, 0);

            // RETURN THE INDEX OF THE BUFFER IN THE ARRAY OF DUPLICATED BUFFERS OF SOUND TYPE 'soundTableIndex'
            // JUST RETURN THE FIRST BUFFER INDEX IF THE SAMPLE IS NOT MULTI - DUPLICATE DON'T EXIST
            return ((soundTableIndex*SOUND3D_MAXSIMULTANEOUS) + sound->voice);
        }
    }

    return -1;
}

B32 Sound3D_Stream_Play(const char* fName, B32 loop, S32 volume)
{
    lpSound3D_StreamData streamData;

    if (loop)
        streamData = &sound3DGlobs.loopStreamData;
    else
        streamData = &sound3DGlobs.streamData;

    if (streamData->playing || streamData->fileOpen)
        Sound3D_Stream_Stop(loop);

    if (Sound3D_Stream_BufferSetup(fName, loop, volume))
    {
        // Ensure that position is at 0, ready to go
        lpDSStreamBuff(loop)->lpVtbl->SetCurrentPosition(lpDSStreamBuff(loop), 0);

        // MUST BE PLAYED LOOPING ON STREAM BUFFER
        lpDSStreamBuff(loop)->lpVtbl->Play(lpDSStreamBuff(loop), 0, 0, DSBPLAY_LOOPING);

        streamData->playing = TRUE;
        return TRUE;
    } else {
        streamData->playing = FALSE;
        return FALSE;
    }
}

B32 Sound3D_Stream_BufferSetup(const char* waveFName, B32 loop, S32 volume)
{
    // TODO: Implement Sound3D_Stream_BufferSetup
    return FALSE;
}

B32 Sound3D_Stream_FillDataBuffer(B32 looping)
{
    // TODO: Implement Sound3D_Stream_FillDataBuffer
    return FALSE;
}

B32 Sound3D_CheckAlreadyExists(LPDIRECT3DRMFRAME3 frame, LPDIRECTSOUND3DBUFFER sound3DBuff)
{
    lpSound3D_SoundRecord check = sound3DGlobs.soundRecord;

    while (check)
    {
        if (check->sound3DBuff == sound3DBuff)
        {
            // REMOVE THE SOUND BECAUSE IT WILL BE USED ELSEWHERE
            Sound3D_RemoveSound(check->frame, check->sound3DBuff);

            return TRUE;
        }

        check = check->next;
    }

    return FALSE;
}

void Sound3D_AddSoundRecord(LPDIRECT3DRMFRAME3 frame, LPDIRECTSOUNDBUFFER soundBuff, LPDIRECTSOUND3DBUFFER sound3DBuff)
{
    // TODO: Implement Sound3D_AddSoundRecord
}

void Sound3D_RemoveSound(LPDIRECT3DRMFRAME3 owner, LPDIRECTSOUND3DBUFFER sound3DBuff)
{
    // TODO: Implement Sound3D_RemoveSound
}

void Sound3D_StopAllSounds()
{
    // TODO: Implement Sound3D_StopAllSounds
}
