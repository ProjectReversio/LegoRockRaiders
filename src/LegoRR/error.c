#include "error.h"
#include <windows.h>
#include <stdio.h>
#include "file.h"
#include "directdraw.h"
#include "config.h"

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

void Error_Out(B32 errFatal, const char* msg, ...)
{
    // TODO: Implement Error_Out
}

void Error_DebugBreak()
{
    if (MessageBox(NULL, "Error_DebugBreak() called\nCrash to debugger?", "Debugger", MB_OKCANCEL) == IDOK)
        DebugBreak();
}

void Error_TerminateProgram(const char* msg)
{
    // Call exit 0.  Should probably terminate properly and clean everything up.
#ifdef _DEBUG
    if (!DirectDraw_FullScreen())
    {
        U8 message[1024];
        S32 r;
        sprintf(message, "Error:\n%s\nLast entry read from config: \"%s\"\n\nSelect Retry to crash to debugger", msg, Config_Debug_GetLastFind());

        ShowCursor(TRUE);

        r = MessageBox(NULL, message, "Debugger", MB_ABORTRETRYIGNORE);
        if (r == IDRETRY)
            DebugBreak();
        else if (r == IDIGNORE)
            return;
    }
#endif // _DEBUG

    Error_Shutdown();
    exit(0);
}
