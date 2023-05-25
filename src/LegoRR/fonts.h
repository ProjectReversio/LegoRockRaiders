#pragma once

#include <stdarg.h>
#include "types.h"

#define FONT_LASTCHARACTER 202 // Last character in the font image.

typedef struct Font
{
    void* temp;
    // TODO: Implement Font
} Font, *lpFont;

extern U32 Font_PrintF(lpFont font, S32 x, S32 y, const char* msg, ...);

extern void Font_VGetStringInfo(lpFont font, U32* width, U32* lineCount, const char* msg, va_list args);

inline void Font_GetStringInfo(lpFont font, U32* width, U32* lineCount, const char* msg, ...) { va_list args; va_start(args, msg); Font_VGetStringInfo(font, width, lineCount, msg, args); va_end(args); }
