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
extern void Error_CloseLog();
extern void Error_Shutdown();

extern void Error_FullScreen(B32 on);

extern char* Error_Format(char *msg, ...);

// TODO: These are for release mode only, we need to implement debug versions.
#define Error_Warn(b,s)     (b)
#define Error_Fatal(b,s)    (b)
#define Error_Debug(s)
inline void Error_CheckWarn(B32 check) {}
#define Error_LogLoad(b,s)          (b)
#define Error_LogLoadError(b,s)     (b)
#define Error_LogRedundantFile(b,s) (b)
