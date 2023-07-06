#pragma once

#include "platform.h"
#include <dinput.h>

typedef struct DIcallbackData
{
    IDirectInputA* di;
    HWND hWnd;
    void* joystick;
} DIcallbackData, *PDIcallbackData;

typedef struct Input_Globs
{
    B8 prevKey_Map[256];
    LPDIRECTINPUT lpdi;
    LPDIRECTINPUTDEVICE lpdiKeyboard;
    LPDIRECTINPUTDEVICE lpdiMouse;
    B32 fKeybdAcquired, fMouseAcquired;
    RECT MouseBounds;
    S32 msx, msy, prev_msx, prev_msy;
    S32 diffx, diffy;
    B32 mslb, msrb;
    B32 mslblast, mslbheld;
    B32 lClicked, rClicked;
    B32 lDoubleClicked, rDoubleClicked;
    B32 caps;
    DIMOUSESTATE dims;
    DIcallbackData cbd;
    char Input_KeyTemp;
    U8 padding1[3];
    B8 Key_Map[256];

} Input_Globs, *lpInput_Globs;

extern Input_Globs inputGlobs;

extern B32 Input_InitKeysAndDI();
extern void Input_ReleaseKeysAndDI();

extern void Input_ReadKeys();
extern void Input_ReadMouse2();

extern U32 Input_AnyKeyPressed();

#define Input_IsKeyDown(k) ((B32)inputGlobs.Key_Map[(k)])
#define Input_IsKeyUp(k) (!inputGlobs.Key_Map[(k)])
#define Input_IsPrevKeyDown(k) ((B32)inputGlobs.prevKey_Map[(k)])
#define Input_IsPrevKeyUp(k) (!inputGlobs.prevKey_Map[(k)])
#define Input_IsKeyPressed(k) (Input_IsKeyDown((k)) && Input_IsPrevKeyUp((k)))
#define Input_IsKeyReleased(k) (Input_IsKeyUp((k)) && Input_IsPrevKeyDown((k)))
#define Input_IsKeyChanged(k) (Input_IsKeyDown((k)) != Input_IsPrevKeyDown((k)))
#define Input_IsKeyHeld(k) (Input_IsKeyDown((k)) && Input_IsPrevKeyDown((k)))
#define Input_IsKeyUntouched(k) (Input_IsKeyUp((k)) && Input_IsPrevKeyUp((k)))

#define Input_ClearKey(k) (inputGlobs.Key_Map[(k)] = FALSE)
#define Input_ClearAllKeys() memset(inputGlobs.Key_Map, 0, sizeof(inputGlobs.Key_Map))
