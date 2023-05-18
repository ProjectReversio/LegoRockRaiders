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

void Error_Out(B32 errFatal, const char* lpOutputString, ...)
{
    if (errorGlobs.dumpFile)
    {
        va_list args;
        U8 msg[512];
        va_start(args, lpOutputString);
        File_VPrintF(errorGlobs.dumpFile, lpOutputString, args);
        vsprintf(msg, lpOutputString, args);
        va_end(args);
        File_Flush(errorGlobs.dumpFile);
        if (errFatal)
            Error_TerminateProgram(msg);
    } else {

        HANDLE heventDBWIN;  /* DBWIN32 synchronization object */
        HANDLE heventData;   /* data passing synch object */
        HANDLE hSharedFile;  /* memory mapped file shared data */
        LPSTR lpszSharedMem;
        char achBuffer[500];

        /* create the output buffer */
        va_list args;
        va_start(args, lpOutputString);
        vsprintf(achBuffer, lpOutputString, args);
        va_end(args);

        /*
        Do a regular OutputDebugString so that the output is
        still seen in the debugger window if it exists.

          This ifdef is necessary to avoid infinite recursion
          from the inclusion of W95TRACE.H
        */
#ifdef _cplusplus
        #ifdef _UNICODE
		::OutputDebugStringW(achBuffer);
#else
		::OutputDebugStringA(achBuffer);
#endif
#else
#ifdef _UNICODE
        OutputDebugStringW(achBuffer);
#else
        OutputDebugStringA(achBuffer);
#endif
#endif
        // TEMP: Not in original source, but needed for CLion console output
        {
            setvbuf(stdout, NULL, _IONBF, 0);
            printf("%s", achBuffer);
        }

        // TODO: Finish Implementing Error_Out

        if (errFatal)
            Error_TerminateProgram(achBuffer);
    }
}

void Error_Log(lpFile logFile, B32 log, const char* lpOutputString, ...)
{
    if (log)
    {
        if (logFile)
        {
            va_list args;

            U8 msg[512];

            va_start(args, lpOutputString);
            File_VPrintF(logFile, lpOutputString, args);
            vsprintf(msg, lpOutputString, args);
            va_end(args);
            File_Flush(logFile);
        }
    }
}

B32 Error_SetDumpFile(const char* errors, const char* loadLog, const char* loadErrorLog, const char* redundantLog)
{
    B32 ok = TRUE;

    if (loadLog)
    {
        if (!(errorGlobs.loadLogFile = File_Open(loadLog, "w")))
            ok = FALSE;
        else
            strcpy(errorGlobs.loadLogName, loadLog);
    }

    if (loadErrorLog)
    {
        if (!(errorGlobs.loadErrorLogFile = File_Open(loadErrorLog, "w")))
            ok = FALSE;
        else
            Error_LogLoadError(TRUE, "<ErrorNum>[0=InvalidFName,1=UnableToOpen,2=UnableToOpenForWrite,3=UnableToVerifyName4=TextureError] <File>");
    }

    if (errors)
    {
        if (!(errorGlobs.dumpFile = File_Open(errors, "w")))
            ok = FALSE;
    }

    if (redundantLog)
    {
        strcpy(errorGlobs.redundantLogName, redundantLog);
    }

    return ok;
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
