#include "init.h"
#include "main.h"
#include "resource/resource.h"

Init_Globs initGlobs = { NULL };

BOOL CALLBACK Init_DialogProc(HWND hWndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    // TODO: Implement Init_DialogProc
    return FALSE;
}

B32 Init_Initialize(B32 setup, B32 debug, B32 best, B32 window, const char* noHALMsg)
{
    initGlobs.selDriver = NULL;
    initGlobs.selDevice = NULL;
    initGlobs.selMode = NULL;
    initGlobs.wasFullScreen = TRUE;
    initGlobs.selFullScreen = TRUE;
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
