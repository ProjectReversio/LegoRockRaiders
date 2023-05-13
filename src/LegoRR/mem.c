#include "mem.h"

Mem_Globs memGlobs = { NULL };

void Mem_Initialize()
{
    for (U32 loop = 0; loop < MEMORY_MAXHANDLES; loop++)
    {
        memGlobs.handleList[loop].addr = NULL;
        memGlobs.handleList[loop].flags = MEMORY_HANDLE_FLAG_NONE;
    }
}

void Mem_Shutdown(B32 freeAll)
{
#ifdef _DEBUG
    // TODO: Implement Mem_Shutdown
#endif
}
