#pragma once

#include <stdarg.h>
#include "platform.h"
#include "images.h"
#include "error.h"

#define FONT_MAXLISTS 32

#define FONT_LASTCHARACTER 202 // Last character in the font image.

#define FONT_GRIDWIDTH 10
#define FONT_GRIDHEIGHT 19
#define FONT_MAXSTRINGLEN 1024

#ifdef _DEBUG
// TODO: in the original code, they never called Font_Initialize,
//  so we need to do the same, but this would cause this error to come up.
//  so for now we'll just disable it
#define Font_CheckInit()
//#define Font_CheckInit() if (!(fontGlobs.flags & FONT_FLAG_INITIALIZED)) { Error_Fatal(TRUE, "Error: Font_Initialize() Has Not Been Called"); }
#else
#define Font_CheckInit()
#endif

typedef enum FontFlags
{
    FONT_FLAG_NONE = 0,
    FONT_FLAG_INITIALIZED = 0x1,
    FONT_FLAG_WINDOWSET = 0x2,
    FONT_FLAG_WINDOWWRAPPED = 0x4,
} FontFlags;

typedef struct Font
{
    lpImage image;
    Area2F posSet[FONT_GRIDWIDTH][FONT_GRIDHEIGHT];
    U32 fontHeight, tabWidth;

    U32 flags;

    struct Font* nextFree;

} Font, *lpFont;

typedef struct Font_Globs
{
    Font* listSet[FONT_MAXLISTS];
    Font* freeList;

    U32 listCount;
    U32 flags;
} Font_Globs, *lpFont_Globs;

extern Font_Globs fontGlobs;

extern void Font_Initialize();
extern void Font_Shutdown();

extern lpFont Font_Load(const char* fname);

extern lpFont Font_Create(lpImage image);
extern void Font_AddList();
extern void Font_Remove(lpFont dead);

extern U32 Font_PrintF(lpFont font, S32 x, S32 y, const char* msg, ...);
extern U32 Font_VPrintF(lpFont font, S32 x, S32 y, const char* msg, va_list args);
extern U32 Font_VPrintF2(lpFont font, S32 x, S32 y, B32 render, U32* lineCount, const char* msg, va_list args);

extern U32 Font_OutputChar(lpFont font, S32 x, S32 y, U8 c, B32 render);

extern void Font_VGetStringInfo(lpFont font, U32* width, U32* lineCount, const char* msg, va_list args);

extern void Font_RemoveCallback(lpFont font, void* data);
extern void Font_RunThroughLists(void (*Callback)(lpFont font, void* data), void* data);

extern U32 Font_GetHeight(lpFont font);
extern U32 Font_GetCharWidth(lpFont font, U8 c);

inline U32 Font_GetStringWidth(lpFont font, const char* msg, ...) { U32 width; va_list args; va_start(args, msg); Font_VGetStringInfo(font, &width, NULL, msg, args); va_end(args); return width; }
inline U32 Font_GetLineCount(lpFont font, const char* msg, ...) { U32 lineCount; va_list args; va_start(args, msg); Font_VGetStringInfo(font, NULL, &lineCount, msg, args); va_end(args); return lineCount; }
inline void Font_GetStringInfo(lpFont font, U32* width, U32* lineCount, const char* msg, ...) { va_list args; va_start(args, msg); Font_VGetStringInfo(font, width, lineCount, msg, args); va_end(args); }
