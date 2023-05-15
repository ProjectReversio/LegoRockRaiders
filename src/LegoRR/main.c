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
#include "dxbug.h"

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

void Main_AdjustWindowRect(LPRECT rect)
{
    if (!(mainGlobs.flags & MAIN_FLAG_FULLSCREEN))
        AdjustWindowRect(rect, mainGlobs.style, FALSE);
}

void Main_SetupDisplay(B32 fullScreen, U32 xPos, U32 yPos, U32 width, U32 height)
{
    mainGlobs.appWidth = width;
    mainGlobs.appHeight = height;
    if (fullScreen)
        mainGlobs.flags |= MAIN_FLAG_FULLSCREEN;

    Error_FullScreen(fullScreen);

    if (!fullScreen)
    {
        // Adjust the window to any new settings...

        RECT rect;
        mainGlobs.style = WS_POPUP | WS_SYSMENU | WS_CAPTION;

        rect.left = xPos;
        rect.top = yPos;
        rect.right = xPos + width;
        rect.bottom = yPos + height;

        Main_AdjustWindowRect(&rect);

        SetWindowLong(mainGlobs.hWnd, GWL_STYLE, mainGlobs.style);
        SetWindowPos(mainGlobs.hWnd, NULL, rect.left, rect.top, rect.right - rect.left, rect.bottom - rect.top, SWP_NOZORDER);

        ShowCursor(FALSE);
    } else {
        RECT rect;
        HWND hWndDesktop = GetDesktopWindow();

        GetWindowRect(hWndDesktop, &rect);
        SetWindowPos(mainGlobs.hWnd, NULL, rect.left, rect.top, rect.right - rect.left, rect.bottom - rect.top, SWP_NOZORDER);

        SetCursor(FALSE);
    }

    ShowWindow(mainGlobs.hWnd, SW_SHOW);
    SetActiveWindow(mainGlobs.hWnd);
}



B32 Main_SetupDirect3D(lpGraphics_Device dev, LPDIRECTDRAW ddraw1, LPDIRECTDRAWSURFACE4 backSurf, B32 doubleBuffered)
{
    LPGUID guid = NULL;
    LPDIRECT3DRM lpD3DRM1;
    HRESULT r;
    LPDIRECTDRAWSURFACE surf1;

    if (dev)
    {
        guid = &dev->guid;
        if (dev->flags & GRAPHICS_DEVICE_FLAG_VIDEOTEXTURE)
            mainGlobs.flags |= MAIN_FLAG_VIDEOTEXTURE;
    }

    if (!(mainGlobs.flags & MAIN_FLAG_FORCEVERTEXFOG) && (dev->flags & GRAPHICS_DEVICE_FLAG_HARDWARE))
        Main_SetFogMethod(D3DRMFOGMETHOD_TABLE);
    else
        Main_SetFogMethod(D3DRMFOGMETHOD_VERTEX);

    // Create D3DRM and the device...

    if (Direct3DRMCreate(&lpD3DRM1) == D3DRM_OK)
    {
        if (lpD3DRM1->lpVtbl->QueryInterface(lpD3DRM1, &IID_IDirect3DRM3, &mainGlobs.lpD3DRM) == D3DRM_OK)
        {
            backSurf->lpVtbl->QueryInterface(backSurf, &IID_IDirectDrawSurface, &surf1);
            if ((r = mainGlobs.lpD3DRM->lpVtbl->CreateDeviceFromSurface(mainGlobs.lpD3DRM, guid, ddraw1, surf1, 0, &mainGlobs.device)) == D3DRM_OK)
            {
                LPDIRECT3DDEVICE2 imdev2;
                mainGlobs.device->lpVtbl->GetDirect3DDevice2(mainGlobs.device, &imdev2);
                imdev2->lpVtbl->QueryInterface(imdev2, &IID_IDirect3DDevice3, &mainGlobs.imDevice);
                imdev2->lpVtbl->Release(imdev2);

                if (doubleBuffered)
                    mainGlobs.device->lpVtbl->SetBufferCount(mainGlobs.device, 2);

                return TRUE;
            } else {
                CHKDD(r);
                Error_Warn(TRUE, "Unable to create Device");
            }
        } else {
            Error_Warn(TRUE, "Failed query for IID_IDirect3DRM3");
        }
    } else {
        Error_Warn(TRUE, "Unable to create lpD3DRM1");
    }

    CHKRM(r);

    return FALSE;
}
