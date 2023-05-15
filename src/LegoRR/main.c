#include <stdio.h>
#include <windows.h>
#include "main.h"
#include "registry.h"
#include "error.h"
#include "mem.h"
#include "config.h"
#include "input.h"
#include "directdraw.h"
#include "sound.h"
#include "init.h"
#include "animation.h"
#include "draw.h"
#include "lego.h"

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
    Mem_Initialize();
    File_Initialize(mainGlobs.programName, insistOnCD, "SOFTWARE\\\\LEGO Media\\\\Games\\\\Rock Raiders");
    Config_Initialize();
    Input_InitKeysAndDI();
    if (Main_InitApp(hInstance))
    {
        DirectDraw_Initialize(mainGlobs.hWnd);
        if (Sound_Initialize(nosound) && Init_Initialize(TRUE, mainGlobs.flags & MAIN_FLAG_DEBUGMODE, mainGlobs.flags & MAIN_FLAG_BEST, mainGlobs.flags & MAIN_FLAG_WINDOW, errorMessage))
        {
            Animation_Initialize(directDrawGlobs.lpDirectDraw);
            Draw_Initialize(NULL);
            Gods_Go(mainGlobs.programName);

            if (mainGlobs.stateSet)
            {
                if (mainGlobs.currState.Initialize != NULL && !mainGlobs.currState.Initialize())
                {
                    mainGlobs.currState.Initialize = NULL;
                    mainGlobs.currState.MainLoop = NULL;
                    mainGlobs.currState.Shutdown = NULL;
                }

                if (mainGlobs.currState.MainLoop != NULL)
                {
                    // TODO: Finish Implementing WinMain
                }

                if (mainGlobs.currState.Shutdown != NULL)
                    mainGlobs.currState.Shutdown();
            }
        }
        DirectDraw_Shutdown();
        DestroyWindow(mainGlobs.hWnd);
    }

    Input_ReleaseKeysAndDI();
    Config_Shutdown();
    Mem_Shutdown(FALSE);
    Error_CloseLog();
    Error_Shutdown();

    return 0;
}

void Main_ParseCommandLine(const char* lpszCmdLine, B32* out_nosound, B32* out_insistOnCD)
{
    // TODO: Implement Main_ParseCommandLine
}

LRESULT CALLBACK Main_WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    // TODO: Implement Main_WndProc
    return DefWindowProc(hWnd, message, wParam, lParam);
}

B32 Main_InitApp(HINSTANCE hInstance)
{
    WNDCLASSA wndClass;
    wndClass.style = CS_DBLCLKS;
    wndClass.lpfnWndProc = Main_WndProc;
    wndClass.cbClsExtra = 0;
    wndClass.cbWndExtra = 0;
    wndClass.hInstance = hInstance;
    wndClass.hIcon = NULL;
    wndClass.hCursor = NULL;
    wndClass.hbrBackground = NULL;
    wndClass.lpszMenuName = NULL;
    wndClass.lpszClassName = mainGlobs.className;
    if (!RegisterClassA(&wndClass))
    {
        MessageBoxA(NULL, "\"Unable to register window class", "Fatal Error", MB_OK);
        return FALSE;
    }

    mainGlobs.hWnd = CreateWindowExA(WS_EX_APPWINDOW, mainGlobs.className, "", WS_POPUP | WS_SYSMENU, 0, 0, 100, 100, NULL, NULL, hInstance, NULL);
    if (!mainGlobs.hWnd)
    {
        MessageBoxA(NULL, "Unable to Create Main Window", "Fatal Error", MB_OK);
        return FALSE;
    }

    SetFocus(mainGlobs.hWnd);
    return TRUE;
}

U32 Main_GetWindowsBitDepth()
{
    return GetDeviceCaps(GetDC(Main_hWnd()), BITSPIXEL);
}
