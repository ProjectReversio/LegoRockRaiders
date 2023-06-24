#pragma once

#include "platform.h"
#include <d3drm.h>

extern void Sound3D_SetMaxDist(F32 dist);
extern void Sound3D_SetMinDistForAtten(F32 dist);
extern F32 Sound3D_SetRollOffFactor(F32 rollOff);

extern void Sound3D_MakeListener(LPDIRECT3DRMFRAME3 frame);

extern B32 Sound3D_Stream_Stop(B32 looping);
