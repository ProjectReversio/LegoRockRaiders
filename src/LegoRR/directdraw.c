#include "directdraw.h"
#include "main.h"
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
    if (directDrawGlobs.fSurf)
        directDrawGlobs.fSurf->lpVtbl->Release(directDrawGlobs.fSurf);
    if (directDrawGlobs.lpFrontClipper)
        directDrawGlobs.lpFrontClipper->lpVtbl->Release(directDrawGlobs.lpFrontClipper);
    if (directDrawGlobs.lpBackClipper)
        directDrawGlobs.lpBackClipper->lpVtbl->Release(directDrawGlobs.lpBackClipper);
    if (directDrawGlobs.lpDirectDraw)
    {
        if (directDrawGlobs.fullScreen)
            directDrawGlobs.lpDirectDraw->lpVtbl->RestoreDisplayMode(directDrawGlobs.lpDirectDraw);
        if (directDrawGlobs.lpDirectDraw)
            directDrawGlobs.lpDirectDraw->lpVtbl->Release(directDrawGlobs.lpDirectDraw);
    }
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

HRESULT WINAPI DirectDraw_EnumModeCallback(LPDDSURFACEDESC2 lpDDSurfaceDesc, LPVOID lpContext)
{
    lpGraphics_Mode mode = &directDrawGlobs.modeList[directDrawGlobs.modeCount];
    B32* fullScreen = lpContext;

    mode->flags = GRAPHICS_MODE_FLAG_VALID;
    mode->width = lpDDSurfaceDesc->dwWidth;
    mode->height = lpDDSurfaceDesc->dwHeight;
    mode->bitDepth = lpDDSurfaceDesc->ddpfPixelFormat.dwRGBBitCount;

    if ((*fullScreen))
        sprintf(mode->desc, "%ix%i (%i bit)", lpDDSurfaceDesc->dwWidth, lpDDSurfaceDesc->dwHeight, lpDDSurfaceDesc->ddpfPixelFormat.dwRGBBitCount);
    else
        sprintf(mode->desc, "%ix%i", lpDDSurfaceDesc->dwWidth, lpDDSurfaceDesc->dwHeight);

    if (!(*fullScreen) && directDrawGlobs.modeCount)
    {
        if (mode->bitDepth == Main_GetWindowsBitDepth())
            directDrawGlobs.modeCount++;
        else
            mode->flags &= ~GRAPHICS_MODE_FLAG_VALID;
    } else
    {
        directDrawGlobs.modeCount++;
    }

    return DDENUMRET_OK;
}

B32 DirectDraw_EnumModes(lpGraphics_Driver driver, B32 fullScreen, lpGraphics_Mode ref_list, U32* out_count)
{
    LPDIRECTDRAW4 lpDD;
    LPDIRECTDRAW lpDD1;
    LPGUID guid;
    B32 res = FALSE;

    directDrawGlobs.modeCount = 0;

    if (driver)
    {
        if (driver->flags & GRAPHICS_DRIVER_FLAG_VALID)
        {
            if (driver->flags & GRAPHICS_DRIVER_FLAG_PRIMARY)
                guid = NULL;
            else
                guid = &driver->guid;

            if (DirectDrawCreate(guid, &lpDD1, NULL) == DD_OK)
            {
                if (lpDD1->lpVtbl->QueryInterface(lpDD1, &IID_IDirectDraw4, &lpDD) == DD_OK)
                {
                    directDrawGlobs.modeList = ref_list;
                    lpDD->lpVtbl->EnumDisplayModes(lpDD, 0, NULL, &fullScreen, DirectDraw_EnumModeCallback);
                    res = TRUE;

                    lpDD->lpVtbl->Release(lpDD);
                }
                lpDD1->lpVtbl->Release(lpDD1);
            }
        }
    }

    *out_count = directDrawGlobs.modeCount;

    return res;
}

B32 DirectDraw_Setup(B32 fullscreen, lpGraphics_Driver driver, lpGraphics_Device device, lpGraphics_Mode screen, U32 xPos, U32 yPos, U32 width, U32 height)
{
    // TODO: Implement DirectDraw_Setup

    return FALSE;
}
