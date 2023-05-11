#include <stdio.h>
#include <windows.h>
#include "main.h"
#include "registry.h"
#include "error.h"

Main_Globs mainGlobs = { NULL };

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
    B32 nosound = FALSE;
    B32 insistOnCD = FALSE;

    char mutexName[128];
    sprintf(mutexName, "%s Mutex", "Lego Rock Raiders");
    CreateMutexA(NULL, TRUE, mutexName);
    if (GetLastError() == ERROR_ALREADY_EXISTS)
    {
        //MessageBoxA(NULL, "Lego Rock Raiders is already running.", "Error", MB_OK | MB_ICONERROR);
        return 0;
    }

    // TODO: Cleanup this messy decompiled code
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

    char fullCmdLine[1024];
    char standardParameters[1024];
    if (Registry_GetValue("SOFTWARE\\LEGO Media\\Games\\Rock Raiders", "StandardParameters", REGISTRY_STRING_VALUE, standardParameters, 1024))
        sprintf(fullCmdLine, "%s %s", lpCmdLine, standardParameters);
    else
        sprintf(fullCmdLine, "%s", lpCmdLine);

    Main_ParseCommandLine(fullCmdLine, &nosound, &insistOnCD);

    char errorMessage[1024];
    if (!Registry_GetValue("SOFTWARE\\LEGO Media\\Games\\Rock Raiders", "NoHALMessage", REGISTRY_STRING_VALUE, errorMessage, 1024))
        sprintf(errorMessage, "No DirectX 3D accelerator could be found.");

    Error_Initialize();

    // TODO: Finish Implementing WinMain

    return 0;
}

void Main_ParseCommandLine(const char* lpszCmdLine, B32* out_nosound, B32* out_insistOnCD)
{
    // TODO: Implement Main_ParseCommandLine
}
