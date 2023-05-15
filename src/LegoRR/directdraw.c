#include "directdraw.h"

#include <stdio.h>
#include "main.h"
#include "mem.h"
#include "error.h"

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
    // Enumerate each driver and record its GUID and description
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
            } else {
                Error_Warn(TRUE, "Unable to obtain Direct3D3");
            }
            lpDD->lpVtbl->Release(lpDD);
        } else {
            Error_Warn(TRUE, "Unable to obtain DirectDraw4");
        }
        lpDD1->lpVtbl->Release(lpDD1);
    } else {
        Error_Warn(TRUE, "Unable to create DirectDraw");
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
                } else {
                    Error_Warn(TRUE, "Unable to obtain DirectDraw4");
                }
                lpDD1->lpVtbl->Release(lpDD1);
            } else {
                Error_Warn(TRUE, "Unable to create DirectDraw");
            }
        } else {
            Error_Fatal(TRUE, "Invalid driver passed to DirectDraw_EnumModes()");
        }
    } else {
        Error_Fatal(TRUE, "NULL passed as driver to DirectDraw_EnumModes()");
    }

    *out_count = directDrawGlobs.modeCount;

    return res;
}

B32 DirectDraw_Setup(B32 fullscreen, lpGraphics_Driver driver, lpGraphics_Device device, lpGraphics_Mode mode, U32 xPos, U32 yPos, U32 width, U32 height)
{
    HRESULT r;

    if (driver && !(driver->flags & GRAPHICS_DRIVER_FLAG_VALID))
        driver = NULL;
    if (device && !(device->flags & GRAPHICS_DEVICE_FLAG_VALID))
        device = NULL;
    if (mode && !(mode->flags & GRAPHICS_MODE_FLAG_VALID))
        mode = NULL;

    U32 bpp = 16;
    if (mode)
    {
        width = mode->width;
        height = mode->height;
        bpp = mode->bitDepth;
    }

    directDrawGlobs.width = width;
    directDrawGlobs.height = height;
    directDrawGlobs.fullScreen = fullscreen;

    LPGUID guid;
    if (driver == NULL)
        guid = NULL;
    else if (driver->flags & GRAPHICS_DRIVER_FLAG_PRIMARY)
        guid = NULL;
    else
        guid = &driver->guid;

    Main_SetupDisplay(fullscreen, xPos, yPos, width, height);

    LPDIRECTDRAW ddraw1;
    if (DirectDrawCreate(guid, &ddraw1, NULL) == DD_OK)
    {
        if (ddraw1->lpVtbl->QueryInterface(ddraw1, &IID_IDirectDraw4, &directDrawGlobs.lpDirectDraw) == DD_OK)
        {
            if (directDrawGlobs.lpDirectDraw->lpVtbl->SetCooperativeLevel(directDrawGlobs.lpDirectDraw, directDrawGlobs.hWnd, fullscreen ? (DDSCL_EXCLUSIVE | DDSCL_FULLSCREEN) : DDSCL_NORMAL) == DD_OK)
            {
                if (fullscreen)
                    r = directDrawGlobs.lpDirectDraw->lpVtbl->SetDisplayMode(directDrawGlobs.lpDirectDraw, width, height, bpp, 0, 0);
                else
                    r = DD_OK;

                if (r == DD_OK)
                {
                    DDSURFACEDESC2 desc;
                    memset(&desc, 0, sizeof(desc));
                    desc.dwSize = sizeof(desc);
                    desc.dwFlags = DDSD_CAPS;
                    desc.ddsCaps.dwCaps = DDSCAPS_3DDEVICE | DDSCAPS_PRIMARYSURFACE;
                    if (fullscreen)
                    {
                        desc.dwFlags |= DDSD_BACKBUFFERCOUNT;
                        desc.dwBackBufferCount = 1;
                        desc.ddsCaps.dwCaps |= DDSCAPS_FLIP | DDSCAPS_COMPLEX;
                    }

                    if (directDrawGlobs.lpDirectDraw->lpVtbl->CreateSurface(directDrawGlobs.lpDirectDraw, &desc, &directDrawGlobs.fSurf, 0) == DD_OK)
                    {
                        if (!fullscreen)
                        {
                            // Create the back buffer
                            desc.ddsCaps.dwCaps &= ~DDSCAPS_PRIMARYSURFACE;
                            desc.ddsCaps.dwCaps |= DDSCAPS_OFFSCREENPLAIN;
                            desc.dwFlags |= DDSD_HEIGHT | DDSD_WIDTH;
                            desc.dwWidth = width;
                            desc.dwHeight = height;
                            r = directDrawGlobs.lpDirectDraw->lpVtbl->CreateSurface(directDrawGlobs.lpDirectDraw, &desc, &directDrawGlobs.bSurf, NULL);
                        } else {
                            DDSCAPS2 ddscaps;
                            memset(&ddscaps, 0, sizeof(ddscaps));
                            ddscaps.dwCaps = DDSCAPS_BACKBUFFER;
                            r = directDrawGlobs.fSurf->lpVtbl->GetAttachedSurface(directDrawGlobs.fSurf, &ddscaps, &directDrawGlobs.bSurf);
                        }

                        if (r == DD_OK)
                        {
                            if (DirectDraw_CreateClipper(fullscreen, width, height))
                            {
                                if (Main_SetupDirect3D(device, ddraw1, directDrawGlobs.bSurf, fullscreen))
                                {
                                    // Everything went OK, so tidy up and return
                                    ddraw1->lpVtbl->Release(ddraw1);

                                    if (fullscreen)
                                        ShowCursor(FALSE);

                                    return TRUE;
                                }
                            }
                            directDrawGlobs.bSurf->lpVtbl->Release(directDrawGlobs.bSurf);
                            directDrawGlobs.bSurf = NULL;
                        } else {
                            Error_Warn(TRUE, "Error creating back surface");
                        }

                        directDrawGlobs.fSurf->lpVtbl->Release(directDrawGlobs.fSurf);
                        directDrawGlobs.fSurf = NULL;
                    } else {
                        Error_Warn(TRUE, "Error creating front surface");
                    }
                } else {
                    Error_Warn(TRUE, "Cannot set Display Mode");
                }
            } else {
                Error_Warn(TRUE, "Cannot set Cooperative Level");
            }
            directDrawGlobs.lpDirectDraw->lpVtbl->Release(directDrawGlobs.lpDirectDraw);
            directDrawGlobs.lpDirectDraw = NULL;
        } else {
            Error_Warn(TRUE, "Cannot obtain DirectDraw4");
        }
        ddraw1->lpVtbl->Release(ddraw1);
    } else {
        Error_Warn(TRUE, "Cannot create DirectDraw");
    }

    return FALSE;
}

B32 DirectDraw_CreateClipper(B32 fullscreen, U32 width, U32 height)
{
    HRGN handle;
    U32 size;
    RGNDATA *region;

    if (directDrawGlobs.lpDirectDraw->lpVtbl->CreateClipper(directDrawGlobs.lpDirectDraw, 0, &directDrawGlobs.lpBackClipper, NULL) == DD_OK)
    {
        handle = CreateRectRgn(0, 0, width, height);
        size = GetRegionData(handle, 0, NULL);
        region = Mem_Alloc(size);
        GetRegionData(handle, size, region);

        if (directDrawGlobs.lpBackClipper->lpVtbl->SetClipList(directDrawGlobs.lpBackClipper, region, 0) == DD_OK)
        {
            Mem_Free(region);

            if (directDrawGlobs.bSurf->lpVtbl->SetClipper(directDrawGlobs.bSurf, directDrawGlobs.lpBackClipper) == DD_OK)
            {
                if (!fullscreen)
                {
                    // Create the window clipper
                    if (directDrawGlobs.lpDirectDraw->lpVtbl->CreateClipper(directDrawGlobs.lpDirectDraw, 0, &directDrawGlobs.lpFrontClipper, NULL) == DD_OK)
                    {
                        // Associate the clipper with the window (obtain window sizes).
                        if (directDrawGlobs.lpFrontClipper->lpVtbl->SetHWnd(directDrawGlobs.lpFrontClipper, 0, directDrawGlobs.hWnd) == DD_OK)
                        {
                            if (directDrawGlobs.fSurf->lpVtbl->SetClipper(directDrawGlobs.fSurf, directDrawGlobs.lpFrontClipper) == DD_OK)
                            {
                                return TRUE;
                            } else
                            {
                                Error_Warn(TRUE, "Cannot attach clipper to front buffer");
                            }
                        } else {
                            Error_Warn(TRUE, "Cannot initialize clipper from hWnd");
                        }

                    } else {
                        Error_Warn(TRUE, "Cannot create front clipper");
                    }
                } else {
                    return TRUE;
                }
            } else {
                Error_Warn(TRUE, "Cannot attach clipper to back buffer");
            }
            directDrawGlobs.lpBackClipper->lpVtbl->Release(directDrawGlobs.lpBackClipper);
            directDrawGlobs.lpBackClipper = NULL;
        } else {
            Error_Warn(TRUE, "Cannot set clip list");
        }
        Mem_Free(region);
    }

    return FALSE;
}
