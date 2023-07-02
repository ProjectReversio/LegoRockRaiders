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
    U32 loop, firstLine = 0, sub;
    S32 currWidth = 0, currHeight;
    S32 over, maxOver;
    U8 c;
    U32 charWidth, wordWidth = 0;
    U32 lineWidthList[TEXTWINDOW_MAXLINES];
    B32 caretFlag = FALSE;

    if (lpLowestPoint)
        *lpLowestPoint = 0;

    // This comment is in the original source:
    // Slightly rarer crash bug? - to do with buffer wrap and going over buffer end?
    // This may be fixed, it was caused if the text size was EXACTLY 1024 bytes
    // this was messing up because it would slip past one of the == checks below.

    window->maxLines = (U32) window->windowSize.height / window->font->fontHeight;

    if (!(window->flags & TEXTWINDOW_FLAG_OVERLAY))
    {
        maxOver = window->windowBufferSize;

        if (window->flags & TEXTWINDOW_FLAG_WINDOWWRAPPED)
        {
            // Find the first line after the buffer end...
            for (loop = 0; loop < window->usedLines; loop++)
            {
                over = window->lineList[loop] - window->windowBufferEnd;
                if (over > 0 && over < maxOver)
                {
                    firstLine = window->lineList[loop];
                    maxOver = over;
                }
            }
        }

        // Rebuild the line list from the new start point...
        window->usedLines = 0;
        window->lineList[window->usedLines++] = loop = firstLine;

        while (loop != window->windowBufferEnd)
        {
            c = window->windowBuffer[loop];

            if (c == '\a') // use \a as a page end.
                break;
            else if (c == '\n')
            {
                window->lineList[window->usedLines] = loop + 1;
                lineWidthList[window->usedLines - 1] = currWidth;
                window->usedLines++;
                currWidth = 0;
                wordWidth = 0;
            } else {
                if (isspace(c))
                {
                    window->lineList[window->usedLines] = loop + 1;
                    lineWidthList[window->usedLines - 1] = currWidth;
                    wordWidth = 0;
                }

                charWidth = Font_GetCharWidth(window->font, c);
                if (c == 203)
                    charWidth = 0;

                currWidth += charWidth;
                wordWidth += charWidth;

                if (currWidth >= window->windowSize.width)
                {
                    // Check to see if the word is longer than the line
                    if (wordWidth >= window->windowSize.width)
                    {
                        // If so, split the word onto the next line
                        window->lineList[window->usedLines] = loop;
                        lineWidthList[window->usedLines - 1] = currWidth - charWidth;
                        window->usedLines++;
                        currWidth = charWidth;
                        wordWidth = charWidth;
                    } else {
                        currWidth = wordWidth;
                        window->usedLines++;
                    }
                }
            }

            if (++loop >= window->windowBufferSize)
                loop = 0;
        }

        window->lineList[window->usedLines] = loop;
        lineWidthList[window->usedLines - 1] = currWidth;

        // Display the required lines...
        if ((window->usedLines - window->maxLines + 1) > 0)
            posFromEnd = posFromEnd % (window->usedLines - window->maxLines + 1);
        else
            posFromEnd = 0;

        for (loop = 0; loop < window->usedLines; loop++)
        {
            currWidth = (S32) window->windowSize.x;

            if (window->flags & TEXTWINDOW_FLAG_CENTERED)
                currWidth += (S32) ((window->windowSize.width - lineWidthList[loop]) / 2);

            if (window->usedLines > window->maxLines &&
                loop >= window->usedLines - window->maxLines - posFromEnd &&
                loop < window->usedLines - posFromEnd)
            {
                currHeight = (S32) window->windowSize.y + ((loop - (window->usedLines - window->maxLines - posFromEnd)) * window->font->fontHeight);
            }
            else if (window->usedLines <= window->maxLines)
                currHeight = (S32)window->windowSize.y + (loop * window->font->fontHeight);
            else
                currHeight = 0;

            if (lpLowestPoint)
            {
                if (currHeight > *lpLowestPoint)
                    *lpLowestPoint = currHeight;
            }

            if (currHeight)
            {
                sub = window->lineList[loop];
                while (sub != window->lineList[loop + 1])
                {
                    c = window->windowBuffer[sub];

                    // errrrkkk! crowbar some code in here so we can ignore the caret till the next char
                    if (c == 203)
                    {
                        // if we're at the caret position, flag it and carry on
                        caretFlag = TRUE;
                    } else {
                        // if caretFlag is TRUE then the char before was the caret, safe to draw it now
                        if (caretFlag == TRUE)
                        {
                            int temp;
                            // reset the caretFlag
                            temp = Font_OutputChar(window->font, currWidth, currHeight, c, TRUE);
                            Font_OutputChar(window->font, currWidth - 4, currHeight, 203, TRUE);
                            currWidth += temp;
                            caretFlag = FALSE;
                        } else {
                            // output our normal char
                            currWidth += Font_OutputChar(window->font, currWidth, currHeight, c, TRUE);
                        }
                    }

                    if (++sub == (window->windowBufferSize + 1))
                        sub = 0;

                    if ((sub == window->lineList[loop + 1]) && (caretFlag == TRUE))
                    {
                        Font_OutputChar(window->font, currWidth - 4, currHeight, 203, TRUE);
                        caretFlag = FALSE;
                    }

                }

            }
        }

        if (lpLowestPoint)
            *lpLowestPoint += window->font->fontHeight;

        // A non overlay text window cannot over run, it just wraps. So return false.
        return FALSE;
    } else {
        return TextWindow_UpdateOverlay(window, time, lpLowestPoint);
    }
}

B32 TextWindow_UpdateOverlay(lpTextWindow window, F32 time, S32* lpLowestPoint)
{
    // TODO: Implement TextWindow_UpdateOverlay
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
