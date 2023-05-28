#pragma once

#include "types.h"
#include "maths.h"
#include "container.h"
#include "sfx.h"

extern void Lws_Initialize(const char* sharedDir, B32 (*FindSFXID)(const char* name, SFX_ID* sfxID), S32 (*PlaySample3D)(lpContainer cont, SFX_ID type, B32 loop, B32 onCont, Point3F* wPos), B32 (*SoundEnabled)());
