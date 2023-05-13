#pragma once

#include "types.h"
#include "directdraw.h"

typedef struct Init_Globs
{
    U32 driverCount;
    U32 deviceCount;
    U32 modeCount;
    Graphics_Driver drivers[DIRECTDRAW_MAXDRIVERS];
    Graphics_Device devices[DIRECTDRAW_MAXDEVICES];
    Graphics_Mode modes[DIRECTDRAW_MAXMODES];
    Graphics_Driver* selDriver;
    Graphics_Device* selDevice;
    Graphics_Mode* selMode;
    B32 selFullScreen;
    B32 wasFullScreen;
    Graphics_Mode validModes[DIRECTDRAW_MAXMODES];
    U32 validModeCount;
} Init_Globs, *lpInit_Globs;

extern Init_Globs initGlobs;

extern B32 Init_Initialize(B32 setup, B32 debug, B32 best, B32 window, const char* noHALMsg);
extern void Init_AddValidMode(U32 width, U32 height, U32 depth);
