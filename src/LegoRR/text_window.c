#include <stdio.h>
#include "text_window.h"
#include "mem.h"

void TextWindow_Initialize()
{
    // Empty in original source
}

lpTextWindow TextWindow_Create(lpFont font, Area2F* size, U32 bufferSize)
{
    lpTextWindow textWnd = (lpTextWindow)Mem_Alloc(sizeof(TextWindow));
    if (textWnd == NULL)
        return NULL;

    memset(textWnd, 0, sizeof(TextWindow));

    textWnd->font = font;
    textWnd->windowSize = *size;

    textWnd->windowBuffer = Mem_Alloc(bufferSize);
    textWnd->windowBufferSize = bufferSize;
    TextWindow_EnableCentering(textWnd, TRUE);

    return textWnd;
}

void TextWindow_EnableCentering(lpTextWindow window, BOOL enable)
{
    if (enable)
        window->flags |= TEXTWINDOW_FLAG_CENTERED;
    else
        window->flags &= ~TEXTWINDOW_FLAG_CENTERED;
}

B32 TextWindow_Update(lpTextWindow window, U32 posFromEnd, F32 time, S32* lpLowestPoint)
{
    // TODO: Implement TextWindow_Update
    return FALSE;
}

void TextWindow_Clear(lpTextWindow window)
{
    memset(window->windowBuffer, 0, window->windowBufferSize);
    memset(window->secondaryBuffer, 0, TEXTWINDOW_MAXSTRINGLEN);
    window->usedLines = 0;
    window->windowBufferEnd = 0;
    window->flags &= ~TEXTWINDOW_FLAG_WINDOWWRAPPED;
}

void TextWindow_PrintF(lpTextWindow window, const char* msg, ...)
{
    va_list args;

    va_start(args, msg);
    TextWindow_VPrintF(window, 0, msg, args);
    va_end(args);
}

void TextWindow_VPrintF(lpTextWindow window, U32 page, const char* msg, va_list args)
{
    U8 line[TEXTWINDOW_MAXSTRINGLEN];
    U32 count, loop, onPage = 0;

    count = vsprintf(line, msg, args);

    for (loop = 0; loop < count; loop++)
    {
        if (onPage == page)
            break;

        if ('\a' == line[loop])
            onPage++;
    }

    if (loop == count)
        return; // Couldn't find the page...

    for (; loop < count; loop++)
    {
        window->windowBuffer[window->windowBufferEnd++] = line[loop];
        if (window->windowBufferEnd == window->windowBufferSize)
        {
            window->windowBufferEnd = 0;
            window->flags |= TEXTWINDOW_FLAG_WINDOWWRAPPED;
        }
    }
}
