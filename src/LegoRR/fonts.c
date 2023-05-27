#include <stdio.h>
#include "fonts.h"
#include "mem.h"

Font_Globs fontGlobs = { NULL };

void Font_Initialize()
{
    U32 loop;

    for (loop = 0; loop < FONT_MAXLISTS; loop++)
        fontGlobs.listSet[loop] = NULL;

    fontGlobs.freeList = NULL;
    fontGlobs.listCount = 0;
    fontGlobs.flags = FONT_FLAG_INITIALIZED;
}

void Font_Shutdown()
{
    U32 loop;

    Font_RunThroughLists(Font_RemoveCallback, NULL);

    for (loop = 0; loop < FONT_MAXLISTS; loop++)
    {
        if (fontGlobs.listSet[loop])
            Mem_Free(fontGlobs.listSet[loop]);
    }

    fontGlobs.freeList = NULL;
    fontGlobs.flags = FONT_FLAG_NONE;
}

lpFont Font_Load(const char* fname)
{
    lpImage image;
    lpFont font;
    U32 x, y, width, height, pitch, bpp;
    Area2F* pos;
    U8* buffer;
    U32 pen255, mask, xBack, dw, loc;

    if ((image = Image_LoadBMP(fname)))
    {
        if ((font = Font_Create(image)))
        {
            Image_SetPenZeroTrans(image);
            width = Image_GetWidth(image) / FONT_GRIDWIDTH;
            height = Image_GetHeight(image) / FONT_GRIDHEIGHT;

            pen255 = Image_GetPen255(image);
            mask = Image_GetPixelMask(image);

            if ((buffer = Image_LockSurface(image, &pitch, &bpp)))
            {
                for (y = 0; y < FONT_GRIDHEIGHT; y++)
                {
                    for (x = 0; x < FONT_GRIDWIDTH; x++)
                    {
                        pos = &font->posSet[x][y];
                        pos->x = (F32) (width * x);
                        pos->y = (F32) (height * y);
                        pos->width = (F32) width;
                        pos->height = (F32) height;

                        // Pull back the width while the pixel on the end is pen255...

                        for (xBack = width - 1; xBack; xBack--)
                        {
                            loc = pitch * (U32) pos->y; // Get the start of the line...
                            loc += (U32) (pos->x + xBack) * (bpp / 8); // Get the end of the current character

                            dw = buffer[loc] << 24;
                            dw |= buffer[loc + 1] << 16;
                            dw |= buffer[loc + 2] << 8;
                            dw |= buffer[loc + 3];

                            if ((dw & mask) == pen255)
                                pos->width--;
                            else
                                break;
                        }
                    }
                }

                font->fontHeight = (U32) font->posSet[0][0].height;
                font->tabWidth = (U32) font->posSet[0][0].width * 8;

                // Clean up and return...
                Image_UnlockSurface(image);
                return font;
            }

            Font_Remove(font);
        }

        Image_Remove(image);
    }

    return NULL;
}

lpFont Font_Create(lpImage image)
{
    lpFont newFont;

    Font_CheckInit();

    if (fontGlobs.freeList == NULL)
        Font_AddList();

    newFont = fontGlobs.freeList;
    fontGlobs.freeList = newFont->nextFree;
    memset(newFont, 0, sizeof(Font));
    newFont->nextFree = newFont;

    newFont->image = image;

    return newFont;
}

void Font_AddList()
{
    lpFont list;
    U32 loop, count;

    Font_CheckInit();

    Error_Fatal(fontGlobs.listCount + 1 >= FONT_MAXLISTS, "Run out of lists");

    count = 0x00000001 << fontGlobs.listCount;

    list = fontGlobs.listSet[fontGlobs.listCount] = Mem_Alloc(sizeof(Font) * count);
    if (list)
    {
        fontGlobs.listCount++;

        for (loop = 1; loop < count; loop++)
        {
            list[loop - 1].nextFree = &list[loop];
        }

        list[count - 1].nextFree = fontGlobs.freeList;
        fontGlobs.freeList = list;
    } else {
        Error_Fatal(TRUE, Error_Format("Unable to allocate %d bytes of memory for new list.\n", sizeof(Font) * count));
    }
}

void Font_Remove(lpFont dead)
{
    Font_CheckInit();
    Error_Fatal(!dead, "NULL passed to Font_Remove()");

    Image_Remove(dead->image);

    dead->nextFree = fontGlobs.freeList;
    fontGlobs.freeList = dead;
}

U32 Font_PrintF(lpFont font, S32 x, S32 y, const char* msg, ...)
{
    va_list args;
    U32 width;

    va_start(args, msg);
    width = Font_VPrintF2(font, x, y, TRUE, NULL, msg, args);
    va_end(args);

    return width;
}

U32 Font_VPrintF(lpFont font, S32 x, S32 y, const char* msg, va_list args)
{
    return Font_VPrintF2(font, x, y, TRUE, NULL, msg, args);
}

U32 Font_VPrintF2(lpFont font, S32 x, S32 y, B32 render, U32* lineCount, const char* msg, va_list args)
{
    U8 line[FONT_MAXSTRINGLEN], line2[FONT_MAXSTRINGLEN];
    U32 width, loop, lines = 1;
    U32 xPos = 0, xMax = 0, yIncrease = font->fontHeight;
    lpImage image;
    U8* s, *t;

    // '%b' in the string refers to an image...
    // Change '%b' to the sequence @[0x<address>]...
    for (t = line2, s = msg; *s != '\0'; s++, t++)
    {
        Error_Fatal(*s == '@' && *(s + 1) == '[', "Invalid character sequence in string");
        if (*s == '%' && *(s + 1) == 'b')
        {
            *t++ = '@'; *t++ = '['; *t++ = '0'; *t++ = 'x'; *t++ = '%';
            *t++ = '0'; *t++ = '.'; *t++ = '8'; *t++ = 'x'; *t++ = ']';
            s++;
        } else {
            *t = *s;
        }
    }
    *t = '\0';

    width = vsprintf(line, line2, args);

    for (loop = 0; loop < width; loop++)
    {
        if (line[loop] == '\n')
        {
            if (xPos > xMax)
                xMax = xPos;

            xPos = 0;
            y += yIncrease;
            yIncrease = font->fontHeight;
            lines++;
        } else if (line[loop] == '\t')
        {
            xPos += font->tabWidth - (xPos % font->tabWidth);
        } else if (loop < width - 12 &&
                   line[loop] == '@' &&
                   line[loop + 1] == '[' &&
                   line[loop + 2] == '0' &&
                   line[loop + 3] == 'x' &&
                   line[loop + 12] == ']')
        {
            U32 addr = 0, sub;
            Point2F pos;
            for (sub = 0; sub < 8; sub++)
                addr |= (line[loop + 4 + sub] - (isdigit(line[loop + 4 + sub]) ? '0' : ('a' - 10))) << (28 - (sub * 4));

            if ((image = (lpImage) addr))
            {
                pos.x = (F32) (x + xPos);
                pos.y = (F32) y;

                if (line[loop] != 203)
                    xPos += image->width;

                if (image->height > yIncrease)
                    yIncrease = image->height;

                Image_Display(image, &pos);
            }
            loop += 12;
        } else {
            U32 fontWidth = Font_OutputChar(font, x + xPos, y, line[loop], render);

            if (line[loop] != 203)
                xPos += fontWidth;
        }
    }

    if (lineCount)
        *lineCount = lines;
    return xPos > xMax ? xPos : xMax;
}

U32 Font_OutputChar(lpFont font, S32 x, S32 y, U8 c, B32 render)
{
    U32 gx, gy;
    Point2F pos = { (F32) x, (F32) y };

    c -= 32;
    gy = c / FONT_GRIDWIDTH;
    gx = c % FONT_GRIDWIDTH;

    if (gy < FONT_GRIDHEIGHT)
    {
        if (render)
            Image_DisplayScaled(font->image, &font->posSet[gx][gy], &pos, NULL);
        return (U32) font->posSet[gx][gy].width;
    }

    return 0;
}

void Font_VGetStringInfo(lpFont font, U32* width, U32* lineCount, const char* msg, va_list args)
{
    U32 w = Font_VPrintF2(font, 0, 0, FALSE, lineCount, msg, args);
    if (width)
        *width = w;
}

void Font_RemoveCallback(lpFont font, void* data)
{
    Font_Remove(font);
}

void Font_RunThroughLists(void (*Callback)(lpFont font, void* data), void* data)
{
    U32 list, count, loop;
    lpFont tempFont;

    for (list = 0; list < fontGlobs.listCount; list++)
    {
        if (fontGlobs.listSet[list])
        {
            count = 0x00000001 << list;
            for (loop = 0; loop < count; loop++)
            {
                if ((tempFont = &fontGlobs.listSet[list][loop]))
                {
                    if (tempFont->nextFree == tempFont)
                    {
                        // This is a valid un-removed font....
                        Callback(tempFont, data);
                    }
                }
            }
        }
    }
}
