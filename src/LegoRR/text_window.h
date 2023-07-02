#pragma once

#include "platform.h"
#include "fonts.h"

#define TEXTWINDOW_MAXSTRINGLEN     1024
#define TEXTWINDOW_MAXLINES         256

#define	TEXTWINDOW_DISPLAYDELAY     25

#define TEXTWINDOW_FLAG_WINDOWWRAPPED   0x00000001
#define TEXTWINDOW_FLAG_OVERLAY         0x00000002
#define TEXTWINDOW_FLAG_CENTERED        0x00000004

typedef struct TextWindow
{
    lpFont font;
    Area2F windowSize;
    U8* windowBuffer;
    char secondaryBuffer[TEXTWINDOW_MAXSTRINGLEN];
    U32 windowBufferSize, windowBufferEnd;
    U32 lineList[TEXTWINDOW_MAXLINES];
    U32 usedLines;
    S32 maxLines;
    F32 displayDelay;
    U32 flags;
} TextWindow, *lpTextWindow;

extern void TextWindow_Initialize();

extern lpTextWindow TextWindow_Create(lpFont font, Area2F* size, U32 bufferSize);

extern void TextWindow_EnableCentering(lpTextWindow window, BOOL enable);

extern B32 TextWindow_Update(lpTextWindow window, U32 posFromEnd, F32 time, S32* lpLowestPoint);
extern B32 TextWindow_UpdateOverlay(lpTextWindow window, F32 time, S32* lpLowestPoint);

extern void TextWindow_Clear(lpTextWindow window);
extern void TextWindow_PrintF(lpTextWindow window, const char* msg, ...);
extern void TextWindow_VPrintF(lpTextWindow window, U32 page, const char* msg, va_list args);
