#include <stdio.h>
#include "init.h"
#include "main.h"
#include "error.h"
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

B32 Init_GetMode(U8* name, U32* mode)
{
    for (U32 loop = 0; loop < initGlobs.modeCount; loop++)
    {
        if (!strcmp(initGlobs.modes[loop].desc, name))
        {
            *mode = loop;
            return TRUE;
        }
    }
    return FALSE;
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
        if (HIWORD(wParam) == LBN_SELCHANGE)
        {
            //Error_Debug(Error_Format("Selection changed on list box #%i (hWnd == 0x%x)\n", LOWORD(wParam), lParam));

            if (LOWORD(wParam) == IDC_DEVICE)
            {
                initGlobs.selDevice = &initGlobs.devices[SendMessageA((HWND)lParam, LB_GETCURSEL, 0, 0)];
            } else if (LOWORD(wParam) == IDC_MODE)
            {
                U32 num;
                U8 desc[1024];
                U32 mode;

                num = SendMessageA((HWND)lParam, LB_GETCURSEL, 0, 0);
                SendMessageA((HWND)lParam, LB_GETTEXT, num, (LPARAM)desc);
                if (Init_GetMode(desc, &mode))
                    initGlobs.selMode = &initGlobs.modes[mode];
            } else if (LOWORD(wParam) == IDC_DRIVER)
            {
                initGlobs.selDriver = &initGlobs.drivers[SendMessageA((HWND)lParam, LB_GETCURSEL, 0, 0)];

                Init_HandleWindowButton(hWndDlg);
                Init_SetModeList(hWndDlg);
                Init_SetDeviceList(hWndDlg);
            }
        } else if (HIWORD(wParam) == BN_CLICKED)
        {
            if (LOWORD(wParam) == IDC_WINDOW)
                Init_SetFullScreen(hWndDlg, FALSE);
            if (LOWORD(wParam) == IDC_FULLSCREEN) // this is not an else if in the original code
                Init_SetFullScreen(hWndDlg, TRUE);
            else if (LOWORD(wParam) == IDOK)
                EndDialog(hWndDlg, wParam);
            else if (LOWORD(wParam) == IDCANCEL)
                EndDialog(hWndDlg, wParam);
        }

        return TRUE;
    }
    return FALSE;
}

B32 Init_Initialize(B32 setup, B32 debug, B32 best, B32 window, const char* noHALMsg)
{
    initGlobs.selDriver = NULL;
    initGlobs.selDevice = NULL;
    initGlobs.selMode = NULL;
#ifdef LEGORR_WINDOWED_DEFAULT
    initGlobs.selFullScreen = initGlobs.wasFullScreen = FALSE;
#else
    initGlobs.selFullScreen = initGlobs.wasFullScreen = TRUE;
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
    if (initGlobs.driverCount)
    {
        initGlobs.selDriver = initGlobs.drivers;
        if (best)
        {
            B32 found = FALSE;

            initGlobs.selFullScreen = !window;
            rval = IDOK;

            for (S32 loop = initGlobs.driverCount - 1; loop >= 0; loop--)
            {
                if (DirectDraw_EnumDevices(&initGlobs.drivers[loop], initGlobs.devices, &initGlobs.deviceCount))
                {
                    DirectDraw_EnumModes(&initGlobs.drivers[loop], initGlobs.selFullScreen, initGlobs.modes, &initGlobs.modeCount);
                    for (U32 mode = 0; mode < initGlobs.modeCount; mode++)
                    {
                        if (Init_IsValidMode(mode))
                        {
                            for (S32 sub = initGlobs.deviceCount - 1; sub >= 0; sub--)
                            {
                                if (initGlobs.devices[sub].flags & GRAPHICS_DEVICE_FLAG_HARDWARE)
                                {
                                    initGlobs.selDriver = &initGlobs.drivers[loop];
                                    initGlobs.selDevice = &initGlobs.devices[sub];
                                    initGlobs.selMode = &initGlobs.modes[mode];
                                    found = TRUE;
                                    break;
                                }
                            }
                        }
                    }
                } else {
                    MessageBoxA(NULL, "Please install DirectX version 6 or later", "Error", MB_OK);
                    return FALSE;
                }
                if (found == TRUE)
                    break;
            }

            if (!found)
            {
                MessageBoxA(NULL, noHALMsg, "Error", MB_OK);
                return FALSE;
            }
        } else {
            if (setup)
                rval = DialogBoxParamA(Main_hInst(), MAKEINTRESOURCE(LEGORR_MODESELECTION_DIALOG), Main_hWnd(), Init_DialogProc, 0);
        }

        if (rval == IDOK)
        {
            if (initGlobs.selFullScreen)
                return DirectDraw_Setup(TRUE, initGlobs.selDriver, initGlobs.selDevice, initGlobs.selMode, 0, 0, 320, 200);
            else if (initGlobs.selMode)
                return DirectDraw_Setup(FALSE, NULL, initGlobs.selDevice, NULL, 100, 100, initGlobs.selMode->width, initGlobs.selMode->height);
            else
                return DirectDraw_Setup(FALSE, NULL, initGlobs.selDevice, NULL, 40, 40, 640, 480);
        } else if (rval == -1)
        {
            Error_Warn(TRUE, "Cannot create dialog box");
        }
    } else {
        Error_Warn(TRUE, "Cannot Enumerate Drivers");
    }
    return FALSE;
}

void Init_AddValidMode(U32 width, U32 height, U32 depth)
{
    initGlobs.validModes[initGlobs.validModeCount].width = width;
    initGlobs.validModes[initGlobs.validModeCount].height = height;
    initGlobs.validModes[initGlobs.validModeCount].bitDepth = depth;
    initGlobs.validModeCount++;
}
