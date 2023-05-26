#include "fonts.h"

Font_Globs fontGlobs = { NULL };

void Font_Initialize()
{
    // TODO: Implement Font_Initialize
}

void Font_Shutdown()
{
    // TODO: Implement Font_Shutdown
}

lpFont Font_Load(const char* fname)
{
    // TODO: Implement Font_Load
    return NULL;
}

lpFont Font_Create(lpImage image)
{
    // TODO: Implement Font_Create
    return NULL;
}

void Font_AddList()
{
    // TODO: Implement Font_AddList
}

void Font_Remove(lpFont dead)
{
    // TODO: Implement Font_Remove
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
    // TODO: Implement Font_RemoveCallback
}

void Font_RunThroughLists(void (*Callback)(lpFont font, void* data), void* data)
{
    // TODO: Implement Font_RunThroughLists
}
