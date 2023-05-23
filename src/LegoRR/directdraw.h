#pragma once

#include "types.h"
#include "maths.h"
#include <windows.h>
#include <ddraw.h>
#include <d3d.h>

#define DIRECTDRAW_MAXDRIVERS 20
#define DIRECTDRAW_MAXDEVICES 20
#define DIRECTDRAW_MAXMODES 200

#ifdef __cplusplus
extern "C"
{
#endif

typedef enum Graphics_DeviceFlags
{
    GRAPHICS_DEVICE_FLAG_NONE          = 0,
    GRAPHICS_DEVICE_FLAG_VALID         = 0x1,
    GRAPHICS_DEVICE_FLAG_DEPTH8        = 0x10,
    GRAPHICS_DEVICE_FLAG_DEPTH16       = 0x20,
    GRAPHICS_DEVICE_FLAG_DEPTH24       = 0x40,
    GRAPHICS_DEVICE_FLAG_DEPTH32       = 0x80,
    GRAPHICS_DEVICE_FLAG_ZBUFF8        = 0x100,
    GRAPHICS_DEVICE_FLAG_ZBUFF16       = 0x200,
    GRAPHICS_DEVICE_FLAG_ZBUFF24       = 0x400,
    GRAPHICS_DEVICE_FLAG_ZBUFF32       = 0x800,
    GRAPHICS_DEVICE_FLAG_COLOUR        = 0x1000,
    GRAPHICS_DEVICE_FLAG_HARDWARE      = 0x2000,
    GRAPHICS_DEVICE_FLAG_VIDEOTEXTURE  = 0x4000,
    GRAPHICS_DEVICE_FLAG_SYSTEMTEXTURE = 0x8000,
} Graphics_DeviceFlags;

typedef struct Graphics_Device
{
    GUID guid;
    char desc[256]; // "name (description)"
    Graphics_DeviceFlags flags;
} Graphics_Device, *lpGraphics_Device;

typedef enum Graphics_DriverFlags
{
    GRAPHICS_DRIVER_FLAG_NONE     = 0,
    GRAPHICS_DRIVER_FLAG_VALID    = 0x1,
    GRAPHICS_DRIVER_FLAG_PRIMARY  = 0x10,
    GRAPHICS_DRIVER_FLAG_WINDOWOK = 0x20,
} Graphics_DriverFlags;

typedef struct Graphics_Driver
{
    GUID guid;
    char desc[256]; // "name (description)"
    Graphics_DriverFlags flags;
} Graphics_Driver, *lpGraphics_Driver;

typedef enum Graphics_ModeFlags
{
    GRAPHICS_MODE_FLAG_NONE  = 0,
    GRAPHICS_MODE_FLAG_VALID = 0x1,
} Graphics_ModeFlags;

typedef struct Graphics_Mode
{
    U32 width;
    U32 height;
    U32 bitDepth;
    char desc[256]; // "WxH (BPP bit)", "WxH" (windowed)
    Graphics_ModeFlags flags;
} Graphics_Mode, *lpGraphics_Mode;

typedef struct DirectDraw_Globs
{
    HWND hWnd;
    LPDIRECTDRAW4 lpDirectDraw;
    LPDIRECTDRAWSURFACE4 fSurf; // "RenderTarget" surface
    LPDIRECTDRAWSURFACE4 bSurf; // "DrawTarget" surface
    LPDIRECTDRAWSURFACE4 zSurf; // (unused)
    LPDIRECTDRAWCLIPPER lpFrontClipper;
    LPDIRECTDRAWCLIPPER lpBackClipper;
    lpGraphics_Driver driverList;
    lpGraphics_Driver selectedDriver; // (unused)
    lpGraphics_Device deviceList;
    lpGraphics_Device selectedDevice; // (unused)
    Graphics_Mode* modeList; // "ScreenModes"
    U32 driverCount;
    U32 deviceCount;
    U32 modeCount; // "ScreenModes"
    B32 fullScreen;
    U32 width;
    U32 height;
} DirectDraw_Globs, *lpDirectDraw_Globs;

extern DirectDraw_Globs directDrawGlobs;

extern void DirectDraw_Initialize(HWND hWnd);
extern void DirectDraw_Shutdown();

extern B32 DirectDraw_EnumDrivers(lpGraphics_Driver ref_list, U32* out_count);
extern B32 DirectDraw_EnumDevices(lpGraphics_Driver driver, lpGraphics_Device ref_list, U32* out_count);
extern B32 DirectDraw_EnumModes(lpGraphics_Driver driver, B32 fullScreen, lpGraphics_Mode ref_list, U32* out_count);

extern B32 DirectDraw_Setup(B32 fullscreen, lpGraphics_Driver driver, lpGraphics_Device device, lpGraphics_Mode screen, U32 xPos, U32 yPos, U32 width, U32 height);
extern B32 DirectDraw_CreateClipper(B32 fullscreen, U32 width, U32 height);

extern void DirectDraw_Flip();
extern void DirectDraw_Clear(Area2F* window, U32 colour);

inline LPDIRECTDRAW4 DirectDraw() { return directDrawGlobs.lpDirectDraw; }
inline LPDIRECTDRAWSURFACE4 DirectDraw_fSurf() { return directDrawGlobs.fSurf; }
inline LPDIRECTDRAWSURFACE4 DirectDraw_bSurf() { return directDrawGlobs.bSurf; }
inline B32 DirectDraw_FullScreen() { return directDrawGlobs.fullScreen; }

#ifdef __cplusplus
}
#endif