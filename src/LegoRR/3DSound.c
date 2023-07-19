#include "3DSound.h"
#include "error.h"
#include "sound.h"
#include "mem.h"
#include "container.h"
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

inline void Sound3D_RegisterUpdateFrame(LPDIRECT3DRMFRAME3 frame)
{
    U32 loop;

    for (loop = 0; loop < SOUND3D_MAXUPDATEFRAMES; loop++)
    {
        if (sound3DGlobs.updateFrameList[loop] == NULL)
        {
            sound3DGlobs.updateFrameList[loop] = frame;

            break;
        }
    }
}

inline void Sound3D_RemoveUpdateFrame(LPDIRECT3DRMFRAME3 frame)
{
    U32 loop;

    for (loop = 0; loop < SOUND3D_MAXUPDATEFRAMES; loop++)
    {
        if (frame == sound3DGlobs.updateFrameList[loop])
        {
            sound3DGlobs.updateFrameList[loop] = NULL;
        }
    }
}

inline void Sound3D_UpdateFrames()
{
    U32 loop;

    for (loop = 0; loop < SOUND3D_MAXUPDATEFRAMES; loop++)
    {
        if (sound3DGlobs.updateFrameList[loop])
            Sound3D_SoundCallback(sound3DGlobs.updateFrameList[loop], NULL, 0.0f);
    }
}

void Sound3D_Stream_CheckPosition(B32 looping)
{
    lpSound3D_StreamData streamData;
    U32 hr = DS_OK;
    U32 hRet = 0;
    U32 dwPlayPos, dwWritePos, dwPlayed;
    B32 sameLoop;

    if (looping)
        streamData = &sound3DGlobs.loopStreamData;
    else
        streamData = &sound3DGlobs.streamData;

    if (streamData->playing && streamData->fileOpen)
    {
        lpDSStreamBuff(looping)->lpVtbl->GetCurrentPosition(lpDSStreamBuff(looping), &dwPlayPos, &dwWritePos);

        if (dwPlayPos < streamData->wiWave.dwLastPos)
            dwPlayed = streamData->wiWave.dwBufferSize - streamData->wiWave.dwLastPos + dwPlayPos;
        else
            dwPlayed = dwPlayPos - streamData->wiWave.dwLastPos;

        streamData->wiWave.dwProgress += dwPlayed;
        streamData->wiWave.dwLastPos = dwPlayPos;
        sameLoop = FALSE;

        while (streamData->wiWave.dwProgress > streamData->wiWave.dwNextProgressCheck)
        {
            // A play notification has been received.
            U8* lpWrite1;
            U32 dwWrite1;
            U32 cbActual = 0;

            // ON NEXT NOTIFICATION, IF DONE PLAYING IS SET STOP THE SOUND
            if (streamData->wiWave.bDonePlaying && !sameLoop)
            {
                Sound3D_Stream_Stop(looping);
                break;
            }

            sameLoop = TRUE;

            // If the entire file has been read into the buffer, bFoundEnd will be TRUE.
            if (!streamData->wiWave.bFoundEnd)
            {
                // read in more of the file.

                //Error_Debug(Error_Format("Next write pos = %i\n", streamData->wiWave.dwNextWriteOffset));
                //Error_Debug(Error_Format("Write size = %i\n\n", streamData->wiWave.dwNotifySize));

                // Lock the buffer.
                if (lpDSStreamBuff(looping)->lpVtbl->Lock(lpDSStreamBuff(looping), streamData->wiWave.dwNextWriteOffset, streamData->wiWave.dwNotifySize, &lpWrite1, &dwWrite1, NULL, NULL, 0) != DS_OK)
                {
                    Error_Warn(TRUE, "Cannot lock stream buffer.");
                    return;
                }

                // now read the file.
                WaveReadFile(streamData->wiWave.hmmio, dwWrite1, lpWrite1, &streamData->wiWave.mmck, &cbActual);

                // if cbActual is less than the amount asked for, we may have
                // reached end of file. If we are looping, we will simply read the
                // file in again until we have written dwWrite1 bytes into the buffer.
                // If not looping we fill with silence.
                if ((U32)cbActual < dwWrite1)
                {
                    if (!streamData->wiWave.bLoopFile)
                    {
                        streamData->wiWave.bFoundEnd = TRUE;

                        // fill up the rest of the buffer with silence.
                        FillMemory(lpWrite1 + cbActual, (U32)dwWrite1 - cbActual, (U8)(streamData->wiWave.pwfx->wBitsPerSample == 8 ? 128 : 0));
                    } else
                    {
                        // we're looping. Read in the file again.
                        S32 nChkErr = 0;
                        U32 cbWritten = cbActual; // from previous call above.

                        while (cbWritten < (U32)dwWrite1) // write only one notification worth.
                        {
                            // this will keep reading in until the buffer is full. For very short files.
                            WaveStartDataRead(&streamData->wiWave.hmmio, &streamData->wiWave.mmck, &streamData->wiWave.mmckInRIFF);
                            WaveReadFile(streamData->wiWave.hmmio, (U32)dwWrite1 - cbWritten, lpWrite1 + cbWritten, &streamData->wiWave.mmck, &cbActual);
                            cbWritten += cbActual;
                        }
                    }
                }

                // unlock now
                lpDSStreamBuff(looping)->lpVtbl->Unlock(lpDSStreamBuff(looping), lpWrite1, dwWrite1, NULL, 0);

                // setup the next write offset
                streamData->wiWave.dwNextWriteOffset += dwWrite1;
                streamData->wiWave.dwNextProgressCheck += dwWrite1;

                if (streamData->wiWave.dwNextWriteOffset >= streamData->wiWave.dwBufferSize)
                    streamData->wiWave.dwNextWriteOffset -= streamData->wiWave.dwBufferSize;

            } else {
                // We have read in the whole file. We will keep filling in silence
                // in the buffer to allow the remaining sound bytes filled earlier
                // to be played.

                // Allow the rest of the bytes to be played and fill here
                // with silence. The next notification will quit the while loop.
                lpDSStreamBuff(looping)->lpVtbl->Lock(lpDSStreamBuff(looping), streamData->wiWave.dwNextWriteOffset, streamData->wiWave.dwNotifySize, &lpWrite1, &dwWrite1, NULL, NULL, 0);
                FillMemory(lpWrite1, dwWrite1, (U8)(streamData->wiWave.pwfx->wBitsPerSample == 8 ? 128 : 0));

                hr = lpDSStreamBuff(looping)->lpVtbl->Unlock(lpDSStreamBuff(looping), lpWrite1, dwWrite1, NULL, 0);

                // We don't want to cut off the sound before it's done playing.
                // ON NEXT NOTIFICATION, IF DONE PLAYING IS SET STOP THE SOUND
                if (streamData->wiWave.mmckInRIFF.cksize > streamData->wiWave.dwNotifySize &&
                    streamData->wiWave.dwProgress >= streamData->wiWave.mmckInRIFF.cksize - streamData->wiWave.dwNotifySize)
                {
                    streamData->wiWave.bDonePlaying = TRUE;
                } else if (streamData->wiWave.dwProgress >= streamData->wiWave.mmckInRIFF.cksize)
                {
                    // for short files.
                    streamData->wiWave.bDonePlaying = TRUE;
                }

                streamData->wiWave.dwNextProgressCheck += dwWrite1;
            }
        }
    }
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
                sound3DBuff->lpVtbl->SetMode(sound3DBuff, DS3DMODE_NORMAL, DS3D_DEFERRED);
                frame->lpVtbl->GetScene(frame, &root);
                Sound3D_CheckAlreadyExists(frame, sound3DBuff);
                Sound3D_AttachSound(frame, sound3DBuff);
                Sound3D_AddSoundRecord(frame, soundBuff, sound3DBuff);
                frame->lpVtbl->GetPosition(frame, root, (LPD3DVECTOR) &cPos);
                sound3DBuff->lpVtbl->SetPosition(sound3DBuff, cPos.x, cPos.y, cPos.z, DS3D_DEFERRED);
                root->lpVtbl->Release(root);
            } else if (play == Sound3D_Play_OnPos)
            {
                sound3DBuff->lpVtbl->SetMode(sound3DBuff, DS3DMODE_NORMAL, DS3D_DEFERRED);
                Sound3D_SetWorldPos(sound3DBuff, wPos, NULL);
                Sound3D_CheckAlreadyExists(NULL, sound3DBuff);
                Sound3D_AddSoundRecord(NULL, soundBuff, sound3DBuff);
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
    DSBUFFERDESC dsbd;
    S32 nChkErr;
    S32 nRem;
    lpSound3D_StreamData streamData;

    if (loop)
        streamData = &sound3DGlobs.loopStreamData;
    else
        streamData = &sound3DGlobs.streamData;

    if ((nChkErr = WaveOpenFile2(waveFName, &streamData->wiWave.hmmio, &streamData->wiWave.pwfx, &streamData->wiWave.mmckInRIFF)) != 0)
    {
        Error_Warn(TRUE, "Error opening wave file.");
        return FALSE;
    }

    if (streamData->wiWave.pwfx->wFormatTag != WAVE_FORMAT_PCM)
    {
        Error_Warn(TRUE, "Wave file not PCM.");
        WaveCloseReadFile(&streamData->wiWave.hmmio, &streamData->wiWave.pwfx);
        return FALSE;
    }

    // Seek to the data chunk. mmck.ckSize will be the size of all the data in the file.
    if ((nChkErr = WaveStartDataRead(&streamData->wiWave.hmmio, &streamData->wiWave.mmck, &streamData->wiWave.mmckInRIFF)) != 0)
    {
        Error_Warn(TRUE, "Seek failed.");
        WaveCloseReadFile(&streamData->wiWave.hmmio, &streamData->wiWave.pwfx);
        return FALSE;
    }

    // Calculate a buffer length 3 sec. long. This should be an integral number of the
    // number of bytes in one notification period.
    streamData->wiWave.dwNotifySize = streamData->wiWave.pwfx->nSamplesPerSec * 3 * (U32)streamData->wiWave.pwfx->nBlockAlign;
    streamData->wiWave.dwNotifySize = streamData->wiWave.dwNotifySize / SOUND3D_NUMOFPLAYNOTIFICATIONS;
    // the notify size should be an intergral multiple of the nBlockAlign value.
    if ((nRem = streamData->wiWave.dwNotifySize % (U32)streamData->wiWave.pwfx->nBlockAlign) != 0)
        streamData->wiWave.dwNotifySize += streamData->wiWave.pwfx->nBlockAlign - nRem;
    streamData->wiWave.dwBufferSize = streamData->wiWave.dwNotifySize * SOUND3D_NUMOFPLAYNOTIFICATIONS;

    // Create the secondary DirectSoundBuffer object to receive our sound data.
    memset(&dsbd, 0, sizeof(DSBUFFERDESC));
    dsbd.dwSize = sizeof(DSBUFFERDESC);
    // Use new GetCurrentPosition() accuracy (DirectX 2 feature)
    dsbd.dwFlags = DSBCAPS_CTRLPAN | DSBCAPS_CTRLVOLUME | DSBCAPS_CTRLFREQUENCY | DSBCAPS_GETCURRENTPOSITION2;
    dsbd.dwBufferBytes = streamData->wiWave.dwBufferSize;

    // Set Format properties according to the WAVE file we just opened
    dsbd.lpwfxFormat = streamData->wiWave.pwfx;

    {
        LPDIRECTSOUNDBUFFER newBuffer;
        if (lpDSnd()->lpVtbl->CreateSoundBuffer(lpDSnd(), &dsbd, &newBuffer, NULL) != DS_OK)
        {
            Error_Warn(TRUE, "Cannot create sound buffer.");
            return FALSE;
        }

        if (loop)
            sound3DGlobs.lpDSLoopStreamBuff = newBuffer;
        else
            sound3DGlobs.lpDSStreamBuff = newBuffer;
    }

    lpDSStreamBuff(loop)->lpVtbl->SetVolume(lpDSStreamBuff(loop), volume);

    streamData->wiWave.bFoundEnd = FALSE;
    streamData->wiWave.dwNextWriteOffset = 0;
    // KEEP STREAM WRITE 2 NOTIFICATIONS BEHIND PLAY
    // AND WRITE SINGLE NOTIFICATION BLOCKS OF DATA
    streamData->wiWave.dwNextProgressCheck = streamData->wiWave.dwNotifySize * 2;

    // ADD LOOP CAP HERE
    streamData->wiWave.bLoopFile = loop;

    // Fill data in the buffer.
    Sound3D_Stream_FillDataBuffer(loop);

    // we're set to play now.
    streamData->wiWave.bDonePlaying = FALSE;

    streamData->fileOpen = TRUE;

    return TRUE;
}

B32 Sound3D_Stream_FillDataBuffer(B32 looping)
{
    lpSound3D_StreamData streamData;
    U8 *lpWrite1, *lpWrite2;
    U32 dwLen1, dwLen2;
    U32 uActualBytesWritten;
    U32 dwBytes;

    if (looping)
        streamData = &sound3DGlobs.loopStreamData;
    else
        streamData = &sound3DGlobs.streamData;

    dwBytes = streamData->wiWave.dwBufferSize;

    // This is the initial read. So we fill the entire buffer.
    // This will not wrap around so the 2nd pointers will be NULL.
    if (lpDSStreamBuff(looping)->lpVtbl->Lock(lpDSStreamBuff(looping), 0, dwBytes, (void**)&lpWrite1, &dwLen1, (void**)&lpWrite2, &dwLen2, 0) != DS_OK)
    {
        Error_Warn(TRUE, "Cannot lock stream buffer.");
        return FALSE;
    }

    WaveReadFile(streamData->wiWave.hmmio, (U32)dwLen1, lpWrite1, &streamData->wiWave.mmck, &uActualBytesWritten);

    // if the number of bytes written is less than the
    // amount we requested, we have a short file.
    if (uActualBytesWritten < dwLen1)
    {
        if (!streamData->wiWave.bLoopFile)
        {
            // we set the bFoundEnd flag if the length is less than
            // one notify period long which is when the first notification comes in.
            // The next notification will then call send a message to process a stop.
            if (uActualBytesWritten < streamData->wiWave.dwNotifySize)
                streamData->wiWave.bFoundEnd = TRUE;

            // Fill in silence for the rest of the buffer.
            FillMemory(lpWrite1 + uActualBytesWritten, dwLen1 - uActualBytesWritten, (U8)(streamData->wiWave.pwfx->wBitsPerSample == 8 ? 128 : 0));
        } else {
            // we are looping.
            U32 uWritten = uActualBytesWritten; // from previous call above.

            while (uWritten < dwLen1)
            {
                // this will keep reading in until the buffer is full. For very short files.
                WaveStartDataRead(&streamData->wiWave.hmmio, &streamData->wiWave.mmck, &streamData->wiWave.mmckInRIFF);
                WaveReadFile(streamData->wiWave.hmmio, (U32)dwLen1 - uWritten, lpWrite1 + uWritten, &streamData->wiWave.mmck, &uActualBytesWritten);
                uWritten += uActualBytesWritten;
            }
        }
    }

    // now unlock the buffer.

    lpDSStreamBuff(looping)->lpVtbl->Unlock(lpDSStreamBuff(looping), (void*)lpWrite1, dwLen1, NULL, 0);

    streamData->wiWave.dwNextWriteOffset += dwLen1;
    // this is a circular buffer. Do mod buffersize.
    if (streamData->wiWave.dwNextWriteOffset >= streamData->wiWave.dwBufferSize)
        streamData->wiWave.dwNextWriteOffset -= streamData->wiWave.dwBufferSize;

    streamData->wiWave.dwProgress = 0;
    streamData->wiWave.dwLastPos = 0;

    return TRUE;
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
    lpSound3D_SoundRecord temp;

    temp = Mem_Alloc(sizeof(Sound3D_SoundRecord));
    temp->next = sound3DGlobs.soundRecord;
    temp->frame = frame;
    temp->sound3DBuff = sound3DBuff;
    temp->soundBuff = soundBuff;
    sound3DGlobs.soundRecord = temp;
}

B32 Sound3D_RecurseRemoveSoundRecord(LPDIRECT3DRMFRAME3 owner, LPDIRECTSOUND3DBUFFER sound3DBuff, lpSound3D_SoundRecord record)
{
    lpSound3D_SoundRecord temp;

    if (record == NULL)
        return FALSE;

    if (Sound3D_RecurseRemoveSoundRecord(owner, sound3DBuff, record->next))
    {
        temp = record->next;
        record->next = temp->next;
        Mem_Free(temp);
    }

    if (sound3DBuff == record->sound3DBuff)
        return TRUE;

    return FALSE;
}

void Sound3D_RemoveSoundRecord(LPDIRECT3DRMFRAME3 frame, LPDIRECTSOUND3DBUFFER sound3DBuff)
{
    lpSound3D_SoundRecord temp;

    if (Sound3D_RecurseRemoveSoundRecord(frame, sound3DBuff, sound3DGlobs.soundRecord))
    {
        temp = sound3DGlobs.soundRecord;
        sound3DGlobs.soundRecord = sound3DGlobs.soundRecord->next;
        Mem_Free(temp);
    }
}

B32 Sound3D_RecurseRemoveSound(LPDIRECT3DRMFRAME3 owner, LPDIRECTSOUND3DBUFFER sound3DBuff, lpSound3D_SoundFrameRecord record)
{
    lpSound3D_SoundFrameRecord temp;

    if (!record)
        return FALSE;

    if (Sound3D_RecurseRemoveSound(owner, sound3DBuff, record->next))
    {
        Sound3D_RemoveSoundRecord(owner, sound3DBuff);

        temp = record->next;
        record->next = temp->next;
        Mem_Free(temp);
    }

    if (sound3DBuff == record->sound3DBuff)
        return TRUE;

    return FALSE;
}

void Sound3D_RemoveSound(LPDIRECT3DRMFRAME3 owner, LPDIRECTSOUND3DBUFFER sound3DBuff)
{
    lpSound3D_SoundFrameRecord temp;
    Container_AppData* data;

    sound3DBuff->lpVtbl->Release(sound3DBuff);

    if (!owner)
    {
        Sound3D_RemoveSoundRecord(NULL, sound3DBuff);

        return;
    }

    data = (Container_AppData*) owner->lpVtbl->GetAppData(owner);

    if (Sound3D_RecurseRemoveSound(owner, sound3DBuff, data->soundList))
    {
        Sound3D_RemoveSoundRecord(owner, sound3DBuff);

        temp = data->soundList;
        data->soundList = data->soundList->next;

        Mem_Free(temp);
    }

    Sound3D_RemoveUpdateFrame(owner);
}

void Sound3D_AttachSound(LPDIRECT3DRMFRAME3 frame, LPDIRECTSOUND3DBUFFER sound3DBuff)
{
    Container_AppData* data = (Container_AppData*) frame->lpVtbl->GetAppData(frame);
    lpSound3D_SoundFrameRecord temp;

    temp = Mem_Alloc(sizeof(Sound3D_SoundFrameRecord));
    if (data)
        temp->next = data->soundList;
    else
        temp->next = NULL;
    temp->sound3DBuff = sound3DBuff;

    temp->pos.x = 0;
    temp->pos.y = 0;
    temp->pos.z = 0;

    Container_Frame_SetAppData(frame, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, temp, NULL);

    Sound3D_RegisterUpdateFrame(frame);
}

void Sound3D_StopAllSounds()
{
    lpSound3D_SoundRecord temp = sound3DGlobs.soundRecord;

    while (temp)
    {
        temp->soundBuff->lpVtbl->Stop(temp->soundBuff);
        temp = temp->next;
    }

    Sound3D_Stream_Stop(FALSE);
    Sound3D_Stream_Stop(TRUE);
}

void Sound3D_SoundCallback(LPDIRECT3DRMFRAME3 tFrame, void* arg, F32 delay)
{
    Container_AppData* data = (Container_AppData*)tFrame->lpVtbl->GetAppData(tFrame);
    D3DVECTOR rlvVisualInfo;
    LPDIRECT3DRMFRAME3 root;
    lpSound3D_SoundFrameRecord temp = data->soundList;

    tFrame->lpVtbl->GetScene(tFrame, &root);

    while (temp)
    {
        // GET POSITION OF THE 3D SOUND WHEN IT WAS LAST SET, THEN COMPARE IT WITH THE POSITION OF ITS FRAME
        // IF DIFFERENT, UPDATE THE SOUND POSITION
        tFrame->lpVtbl->GetPosition(tFrame, root, &rlvVisualInfo);
        if (!Sound3D_D3DVectorEqual(&rlvVisualInfo, &temp->pos))
        {
            temp->sound3DBuff->lpVtbl->SetPosition(temp->sound3DBuff, rlvVisualInfo.x, rlvVisualInfo.y, rlvVisualInfo.z, DS3D_IMMEDIATE);
            temp->pos = rlvVisualInfo;
        }

        temp = temp->next;
    }

    root->lpVtbl->Release(root);
}

void Sound3D_SetWorldPos(LPDIRECTSOUND3DBUFFER sound3DBuff, lpPoint3F wPos, lpPoint3F vel)
{
    sound3DBuff->lpVtbl->SetPosition(sound3DBuff, wPos->x, wPos->y, wPos->z, DS3D_IMMEDIATE);
}

B32 Sound3D_D3DVectorEqual(LPD3DVECTOR a, LPD3DVECTOR b)
{
    if (a->x == b->x &&
        a->y == b->y &&
        a->z == b->z)
        return TRUE;

    return FALSE;
}
