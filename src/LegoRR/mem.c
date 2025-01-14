#include "mem.h"

#include "error.h"

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

void* Mem_AllocDebug(U32 size, U32 line, const char* file)
{
    // TODO: Implement Mem_AllocDebug
    return Mem_Alloc(size);
}

#ifdef _DEBUG
U32 Mem_AllocHandleDebug(U32 size, U32 line, const char* file)
{
    // TODO: Implement Mem_AllocHandleDebug
    Mem_AllocHandle(size);
}
#endif

U32 Mem_AllocHandle(U32 size)
{
    // Simple backwards compatibility routine...

    U32 loop;

    for (loop = 0; loop < MEMORY_MAXHANDLES; loop++)
    {
        if (!(memGlobs.handleList[loop].flags & MEMORY_HANDLE_FLAG_USED))
        {
            // TODO: Mem_AllocDebug when Debug
            memGlobs.handleList[loop].addr = Mem_Alloc(size);
            memGlobs.handleList[loop].flags = MEMORY_HANDLE_FLAG_USED;
            return loop;
        }
    }

    Error_Fatal(TRUE, "Ran out of memory handles");
    return -1;
}

void Mem_FreeHandle(U32 handle)
{
    if (handle >= 0 && handle < MEMORY_MAXHANDLES)
    {
        if (memGlobs.handleList[handle].flags & MEMORY_HANDLE_FLAG_USED)
        {
            Mem_Free(memGlobs.handleList[handle].addr);
            memGlobs.handleList[handle].addr = NULL;
            memGlobs.handleList[handle].flags = MEMORY_HANDLE_FLAG_NONE;
        } else
        {
            Error_Warn(TRUE, "Handle has already been freed");
        }
    }
    else
    {
        Error_Warn(TRUE, "Handle is out of range");
    }
}

void* Mem_AddressHandle(U32 handle)
{
    if (handle >= 0 && handle < MEMORY_MAXHANDLES)
    {
        if (memGlobs.handleList[handle].flags & MEMORY_HANDLE_FLAG_USED)
            return memGlobs.handleList[handle].addr;

        Error_Warn(TRUE, "Handle has already been freed");
    } else
    {
        Error_Warn(TRUE, "Handle is out of range");
    }

    return NULL;
}
