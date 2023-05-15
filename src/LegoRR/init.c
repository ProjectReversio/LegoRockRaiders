#include <stdio.h>
#include "init.h"
#include "main.h"
#include "resource/resource.h"

Init_Globs initGlobs = { NULL };

void Init_SetModeList(HWND hWndDlg);

void Init_SetFullScreen(HWND hWndDlg, B32 on)
{
    HWND hWndText;

    initGlobs.selFullScreen = on;
    Init_SetModeList(hWndDlg);
    hWndText = GetDlgItem(hWndDlg, IDC_MODELISTTEXT);
    if (on)
        SendMessageA(hWndText, WM_SETTEXT, 0, (LPARAM)"Screen Mode");
    else
        SendMessageA(hWndText, WM_SETTEXT, 0, (LPARAM)"Window Size");
}

void Init_HandleWindowButton(HWND hWndDlg)
{
    HWND hWndButton;

    if (initGlobs.selDriver->flags & GRAPHICS_DRIVER_FLAG_WINDOWOK)
    {
        hWndButton = GetDlgItem(hWndDlg, IDC_WINDOW);
        EnableWindow(hWndButton, TRUE);
        if (!initGlobs.wasFullScreen)
        {
            Init_SetFullScreen(hWndDlg, FALSE);
            hWndButton = GetDlgItem(hWndDlg, IDC_FULLSCREEN);
            SendMessageA(hWndButton, BM_SETCHECK, BST_UNCHECKED, 0);
            hWndButton = GetDlgItem(hWndDlg, IDC_WINDOW);
            SendMessageA(hWndButton, BM_SETCHECK, BST_CHECKED, 0);
        }
    } else {
        initGlobs.wasFullScreen = initGlobs.selFullScreen;
        Init_SetFullScreen(hWndDlg, TRUE);
        hWndButton = GetDlgItem(hWndDlg, IDC_WINDOW);
        SendMessageA(hWndButton, BM_SETCHECK, BST_UNCHECKED, 0);
        EnableWindow(hWndButton, FALSE);
        hWndButton = GetDlgItem(hWndDlg, IDC_FULLSCREEN);
        SendMessageA(hWndButton, BM_SETCHECK, BST_CHECKED, 0);
    }
}

void Init_SetDeviceList(HWND hWndDlg)
{
    HWND hWndList = GetDlgItem(hWndDlg, IDC_DEVICE);

    U32 loop;
    // Clear the existing list (if any)...
    while ((loop = SendMessageA(hWndList, LB_DELETESTRING, 0, 0) != -1));

    U32 best = 0;
    if (DirectDraw_EnumDevices(initGlobs.selDriver, initGlobs.devices, &initGlobs.deviceCount))
    {
        for (loop = 0; loop < initGlobs.deviceCount; loop++)
        {
            if (initGlobs.devices[loop].flags & GRAPHICS_DEVICE_FLAG_HARDWARE)
                best = loop;
            SendMessageA(hWndList, LB_ADDSTRING, 0, (LPARAM)initGlobs.devices[loop].desc);
        }
        initGlobs.selDevice = &initGlobs.devices[best];
    } else {
        SendMessageA(hWndList, LB_ADDSTRING, 0, (LPARAM)"Error: DirectX6 not installed.");
    }

    SendMessageA(hWndList, LB_SETCURSEL, best, 0);
}

B32 Init_IsValidMode(U32 mode)
{
    if (initGlobs.validModeCount)
    {
        for (U32 loop = 0; loop < initGlobs.validModeCount; loop++)
        {
            if ((initGlobs.validModes[loop].width == 0 || initGlobs.modes[mode].width == initGlobs.validModes[loop].width) &&
                (initGlobs.validModes[loop].height == 0 || initGlobs.modes[mode].height == initGlobs.validModes[loop].height) &&
                (initGlobs.validModes[loop].bitDepth == 0 || initGlobs.modes[mode].bitDepth == initGlobs.validModes[loop].bitDepth))
            {
                return TRUE;
            }
        }
        return FALSE;
    } else {
        return TRUE;
    }
}

void Init_SetModeList(HWND hWndDlg)
{
    HWND hWndList = GetDlgItem(hWndDlg, IDC_MODE);

    // Clear the existing list (if any)...
    while (SendMessageA(hWndList, LB_DELETESTRING, 0, 0) != -1);

    B32 first = TRUE;

    DirectDraw_EnumModes(initGlobs.selDriver, initGlobs.selFullScreen, initGlobs.modes, &initGlobs.modeCount);
    for (U32 loop = 0; loop < initGlobs.modeCount; loop++)
    {
        if (Init_IsValidMode(loop))
        {
            SendMessageA(hWndList, LB_ADDSTRING, 0, (LPARAM)initGlobs.modes[loop].desc);
            if (first)
            {
                initGlobs.selMode = &initGlobs.modes[loop];
                first = FALSE;
            }
        }
    }

    HWND hWndButton = GetDlgItem(hWndDlg, IDOK);

    if (first)
    {
        U8 msg[256];
        if (!initGlobs.selFullScreen)
            sprintf(msg, "No supported %i bit modes found", Main_GetWindowsBitDepth());
        else
            sprintf(msg, "No supported screen modes found");
        SendMessageA(hWndList, LB_ADDSTRING, 0, (LPARAM)msg);
        EnableWindow(hWndButton, FALSE);
    } else {
        EnableWindow(hWndButton, TRUE);
    }

    SendMessageA(hWndList, LB_SETCURSEL, 0, 0);
}

BOOL CALLBACK Init_DialogProc(HWND hWndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    if (uMsg == WM_INITDIALOG)
    {
        HWND hWndDesktop = GetDesktopWindow();
        RECT rectDesktop;
        GetWindowRect(hWndDesktop, &rectDesktop);
        RECT dlgRect;
        GetWindowRect(hWndDlg, &dlgRect);
        S32 xPos = rectDesktop.right / 2 - (dlgRect.right - dlgRect.left) / 2;
        S32 yPos = rectDesktop.bottom / 2 - (dlgRect.bottom - dlgRect.top) / 2;
        MoveWindow(hWndDlg, xPos, yPos, dlgRect.right - dlgRect.left, dlgRect.bottom - dlgRect.top, 1);

        HWND hWndButton;
        if (initGlobs.selFullScreen)
            hWndButton = GetDlgItem(hWndDlg, IDC_FULLSCREEN);
        else
            hWndButton = GetDlgItem(hWndDlg, IDC_WINDOW);
        SendMessageA(hWndButton, BM_SETCHECK, BST_CHECKED, 0);

        HWND hWndList = GetDlgItem(hWndDlg, IDC_DRIVER);
        for (U32 i = 0; i < initGlobs.driverCount; i++)
            SendMessageA(hWndList, LB_ADDSTRING, 0, (LPARAM)initGlobs.drivers[i].desc);

        SendMessageA(hWndList, LB_SETCURSEL, 0, 0);
        SetFocus(hWndList);

        Init_HandleWindowButton(hWndDlg);
        Init_SetDeviceList(hWndDlg);
        Init_SetModeList(hWndDlg);

        return FALSE;
    } else if (uMsg == WM_COMMAND)
    {
        // TODO: Implement Init_DialogProc
    }
    return FALSE;
}

B32 Init_Initialize(B32 setup, B32 debug, B32 best, B32 window, const char* noHALMsg)
{
    initGlobs.selDriver = NULL;
    initGlobs.selDevice = NULL;
    initGlobs.selMode = NULL;
#ifdef _DEBUG // For convenience, default to windowed mode in debug builds
    initGlobs.wasFullScreen = FALSE;
    initGlobs.selFullScreen = FALSE;
#else
    initGlobs.wasFullScreen = TRUE;
    initGlobs.selFullScreen = TRUE;
#endif
    initGlobs.validModeCount = 0;

    Init_AddValidMode(640, 480, 16);
    if (debug)
    {
        Init_AddValidMode(800, 600, 16);
        Init_AddValidMode(1024, 768, 16);
    }
    DirectDraw_EnumDrivers(initGlobs.drivers, &initGlobs.driverCount);

    U32 rval = 1;
    if (initGlobs.driverCount != 0)
    {
        initGlobs.selDriver = initGlobs.drivers;
        if (best == 0)
        {
            if (setup)
            {
                rval = DialogBoxParamA(mainGlobs.hInst, (LPCSTR)LEGORR_MODESELECTION_DIALOG, mainGlobs.hWnd, Init_DialogProc, 0);
            }
        } else {
            // TODO: Implement Init_Initialize
        }
        return FALSE;
    }
    // TODO: Implement Init_Initialize
    return FALSE;
}

void Init_AddValidMode(U32 width, U32 height, U32 depth)
{
    initGlobs.validModes[initGlobs.validModeCount].width = width;
    initGlobs.validModes[initGlobs.validModeCount].height = height;
    initGlobs.validModes[initGlobs.validModeCount].bitDepth = depth;
    initGlobs.validModeCount++;
}
