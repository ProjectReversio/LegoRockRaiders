#pragma once

#include <stdarg.h>
#include "types.h"
#include "images.h"

#define FONT_MAXLISTS 32

#define FONT_LASTCHARACTER 202 // Last character in the font image.

#define FONT_GRIDWIDTH 10
#define FONT_GRIDHEIGHT 19

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

extern void Font_VGetStringInfo(lpFont font, U32* width, U32* lineCount, const char* msg, va_list args);

extern void Font_RemoveCallback(lpFont font, void* data);
extern void Font_RunThroughLists(void (*Callback)(lpFont font, void* data), void* data);

inline void Font_GetStringInfo(lpFont font, U32* width, U32* lineCount, const char* msg, ...) { va_list args; va_start(args, msg); Font_VGetStringInfo(font, width, lineCount, msg, args); va_end(args); }
