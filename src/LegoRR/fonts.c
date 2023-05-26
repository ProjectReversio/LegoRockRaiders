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
    // TODO: Implement Font_Load
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
    // TODO: Implement Font_PrintF
    return 0;
}

void Font_VGetStringInfo(lpFont font, U32* width, U32* lineCount, const char* msg, va_list args)
{
    // TODO: Implement Font_VGetStringInfo
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
