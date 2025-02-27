#pragma once

#include "platform.h"
#include "maths.h"
#include <d3drm.h>
#include <dsound.h>

#define SOUND3D_MAXSAMPLES 600
#define SOUND3D_MAXSIMULTANEOUS 3
#define SOUND3D_MAXUPDATEFRAMES 20
#define SOUND3D_DEFAULTSOUNDVOLUME -300

#define SOUND3D_FLAG_USED 0x1
#define SOUND3D_FLAG_ACTIVE 0x2
#define SOUND3D_FLAG_MULTI 0x4
#define SOUND3D_FLAG_STREAM 0x8

#define SOUND3D_NUMOFPLAYNOTIFICATIONS 4

typedef struct Sound3D_WaveDataTAG
{
    WAVEFORMATEX *pwfx; // Wave Format data structure
    HMMIO hmmio; // MM I/O handle for the WAVE
    MMCKINFO mmck; // Multimedia RIFF chunk
    MMCKINFO mmckInRIFF; // Use in opening a WAVE file

    U32 dwBufferSize; // Size of the entire buffer
    U32 dwNotifySize; // Size of each notification period
    U32 dwNextWriteOffset; // Offset to next buffer segment
    U32 dwProgress; // Used with above to show progress
    U32 dwNextProgressCheck;
    U32 dwLastPos; // the last play position returned by GetCurrentPos().

    B32 bDonePlaying; // Signals early abort to timer
    B32 bLoopFile; // Should we loop playback?
    B32 bFoundEnd; // Timer found file end
} Sound3D_WaveData, *lpSound3D_WaveData;

typedef struct Sound3D_StreamDataTAG
{
    B32 fileOpen;

    Sound3D_WaveData wiWave;

    B32 playing;
} Sound3D_StreamData, *lpSound3D_StreamData;

typedef struct Sound3D_SoundRecordTAG
{
    LPDIRECT3DRMFRAME3 frame;

    LPDIRECTSOUNDBUFFER soundBuff;
    LPDIRECTSOUND3DBUFFER sound3DBuff;

    struct Sound3D_SoundRecordTAG* next;

} Sound3D_SoundRecord, *lpSound3D_SoundRecord;

typedef struct Sound3D_SoundFrameRecordTAG
{
    LPDIRECTSOUND3DBUFFER sound3DBuff;

    D3DVECTOR pos;

    struct Sound3D_SoundFrameRecordTAG* next;
} Sound3D_SoundFrameRecord, *lpSound3D_SoundFrameRecord;

typedef struct Sound3D_SoundDataTAG
{
    char fName[MAX_PATH];
    U32 size;
    U32 avgBytesPerSec; // ONLY SET FOR STREAMING BUFFERS
    U32 freq;
    S32 volume;
    U32 offset;
    U8* data;
    WAVEFORMATEX *pwf;
    LPDIRECTSOUNDBUFFER lpDsb3D[SOUND3D_MAXSIMULTANEOUS];
    U32 voice;

    U32 flags;

} Sound3D_SoundData, *lpSound3D_SoundData;

typedef enum Sound3D_PlayTAG
{
    Sound3D_Play_OnFrame,
    Sound3D_Play_OnPos,
    Sound3D_Play_Normal
} Sound3D_Play;

typedef struct Sound3D_GlobsTAG
{
    LPDIRECTSOUND lpDSnd;
    LPDIRECTSOUNDBUFFER lpDSBuff;
    LPDIRECTSOUND3DLISTENER lp3DListenerInfo;
    LPDIRECTSOUNDBUFFER lpDSStreamBuff;
    LPDIRECTSOUNDBUFFER lpDSLoopStreamBuff;

    Sound3D_StreamData streamData;
    Sound3D_StreamData loopStreamData;

    lpSound3D_SoundRecord soundRecord;

    LPDIRECT3DRMFRAME3 listenerFrame;

    F32 minDistanceForAttenuation;
    F32 maxDistance;
    S32 volume;

    Sound3D_SoundData soundTable[SOUND3D_MAXSAMPLES];

    B32 initialized;

    S32 windowsVolume;

    U32 flags;

    LPDIRECT3DRMFRAME3 updateFrameList[SOUND3D_MAXUPDATEFRAMES];
} Sound3D_Globs, *lpSound3D_Globs;

extern Sound3D_Globs sound3DGlobs;

inline LPDIRECTSOUND lpDSnd() { return sound3DGlobs.lpDSnd; }
inline LPDIRECTSOUNDBUFFER lpDSBuff() { return sound3DGlobs.lpDSBuff; }
inline LPDIRECTSOUND3DLISTENER lp3DListenerInfo() { return sound3DGlobs.lp3DListenerInfo; }
inline LPDIRECTSOUNDBUFFER lpDSStreamBuff(B32 looping) { return looping ? sound3DGlobs.lpDSLoopStreamBuff : sound3DGlobs.lpDSStreamBuff; }

extern B32 Sound3D_Initialize(HWND hwndParent);
#define Sound3D_Initialized() sound3DGlobs.initialized

extern B32 Sound3D_CheckVolumeLimits(S32 vol);
extern B32 Sound3D_GetFreeSoundIndex(U32* soundTableIndex);
extern S32 Sound3D_Load(const char* fName, B32 stream, B32 simultaneous, S32 volume);
extern B32 Sound3D_LoadSample(lpSound3D_SoundData sound, const char* fName, B32 simultaneous);
extern B32 Sound3D_CreateSoundBuffer(lpSound3D_SoundData sound, B32 simultaneous);
extern B32 Sound3D_SendSoundToBuffer(lpSound3D_SoundData sound);

extern void Sound3D_SetMaxDist(F32 dist);
extern void Sound3D_SetMinDistForAtten(F32 dist);
extern F32 Sound3D_SetRollOffFactor(F32 rollOff);
extern void Sound3D_SetVolumeToDefault();

extern void Sound3D_MakeListener(LPDIRECT3DRMFRAME3 frame);

extern B32 Sound3D_Stream_Stop(B32 looping);

extern void Sound3D_Update();
extern inline void Sound3D_RegisterUpdateFrame(LPDIRECT3DRMFRAME3 frame);
extern inline void Sound3D_RemoveUpdateFrame(LPDIRECT3DRMFRAME3 frame);
extern inline void Sound3D_UpdateFrames();
extern void Sound3D_Stream_CheckPosition(B32 looping);

extern void Sound3D_UpdateListener();
extern void Sound3D_ListenerCallback(LPDIRECT3DRMFRAME3 obj, void* arg, F32 delta);

extern S32 Sound3D_Play2(Sound3D_Play play, LPDIRECT3DRMFRAME3 frame, S32 soundTableIndex, B32 loop, lpPoint3F wPos);
extern B32 Sound3D_Stream_Play(const char* fName, B32 loop, S32 volume);

extern B32 Sound3D_Stream_BufferSetup(const char* waveFName, B32 loop, S32 volume);
extern B32 Sound3D_Stream_FillDataBuffer(B32 looping);

extern B32 Sound3D_CheckAlreadyExists(LPDIRECT3DRMFRAME3 frame, LPDIRECTSOUND3DBUFFER sound3DBuff);

extern F32 Sound3D_GetSamplePlayTime(S32 handle);

extern void Sound3D_AddSoundRecord(LPDIRECT3DRMFRAME3 frame, LPDIRECTSOUNDBUFFER soundBuff, LPDIRECTSOUND3DBUFFER sound3DBuff);
extern B32 Sound3D_RecurseRemoveSoundRecord(LPDIRECT3DRMFRAME3 owner, LPDIRECTSOUND3DBUFFER sound3DBuff, lpSound3D_SoundRecord record);
extern void Sound3D_RemoveSoundRecord(LPDIRECT3DRMFRAME3 frame, LPDIRECTSOUND3DBUFFER sound3DBuff);
extern B32 Sound3D_RecurseRemoveSound(LPDIRECT3DRMFRAME3 owner, LPDIRECTSOUND3DBUFFER sound3DBuff, lpSound3D_SoundFrameRecord record);
extern void Sound3D_RemoveSound(LPDIRECT3DRMFRAME3 owner, LPDIRECTSOUND3DBUFFER sound3DBuff);
extern void Sound3D_AttachSound(LPDIRECT3DRMFRAME3 frame, LPDIRECTSOUND3DBUFFER sound3DBuff);

extern LPDIRECTSOUNDBUFFER Sound3D_GetSoundBuffer(S32 soundHandle);

extern void Sound3D_StopSound(S32 soundHandle);

extern void Sound3D_StopAllSounds();

extern void Sound3D_SoundCallback(LPDIRECT3DRMFRAME3 tFrame, void* arg, F32 delay);

extern void Sound3D_SetWorldPos(LPDIRECTSOUND3DBUFFER sound3DBuff, lpPoint3F wPos, lpPoint3F vel);

extern B32 Sound3D_D3DVectorEqual(LPD3DVECTOR a, LPD3DVECTOR b);
