#pragma once

#include "platform.h"
#include <dinput.h>

/*typedef enum Keys8 // : U8
{
    KEY__NONE           = 0,
	KEY_ESCAPE          = 1,
	KEY_ONE             = 2,
	KEY_TWO             = 3,
	KEY_THREE           = 4,
	KEY_FOUR            = 5,
	KEY_FIVE            = 6,
	KEY_SIX             = 7,
	KEY_SEVEN           = 8,
	KEY_EIGHT           = 9,
	KEY_NINE            = 10,
	KEY_ZERO            = 11,
	KEY_MINUS           = 12,
	KEY_EQUALS          = 13,
	KEY_BACKSPACE       = 14,
	KEY_TAB             = 15,
	KEY_Q               = 16,
	KEY_W               = 17,
	KEY_E               = 18,
	KEY_R               = 19,
	KEY_T               = 20,
	KEY_Y               = 21,
	KEY_U               = 22,
	KEY_I               = 23,
	KEY_O               = 24,
	KEY_P               = 25,
	KEY_LEFTBRACE       = 26,
	KEY_RIGHTBRACE      = 27,
	KEY_RETURN          = 28,
	KEY_LEFTCTRL        = 29,
	KEY_A               = 30,
	KEY_S               = 31,
	KEY_D               = 32,
	KEY_F               = 33,
	KEY_G               = 34,
	KEY_H               = 35,
	KEY_J               = 36,
	KEY_K               = 37,
	KEY_L               = 38,
	KEY_SEMICOLON       = 39,
	KEY_AT              = 40,
	KEY_RSINGLEQUOTE    = 41,
	KEY_LEFTSHIFT       = 42,
	KEY_HASH            = 43,
	KEY_Z               = 44,
	KEY_X               = 45,
	KEY_C               = 46,
	KEY_V               = 47,
	KEY_B               = 48,
	KEY_N               = 49,
	KEY_M               = 50,
	KEY_LEFTARROW       = 51,
	KEY_RIGHTARROW      = 52,
	KEY_QUESTIONMARK    = 53,
	KEY_RIGHTSHIFT      = 54,
	KEYPAD_ASTERISK     = 55,
	KEY_ALT             = 56,
	KEY_SPACE           = 57,
	KEY_CAPLOCK         = 58,
	KEY_F1              = 59,
	KEY_F2              = 60,
	KEY_F3              = 61,
	KEY_F4              = 62,
	KEY_F5              = 63,
	KEY_F6              = 64,
	KEY_F7              = 65,
	KEY_F8              = 66,
	KEY_F9              = 67,
	KEY_F10             = 68,
	KEYPAD_NUMLOCK      = 69,
	KEY_SCROLLLOCK      = 70,
	KEYPAD_7            = 71,
	KEYPAD_8            = 72,
	KEYPAD_9            = 73,
	KEYPAD_MINUS        = 74,
	KEYPAD_4            = 75,
	KEYPAD_5            = 76,
	KEYPAD_6            = 77,
	KEYPAD_PLUS         = 78,
	KEYPAD_1            = 79,
	KEYPAD_2            = 80,
	KEYPAD_3            = 81,
	KEYPAD_0            = 82,
	KEYPAD_DELETE       = 83,
	KEY_BACKSLASH       = 86,
	KEY_F11             = 87,
	KEY_F12             = 88,
	KEYPAD_ENTER        = 156,
	KEY_RIGHTCTRL       = 157,
	KEYPAD_FORWARDFLASH = 181,
	KEY_PRINTSCREEN     = 183,
	KEY_ALTGR           = 184,
	KEY_HOME            = 199,
	KEY_CURSORUP        = 200,
	KEY_PGUP            = 201,
	KEY_CURSORLEFT      = 203,
	KEY_END             = 207,
	KEY_CURSORDOWN      = 208,
	KEY_PGDN            = 209,
	KEY_INSERT          = 210,
	KEY_DELETE          = 211,
} Keys8;*/

typedef U8 Keys8;

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
