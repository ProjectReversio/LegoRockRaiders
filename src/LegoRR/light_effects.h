#pragma once

#include "platform.h"
#include "config.h"
#include "container.h"

extern void LightEffects_Initialize(lpContainer rootSpotlight, lpContainer rootLight, F32 initialRed, F32 initialGreen, F32 initialBlue);

extern B32 LightEffects_Load(lpConfig config, const char* gameName);
