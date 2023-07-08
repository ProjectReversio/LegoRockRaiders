#pragma once

#include "platform.h"
#include <d3drm.h>
#include <dsound.h>

typedef struct Sound3D_SoundFrameRecordTAG
{
    LPDIRECTSOUND3DBUFFER sound3DBuff;

    D3DVECTOR pos;

    struct Sound3D_SoundFrameRecordTAG* next;
} Sound3D_SoundFrameRecord, *lpSound3D_SoundFrameRecord;

extern void Sound3D_SetMaxDist(F32 dist);
extern void Sound3D_SetMinDistForAtten(F32 dist);
extern F32 Sound3D_SetRollOffFactor(F32 rollOff);

extern void Sound3D_MakeListener(LPDIRECT3DRMFRAME3 frame);

extern B32 Sound3D_Stream_Stop(B32 looping);
