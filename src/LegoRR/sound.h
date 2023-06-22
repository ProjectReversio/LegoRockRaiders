#pragma once

#include "types.h"

typedef enum SoundMode
{
    SoundMode_Once  = 0,
    SoundMode_Loop  = 1,
    SoundMode_Multi = 2,
} SoundMode;

extern B32 Sound_Initialize(B32 nosound);
extern B32 Sound_IsInitialized();
