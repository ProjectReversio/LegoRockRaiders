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

HRESULT WINAPI DirectDraw_EnumDeviceCallback(LPGUID lpGuid, LPSTR lpDeviceDescription,
                                             LPSTR lpDeviceName, LPD3DDEVICEDESC lpHWDesc,
                                             LPD3DDEVICEDESC lpHELDesc, LPVOID lpContext)
{
    lpGraphics_Device dev = &directDrawGlobs.deviceList[directDrawGlobs.deviceCount];
    LPD3DDEVICEDESC desc;

    dev->flags = GRAPHICS_DRIVER_FLAG_VALID;
    if (lpHWDesc->dcmColorModel != 0)
    {
        dev->flags |= GRAPHICS_DEVICE_FLAG_HARDWARE;
        desc = lpHWDesc;
    } else
    {
        desc = lpHELDesc;
    }

    if (desc->dwFlags & D3DDD_COLORMODEL)
    {
        if (desc->dcmColorModel == D3DCOLOR_RGB)
            dev->flags |= GRAPHICS_DEVICE_FLAG_COLOUR;

        if (desc->dwFlags & D3DDD_DEVICERENDERBITDEPTH)
        {
            if (desc->dwDeviceRenderBitDepth & DDBD_8)
                dev->flags |= GRAPHICS_DEVICE_FLAG_DEPTH8;
            if (desc->dwDeviceRenderBitDepth & DDBD_16)
                dev->flags |= GRAPHICS_DEVICE_FLAG_DEPTH16;
            if (desc->dwDeviceRenderBitDepth & DDBD_24)
                dev->flags |= GRAPHICS_DEVICE_FLAG_DEPTH24;
            if (desc->dwDeviceRenderBitDepth & DDBD_32)
                dev->flags |= GRAPHICS_DEVICE_FLAG_DEPTH32;
        }
    }

    if (desc->dwFlags & D3DDD_DEVCAPS)
    {
        if (desc->dwDevCaps & D3DDEVCAPS_TEXTUREVIDEOMEMORY)
            dev->flags |= GRAPHICS_DEVICE_FLAG_VIDEOTEXTURE;
        else if (desc->dwDevCaps & D3DDEVCAPS_TEXTURESYSTEMMEMORY)
            dev->flags |= GRAPHICS_DEVICE_FLAG_SYSTEMTEXTURE;
    }

    dev->guid = *lpGuid;
    sprintf(dev->desc, "%s (%s)", lpDeviceName, lpDeviceDescription);

    directDrawGlobs.deviceCount++;

    return D3DENUMRET_OK;
}

B32 DirectDraw_EnumDevices(lpGraphics_Driver driver, lpGraphics_Device ref_list, U32* out_count)
{
    LPDIRECTDRAW4 lpDD;
    LPDIRECTDRAW lpDD1;
    LPDIRECT3D3 lpD3D;
    LPGUID guid;
    B32 res = FALSE;

    directDrawGlobs.deviceCount = 0;
    if (driver->flags & GRAPHICS_DRIVER_FLAG_PRIMARY)
        guid = NULL;
    else
        guid = &driver->guid;

    if(DirectDrawCreate(&driver->guid, &lpDD1, NULL) == DD_OK)
    {
        if (lpDD1->lpVtbl->QueryInterface(lpDD1, &IID_IDirectDraw4, &lpDD) == DD_OK)
        {
            if (lpDD->lpVtbl->QueryInterface(lpDD, &IID_IDirect3D3, &lpD3D) == DD_OK)
            {
                directDrawGlobs.deviceList = ref_list;
                lpD3D->lpVtbl->EnumDevices(lpD3D, DirectDraw_EnumDeviceCallback, NULL);
                res = TRUE;

                lpD3D->lpVtbl->Release(lpD3D);
            }
            lpDD->lpVtbl->Release(lpDD);
        }
        lpDD1->lpVtbl->Release(lpDD1);
    }

    *out_count = directDrawGlobs.deviceCount;

    return res;
}
