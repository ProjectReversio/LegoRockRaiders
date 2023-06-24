#pragma once

#include <windows.h>
#include "platform.h"
#include <d3d.h>
#include <d3drm.h>
#include "directdraw.h"
#include <ddraw.h>

#define MAIN_MAXRENDERSTATES 200

typedef B32 (*MainStateInitialize)();
typedef B32 (*MainStateMainLoop)(F32 delta);
typedef B32 (*MainStateShutdown)();

typedef void (*MainWindowCallback)(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

typedef struct Main_State
{
    MainStateInitialize Initialize;
    MainStateMainLoop MainLoop;
    MainStateShutdown Shutdown;
} Main_State, *lpMain_State;

typedef struct Main_StateChangeData
{
    U32 origValue;
    B32 changed;
} Main_StateChangeData, *lpMain_StateChangeData;

// Command line argument flags set when starting the game
typedef enum MainFlags
{
    MAIN_FLAG_NONE                   = 0,
    MAIN_FLAG_UPDATED                = 0x1,
    MAIN_FLAG_FULLSCREEN             = 0x2,
    MAIN_FLAG_VIDEOTEXTURE           = 0x4,
    MAIN_FLAG_MIPMAPENABLED          = 0x8,
    MAIN_FLAG_PAUSED                 = 0x10,
    MAIN_FLAG_DONTMANAGETEXTURES     = 0x20,
    MAIN_FLAG_BEST                   = 0x40,
    MAIN_FLAG_DUMPMODE               = 0x80,
    MAIN_FLAG_WINDOW                 = 0x100,
    MAIN_FLAG_STARTLEVEL             = 0x200,
    MAIN_FLAG_CLEANSAVES             = 0x400,
    MAIN_FLAG_SAVELANGFILE           = 0x800,
    MAIN_FLAG_LANGDUMPUNKNOWN        = 0x1000,
    MAIN_FLAG_DEBUGMODE              = 0x2000,
    MAIN_FLAG_DUALMOUSE              = 0x4000,
    MAIN_FLAG_DEBUGCOMPLETE          = 0x8000,
    MAIN_FLAG_TESTERCALL             = 0x10000,
    MAIN_FLAG_LEVELSOPEN             = 0x20000,
    MAIN_FLAG_FORCETEXTUREMANAGEMENT = 0x40000,
    MAIN_FLAG_FORCEVERTEXFOG         = 0x80000,
    MAIN_FLAG_REDUCESAMPLES          = 0x100000,
    MAIN_FLAG_SHOWVERSION            = 0x200000,
    MAIN_FLAG_REDUCEANIMATION        = 0x400000,
    MAIN_FLAG_REDUCEPROMESHES        = 0x800000,
    MAIN_FLAG_REDUCEFLICS            = 0x1000000,
    MAIN_FLAG_REDUCEIMAGES           = 0x2000000,
} MainFlags;

// Programmer mode enum
typedef enum ProgrammerMode
{
    PROGRAMMER_OFF     = 0,
    PROGRAMMER_MODE_1  = 1,
    PROGRAMMER_MODE_2  = 2,
    PROGRAMMER_MODE_3  = 3,
    PROGRAMMER_MODE_10 = 10,
    PROGRAMMER_MODE_11 = 11,
} ProgrammerMode;

typedef enum Graphics_Quality
{
    Wireframe = 0,
    UnlitFlat = 1,
    Flat      = 2,
    Gouraud   = 3,
    Phong     = 4,
} Graphics_Quality;

// Flags specified by the "-flags #" command line argument (which has to be in decimal...)
typedef enum MainCLFlags
{
    CL_FLAG_NONE      = 0,
    CL_FLAG_BLOCKFADE = 0x8000,
} MainCLFlags;

typedef struct Main_Globs
{
    HWND hWnd;
    HINSTANCE hInst;
    B32 active;
    B32 exit;
    char* className;
    char programName[256];
    LPDIRECT3DRM3 lpD3DRM;
    LPDIRECT3DRMDEVICE3 device;
    LPDIRECT3DDEVICE3 imDevice;
    ULONG fogMethod;
    U32 appWidth, appHeight;
    Main_State currState;
    B32 stateSet;
    F32 fixedFrameTiming;
    Main_StateChangeData renderStateData[200];
    U32 style;
    MainFlags flags;
    ProgrammerMode programmerLevel;
    char startLevel[128];
    char languageName[128];
    MainCLFlags clFlags;
    HACCEL accels;
    MainWindowCallback windowCallback;
} Main_Globs, *lpMain_Globs;

extern Main_Globs mainGlobs;

extern void Main_ParseCommandLine(const char* lpszCmdLine, B32* out_nosound, B32* out_insistOnCD);
extern B32 Main_InitApp(HINSTANCE hInstance);

extern __declspec(noreturn) void Main_Exit();

extern B32 Main_SetState(Main_State* state);
extern void Main_SetTitle(const char* title);

extern void Main_LoopUpdate(B32 clear);

extern U32 Main_GetWindowsBitDepth();

extern void Main_SetupDisplay(B32 fullScreen, U32 xPos, U32 yPos, U32 width, U32 height);
extern B32 Main_SetupDirect3D(lpGraphics_Device dev, LPDIRECTDRAW ddraw1, LPDIRECTDRAWSURFACE4 backSurf, B32 doubleBuffered);

extern void Main_ChangeRenderState(D3DRENDERSTATETYPE dwRenderStateType, U32 dwRenderState);
extern void Main_RestoreStates(B32 force);

extern void Main_DisableTextureManagement();

inline HWND Main_hWnd() { return mainGlobs.hWnd; }
inline HINSTANCE Main_hInst() { return mainGlobs.hInst; }
inline void Main_SetFogMethod(U32 m) { mainGlobs.fogMethod = m; }
inline ProgrammerMode Main_ProgrammerMode() { return mainGlobs.programmerLevel; }
inline LPDIRECT3DRM3 lpD3DRM() { return mainGlobs.lpD3DRM; }
inline LPDIRECT3DRMDEVICE3 lpDevice() { return mainGlobs.device; }
inline LPDIRECT3DDEVICE3 lpIMDevice() { return mainGlobs.imDevice; }
inline S32 appWidth() { return mainGlobs.appWidth; }
inline S32 appHeight() { return mainGlobs.appHeight; }
inline const char* Main_GetStartLevel() { return (mainGlobs.flags & MAIN_FLAG_STARTLEVEL) ? mainGlobs.startLevel : NULL; }
