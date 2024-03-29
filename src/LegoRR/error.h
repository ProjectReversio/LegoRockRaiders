#pragma once

#include "platform.h"
#include "file.h"

#ifdef __cplusplus
extern "C"
{
#endif

    typedef enum Error_LoadErrorTAG
{
    Error_LoadError_InvalidFName,
    Error_LoadError_UnableToOpen,
    Error_LoadError_UnableToOpenForWrite,
    Error_LoadError_UnableToVerifyName,
    Error_LoadError_RMTexture,

    Error_LoadError_Count
} Error_LoadErrorTAG;

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

extern void Error_Out(B32 errFatal, const char* msg, ...);
extern void Error_Log(lpFile logFile, B32 log, const char* msg, ...);

extern B32 Error_SetDumpFile(const char* errors, const char* loadLog, const char* loadErrorLog, const char* redundantLog);

extern void Error_DebugBreak();
extern void Error_TerminateProgram(const char* msg);

#ifdef _DEBUG
#define Error_Warn(b,s)             { if (b) { Error_Out(FALSE, "%s(%i): Warning: %s\n", __FILE__, __LINE__, (s)); Error_SetWarn(); } }
#define Error_Fatal(b,s)            { if (b) { Error_Out(TRUE, "%s(%i): Fatal: %s\n", __FILE__, __LINE__, (s)); } }
#define Error_Debug(s)              Error_Out(FALSE, "%s", (s))
#define Error_LogLoad(b,s)          { Error_Log(errorGlobs.loadLogFile, (b), "%s\n", (s)); }
#define Error_LogLoadError(b,s)     { Error_Log(errorGlobs.loadErrorLogFile, (b), "%s\n", (s)); }
#define Error_LogRedundantFile(b,s) { Error_Log(errorGlobs.redundantLogFile, (b), "%s\n", (s)); }
inline void Error_SetWarn() { errorGlobs.warnCalled = TRUE; }
inline void Error_CheckWarn(B32 check) { if (!check) errorGlobs.warnCalled = FALSE; else if (errorGlobs.warnCalled) Error_TerminateProgram("Check warning message log"); }
#else
#define Error_Warn(b,s)     (b)
#define Error_Fatal(b,s)    (b)
#define Error_Debug(s)
inline void Error_CheckWarn(B32 check) {}
#define Error_LogLoad(b,s)          (b)
#define Error_LogLoadError(b,s)     (b)
#define Error_LogRedundantFile(b,s) (b)
#endif

#ifdef __cplusplus
}
#endif
