#include "directdraw.h"
#include <stdio.h>

DirectDraw_Globs directDrawGlobs = { NULL };

void DirectDraw_Initialize(HWND hWnd)
{
    directDrawGlobs.driverCount = 0;
    directDrawGlobs.driverList = NULL;
    directDrawGlobs.deviceCount = 0;
    directDrawGlobs.deviceList = NULL;
    directDrawGlobs.modeCount = 0;
    directDrawGlobs.modeList = NULL;
    directDrawGlobs.hWnd = hWnd;
    directDrawGlobs.lpDirectDraw = NULL;
    directDrawGlobs.zSurf = NULL;
    directDrawGlobs.bSurf = NULL;
    directDrawGlobs.fSurf = NULL;
    directDrawGlobs.lpFrontClipper = NULL;
}

void DirectDraw_Shutdown()
{
    // TODO: Implement DirectDraw_Shutdown
}

BOOL WINAPI DirectDraw_EnumDriverCallback(GUID FAR *lpGUID, LPSTR lpDriverDescription, LPSTR lpDriverName, LPVOID lpContext)
{
    directDrawGlobs.driverList[directDrawGlobs.driverCount].flags = GRAPHICS_DRIVER_FLAG_VALID;

    if (lpGUID != NULL)
        directDrawGlobs.driverList[directDrawGlobs.driverCount].guid = *lpGUID;
    else
        directDrawGlobs.driverList[directDrawGlobs.driverCount].flags |= GRAPHICS_DRIVER_FLAG_PRIMARY;

    if (lpGUID == NULL)
        directDrawGlobs.driverList[directDrawGlobs.driverCount].flags |= GRAPHICS_DRIVER_FLAG_WINDOWOK;

    sprintf(directDrawGlobs.driverList[directDrawGlobs.driverCount].desc, "%s (%s)", lpDriverDescription, lpDriverName);

    directDrawGlobs.driverCount++;

    return TRUE;
}

B32 DirectDraw_EnumDrivers(lpGraphics_Driver ref_list, U32* out_count)
{
    directDrawGlobs.driverList = ref_list;
    DirectDrawEnumerateA(DirectDraw_EnumDriverCallback, NULL);
    *out_count = directDrawGlobs.driverCount;
    return TRUE;
}
