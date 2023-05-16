#pragma once

#include "types.h"
#include "config.h"

typedef struct Lego_Globs
{
    lpConfig config;
    const char* gameName;

    // TODO: Implement Lego_Globs

} Lego_Globs, *lpLego_Globs;

extern Lego_Globs legoGlobs;

extern B32 Gods_Go(const char* programName);

extern B32 Lego_Initialize();
extern B32 Lego_Shutdown_Full();
extern B32 Lego_Shutdown_Quick();
extern B32 Lego_MainLoop(F32 time);
