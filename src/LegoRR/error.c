#include "error.h"
#include <windows.h>
#include <stdio.h>
#include "file.h"

Error_Globs errorGlobs = { NULL };

void Error_Initialize()
{
    errorGlobs.dumpFile = NULL;
    errorGlobs.loadLogFile = NULL;
    errorGlobs.loadErrorLogFile = NULL;
    errorGlobs.redundantLogFile = NULL;
    errorGlobs.fullScreen = FALSE;
}

void Error_CloseLog()
{
    if (errorGlobs.dumpFile)
        File_Close(errorGlobs.dumpFile);
    if (errorGlobs.loadLogFile)
        File_Close(errorGlobs.loadLogFile);
    if (errorGlobs.loadErrorLogFile)
        File_Close(errorGlobs.loadErrorLogFile);

    errorGlobs.dumpFile = errorGlobs.loadLogFile = errorGlobs.loadErrorLogFile = NULL;
}

void Error_Shutdown()
{
    Error_CloseLog();

    if (errorGlobs.redundantLogFile = File_Open(errorGlobs.redundantLogName, "w"))
    {
        File_CheckRedundantFiles(errorGlobs.loadLogName);
        File_Close(errorGlobs.redundantLogFile);
    }
}

void Error_FullScreen(B32 on)
{
    errorGlobs.fullScreen = on;
}

char* Error_Format(char *msg, ...)
{
    static char res[1024];
    va_list args;

    va_start(args, msg);
    vsprintf(res, msg, args);
    va_end(args);

    return res;
}
