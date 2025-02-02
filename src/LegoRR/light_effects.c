#include "light_effects.h"

LightEffects_Globs lightGlobs = { 0 };

void LightEffects_Initialize(lpContainer rootSpotlight, lpContainer rootLight, F32 initialRed, F32 initialGreen, F32 initialBlue)
{
    lightGlobs.initialRGB.red = initialRed;
    lightGlobs.initialRGB.green = initialGreen;
    lightGlobs.initialRGB.blue = initialBlue;
    lightGlobs.currentRGB.red = initialRed;
    lightGlobs.currentRGB.green = initialGreen;
    lightGlobs.currentRGB.blue = initialBlue;
    lightGlobs.rootSpotlight = rootSpotlight;
    lightGlobs.rootLight = rootLight;

    LightEffects_ResetColor();
}

B32 LightEffects_Load(lpConfig config, const char* gameName)
{
    // TODO: Implement LightEffects_Load
    return FALSE;
}

void LightEffects_SetDimmerMode(B32 isDimoutMode)
{
    // TODO: Implement LightEffects_SetDimmerMode
}

void LightEffects_ResetColor()
{
    Container_SetColour(lightGlobs.rootSpotlight, lightGlobs.initialRGB.red, lightGlobs.initialRGB.green, lightGlobs.initialRGB.blue);
}

void LightEffects_Update(F32 elapsed)
{
    // TODO: Implement LightEffects_Update
}
