#pragma once

#include "types.h"
#include "file.h"

typedef struct Error_Globs
{
    File* dumpFile;
    File* loadLogFile;
    File* loadErrorLogFile;
    File* redundantLogFile;
    char loadLogName[1024];
    char redundantLogName[1024];
    B32 warnCalled;
    B32 fullScreen;
} Error_Globs, *lpError_Globs;

extern Error_Globs errorGlobs;

extern void Error_Initialize();
