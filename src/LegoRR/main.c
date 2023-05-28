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

#define STANDARD_FRAMERATE 25.0f

void Main_HandleIO();
void Main_Finalize3D();

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
        return 0; // App is already running
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

//#ifdef _DEBUG
//    // Since we currently have a different name for the debug executable,
//    // we need to specify LegoRR for the program name in order to load things correctly.
//    strcpy(mainGlobs.programName, "LegoRR");
//#endif

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

    // Initialize everything (memory, window, DirectDraw, etc.)
    Error_Initialize();
    Mem_Initialize();

//#ifdef _DEBUG
//    // Log file access in debug mode
//    File_LogFileAccess(TRUE);
//#endif

#ifdef _DEBUG
    // Since we currently have a different name for the debug executable,
    // we need to specify LegoRR for the program name in order to load the right WAD files.
    File_Initialize("LegoRR", insistOnCD, "SOFTWARE\\\\LEGO Media\\\\Games\\\\Rock Raiders");
#else
    File_Initialize(mainGlobs.programName, insistOnCD, "SOFTWARE\\\\LEGO Media\\\\Games\\\\Rock Raiders");
#endif
    Config_Initialize();
    Input_InitKeysAndDI();

    if (Main_InitApp(hInstance))
    {
        DirectDraw_Initialize(mainGlobs.hWnd);
        if (Sound_Initialize(nosound) && Init_Initialize(TRUE, mainGlobs.flags & MAIN_FLAG_DEBUGMODE, mainGlobs.flags & MAIN_FLAG_BEST, mainGlobs.flags & MAIN_FLAG_WINDOW, errorMessage))
        {
            Animation_Initialize(directDrawGlobs.lpDirectDraw);
            Draw_Initialize(NULL);

            // Call the Gods_Go() function in the application
            Gods_Go(mainGlobs.programName);

            // If the game wants to run in state mode...
            if (mainGlobs.stateSet)
            {
                // Run their initialization code (if required)
                if (mainGlobs.currState.Initialize != NULL && !mainGlobs.currState.Initialize())
                {
                    // Initialization failed so drop out...
                    Error_Warn(TRUE, "Initialization function failed...");
                    mainGlobs.currState.Initialize = NULL;
                    mainGlobs.currState.MainLoop = NULL;
                    mainGlobs.currState.Shutdown = NULL;
                }

                // If? a main loop is specified then run it until it returns false...
                if (mainGlobs.currState.MainLoop != NULL)
                {
                    // Use the MultiMedia timer to give a 'realtime passed' value
                    // per frame to the main loop (in 25th's of a second)
                    F32 time = 1.0f;
                    U32 lastTime, currTime;

                    lastTime = timeGetTime();

                    while(!mainGlobs.exit)
                    {
                        // Handle windows messages and input...
                        inputGlobs.lClicked = FALSE;
                        inputGlobs.rClicked = FALSE;
                        inputGlobs.lDoubleClicked = FALSE;
                        inputGlobs.rDoubleClicked = FALSE;

                        Main_HandleIO();

                        // In fullscreen mode we will always be the active application or I will eat my hat.
                        if (mainGlobs.flags & MAIN_FLAG_FULLSCREEN)
                            mainGlobs.active = TRUE;

                        Input_ReadKeys();
                        Input_ReadMouse2();

                        // Run the main loop (pass 1.0f as the initial timing value)
                        if (!mainGlobs.currState.MainLoop(time))
                            mainGlobs.exit = TRUE;

                        // Update the device and flip the surfaces...
                        Main_Finalize3D();
                        DirectDraw_Flip();
                        mainGlobs.flags &= ~MAIN_FLAG_UPDATED;

                        if (mainGlobs.flags & MAIN_FLAG_DUMPMODE)
                        {
                            time = STANDARD_FRAMERATE / 30.0f;
                        } else if (mainGlobs.flags & MAIN_FLAG_PAUSED)
                        {
                            time = 0.0f;
                            lastTime = timeGetTime();
                        } else if (mainGlobs.fixedFrameTiming == 0.0f)
                        {
                            // Measure the time taken over the last frame (to be passed next loop)
                            currTime = timeGetTime();
                            time = ((F32)(currTime - lastTime)) / (1000.0f / STANDARD_FRAMERATE);
                            lastTime = currTime;

#ifndef _UNLIMITEDUPDATETIME
                            if (time > 3.0f)
                                time = 3.0f;
#endif // _UNLIMITEDUPATETIME
                        } else {
                            time = mainGlobs.fixedFrameTiming;
                            lastTime = timeGetTime();
                        }
                    }
                }

                if (mainGlobs.currState.Shutdown != NULL)
                    mainGlobs.currState.Shutdown();
            } else {
                Error_Warn(TRUE, "No State Set: Exiting...");
            }
        } else {
            Error_Warn(TRUE, "Initialization Failed: Exiting...");
        }
        DirectDraw_Shutdown();
        DestroyWindow(mainGlobs.hWnd);
    } else {
        Error_Warn(TRUE, "Unable to initialize window");
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
#ifdef _DEBUG // TEMP
    mainGlobs.programmerLevel = PROGRAMMER_MODE_2;
#endif
    // TODO: Implement Main_ParseCommandLine
}

void Main_Finalize3D()
{
    if (!(mainGlobs.flags & MAIN_FLAG_UPDATED))
    {
        mainGlobs.device->lpVtbl->Update(mainGlobs.device);
        mainGlobs.flags |= MAIN_FLAG_UPDATED;
    }
}

B32 Main_DispatchMessage(MSG* msg)
{
    if ((mainGlobs.flags & MAIN_FLAG_FULLSCREEN) &&
        (msg->message == WM_ACTIVATEAPP || msg->message == WM_SYSKEYDOWN || msg->message == WM_SYSKEYUP))
    {
        return FALSE;
    } else {
        return TRUE;
    }
}

void Main_HandleIO()
{
    MSG msg;

    while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
    {
        if (Main_DispatchMessage(&msg))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }
}

LRESULT Main_WndProc_Fullscreen(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    inputGlobs.lClicked = FALSE;
    inputGlobs.rClicked = FALSE;

    if (mainGlobs.windowCallback != NULL)
        mainGlobs.windowCallback(hWnd, message, wParam, lParam);

    switch (message)
    {
        // Keyboard messages
        case WM_KEYDOWN:
        case WM_KEYUP:
            return 0;

        // Mouse
        case WM_MOUSEMOVE:
            return 0;

        // Handle single or dual mouse
        case WM_LBUTTONDOWN:
        case WM_LBUTTONUP:
        case WM_RBUTTONDOWN:
        case WM_RBUTTONUP:
        {
            if (mainGlobs.flags & MAIN_FLAG_DUALMOUSE)
            {
                // Both buttons valid
                switch (message)
                {
                    case WM_LBUTTONDOWN:
                        inputGlobs.mslb = TRUE;
                        inputGlobs.lClicked = FALSE;
                        return 0;

                    case WM_LBUTTONUP:
                        inputGlobs.mslb = FALSE;
                        inputGlobs.lClicked = TRUE;
                        return 0;

                    case WM_RBUTTONDOWN:
                        inputGlobs.msrb = TRUE;
                        inputGlobs.rClicked = FALSE;
                        return 0;

                    case WM_RBUTTONUP:
                        inputGlobs.msrb = FALSE;
                        inputGlobs.rClicked = TRUE;
                        return 0;
                }
            } else {
                // Merged buttons
                switch (message)
                {
                    case WM_LBUTTONDOWN:
                    case WM_RBUTTONDOWN:
                        if (!inputGlobs.mslb)
                        {
                            inputGlobs.lClicked = FALSE;
                            inputGlobs.rClicked = FALSE;
                        }
                        inputGlobs.mslb = inputGlobs.msrb = TRUE;
                        break;
                    case WM_LBUTTONUP:
                    case WM_RBUTTONUP:
                        inputGlobs.lClicked = TRUE;
                        inputGlobs.rClicked = TRUE;
                        inputGlobs.mslb = inputGlobs.msrb = FALSE;
                        break;
                }
            }
        }

        case WM_LBUTTONDBLCLK:
            inputGlobs.lDoubleClicked = TRUE;
            return 0;
        case WM_RBUTTONDBLCLK:
            inputGlobs.rDoubleClicked = TRUE;
            return 0;

        // Exit messages
        case WM_CLOSE:
        case WM_QUIT:
        case WM_DESTROY:
            mainGlobs.exit = TRUE;
            break;

        // Window messages
        case WM_SIZE:
        case WM_MOVE:
            return 0;

        case WM_ACTIVATE:
            return 0;

        case WM_ACTIVATEAPP:
            mainGlobs.active = (B32) wParam;
            return 0;

        case WM_PAINT:
        case WM_CREATE:
        case WM_COMMAND:
            break;

        case WM_WINDOWPOSCHANGING:
        {
            WINDOWPOS *pos = (WINDOWPOS *) lParam;
            pos->flags &= ~(SWP_NOOWNERZORDER);
            return 0;
        }

        case WM_ENTERMENULOOP:
            return 0;
        case WM_EXITMENULOOP:
            return 0;
        case WM_NCACTIVATE:
            return 0;
        case WM_SYSKEYDOWN:
            return 0;
        default:
            break;
    }

    return DefWindowProc(hWnd, message, wParam, lParam);
}

LRESULT Main_WndProc_Windowed(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    if (mainGlobs.windowCallback != NULL)
        mainGlobs.windowCallback(hWnd, message, wParam, lParam);

    switch (message)
    {
        // Exit messages
        case WM_CLOSE:
        case WM_QUIT:
        case WM_DESTROY:
            mainGlobs.exit = TRUE;
            break;

        // Handle single or dual mouse
        case WM_LBUTTONDOWN:
        case WM_LBUTTONUP:
        case WM_RBUTTONDOWN:
        case WM_RBUTTONUP:
        {
            if (mainGlobs.flags & MAIN_FLAG_DUALMOUSE)
            {
                switch (message)
                {
                    case WM_LBUTTONDOWN:
                        SetCapture(hWnd);
                        inputGlobs.mslb = TRUE;
                        inputGlobs.lClicked = FALSE;
                        return 0;

                    case WM_LBUTTONUP:
                        ReleaseCapture();
                        inputGlobs.mslb = FALSE;
                        inputGlobs.lClicked = TRUE;
                        return 0;

                    case WM_RBUTTONDOWN:
                        SetCapture(hWnd);
                        inputGlobs.msrb = TRUE;
                        inputGlobs.rClicked = FALSE;
                        return 0;

                    case WM_RBUTTONUP:
                        ReleaseCapture();
                        inputGlobs.msrb = FALSE;
                        inputGlobs.rClicked = TRUE;
                        return 0;
                }
            } else {
                // Merged buttons
                switch (message)
                {
                    case WM_LBUTTONDOWN:
                    case WM_RBUTTONDOWN:
                        SetCapture(hWnd);
                        if (!inputGlobs.mslb)
                        {
                            inputGlobs.lClicked = FALSE;
                            inputGlobs.rClicked = FALSE;
                        }
                        inputGlobs.mslb = inputGlobs.msrb = TRUE;
                        break;
                    case WM_LBUTTONUP:
                    case WM_RBUTTONUP:
                        ReleaseCapture();
                        inputGlobs.lClicked = TRUE;
                        inputGlobs.rClicked = TRUE;
                        inputGlobs.mslb = inputGlobs.msrb = FALSE;
                        break;
                }
            }
        }

        case WM_LBUTTONDBLCLK:
            inputGlobs.lDoubleClicked = TRUE;
            break;

        case WM_RBUTTONDBLCLK:
            inputGlobs.rDoubleClicked = TRUE;
            break;

        case WM_ACTIVATEAPP:
            mainGlobs.active = (B32)wParam;
            return 0;

        case WM_ACTIVATE:
            break;

        default:
            return DefWindowProc(hWnd, message, wParam, lParam);
    }

    return 0L;
}

LRESULT CALLBACK Main_WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    if (mainGlobs.flags & MAIN_FLAG_FULLSCREEN)
    {
        return Main_WndProc_Fullscreen(hWnd, message, wParam, lParam);
    } else {
        return Main_WndProc_Windowed(hWnd, message, wParam, lParam);
    }
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

B32 Main_SetState(Main_State* state)
{
    if (state != NULL)
    {
        mainGlobs.currState = *state;
        mainGlobs.stateSet = TRUE;
        return TRUE;
    }

    return FALSE;
}

void Main_SetTitle(const char* title)
{
    SetWindowTextA(mainGlobs.hWnd, title);
}

void Main_LoopUpdate(B32 clear)
{
    Main_HandleIO();
    Input_ReadKeys();
    Input_ReadMouse2();

    // Update the device and flip the surfaces...
    Main_Finalize3D();
    DirectDraw_Flip();

    if (clear)
        DirectDraw_Clear(NULL, 0);
    mainGlobs.flags &= ~MAIN_FLAG_UPDATED;
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

void Main_ChangeRenderState(D3DRENDERSTATETYPE dwRenderStateType, U32 dwRenderState)
{
    lpMain_StateChangeData data;
    U32 currValue;
    HRESULT r;

    Error_Fatal(dwRenderStateType >= MAIN_MAXRENDERSTATES, "RenderState type is out of range");

    data = &mainGlobs.renderStateData[dwRenderStateType];
    r = mainGlobs.imDevice->lpVtbl->GetRenderState(mainGlobs.imDevice, dwRenderStateType, &currValue);
    Error_Fatal(r != D3D_OK, Error_Format("Failed to GetRenderState(%i)", dwRenderStateType));

    if (currValue != dwRenderState)
    {
        mainGlobs.imDevice->lpVtbl->SetRenderState(mainGlobs.imDevice, dwRenderStateType, dwRenderState);
        if (data->changed)
        {
            if (data->origValue == currValue)
                data->changed = FALSE;
        } else {
            data->origValue = currValue;
            data->changed = TRUE;
        }
    }
}

void Main_RestoreStates(B32 force)
{
    if (force)
    {
        U32 loop;
        lpMain_StateChangeData data;

        for (loop = 0; loop < MAIN_MAXRENDERSTATES; loop++)
        {
            data = &mainGlobs.renderStateData[loop];
            if (data->changed)
            {
                mainGlobs.imDevice->lpVtbl->SetRenderState(mainGlobs.imDevice, loop, data->origValue);
                data->changed = FALSE;
            }
        }
    }
}

void Main_DisableTextureManagement()
{
    if (!(mainGlobs.flags & MAIN_FLAG_FORCETEXTUREMANAGEMENT))
        mainGlobs.flags |= MAIN_FLAG_DONTMANAGETEXTURES;
}
