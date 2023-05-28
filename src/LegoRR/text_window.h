#pragma once

#include "types.h"
#include "fonts.h"

typedef struct TextWindow
{
    lpFont font;
    // TODO: Implement TextWindow
} TextWindow, *lpTextWindow;

extern lpTextWindow TextWindow_Create(lpFont font, Area2F* area, U32 size);

extern void TextWindow_PrintF(lpTextWindow textWindow, const char* format, ...);
