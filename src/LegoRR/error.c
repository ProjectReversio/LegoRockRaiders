#include "error.h"
#include <windows.h>
#include <stdio.h>

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
    // TODO: Implement Error_CloseLog
}

void Error_Shutdown()
{
    // TODO: Implement Error_Shutdown
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
