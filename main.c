#include <stdio.h>
#include <windows.h>
#include "main.h"

Main_Globs mainGlobs = { NULL };

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
    BOOL nosound = FALSE;
    BOOL insistOnCD = FALSE;

    char mutexName[128];
    sprintf(mutexName, "%s Mutex", "Lego Rock Raiders");
    CreateMutexA(NULL, TRUE, mutexName);
    if (GetLastError() == ERROR_ALREADY_EXISTS)
    {
        //MessageBoxA(NULL, "Lego Rock Raiders is already running.", "Error", MB_OK | MB_ICONERROR);
        return 0;
    }

    LPSTR cmd;
    CHAR c;
    const char *i;
    UCHAR *it;
    UCHAR c_1;
    UCHAR *it_1;
    char *j;
    char *v12;

    cmd = GetCommandLineA();
    c = *cmd;
    for ( i = cmd; c; ++cmd )
    {
        if ( c == '\\' )
            i = cmd + 1;
        c = cmd[1];
    }
    strcpy(mainGlobs.programName, i);
    it = mainGlobs.programName;
    if ( mainGlobs.programName[0] )
    {
        do
        {
            if ( *it == '"' )
                *it = 0;
        }
        while ( *++it );
    }
    c_1 = mainGlobs.programName[0];
    it_1 = mainGlobs.programName;
    for ( j = mainGlobs.programName; c_1; ++it_1 )
    {
        if ( c_1 == '.' )
            j = (it_1 + 1);
        c_1 = it_1[1];
    }
    if ( j != mainGlobs.programName )
    {
        _strupr(j);
        v12 = strstr(mainGlobs.programName, ".EXE");
        if ( v12 )
            *v12 = 0;
    }

    mainGlobs.className = mainGlobs.programName;
    mainGlobs.active = FALSE;
    mainGlobs.exit = FALSE;
    mainGlobs.stateSet = FALSE;
    mainGlobs.hInst = hInstance;
    mainGlobs.fixedFrameTiming = 0.0f;
    mainGlobs.flags = MAIN_FLAG_NONE;

    // TODO: Finish Implementing WinMain

    return 0;
}
