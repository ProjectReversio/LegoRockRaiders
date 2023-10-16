#pragma once

#include "platform.h"
#include "config.h"
#include "container.h"

typedef struct LightEffects_Globs
{
    lpContainer rootSpotlight; // [Cont+Move] init
    lpContainer rootLight; // [Move] init (Reference used when getting position of rootSpotlight)
    ColourRGBF initialRGB; // [Color] init
    ColourRGBF currentRGB; // [Color+Blink+Fade] init+update
    ColourRGBF blinkRGBMax; // [Blink] init (cfg: BlinkRGBMax)

    // TODO: Implement LightEffects_Globs
} LightEffects_Globs;

extern LightEffects_Globs lightGlobs;

extern void LightEffects_Initialize(lpContainer rootSpotlight, lpContainer rootLight, F32 initialRed, F32 initialGreen, F32 initialBlue);

extern B32 LightEffects_Load(lpConfig config, const char* gameName);

extern void LightEffects_SetDimmerMode(B32 isDimoutMode);

extern void LightEffects_ResetColor();
