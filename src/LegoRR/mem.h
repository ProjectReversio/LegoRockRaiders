#pragma once

#include "platform.h"
#include <malloc.h>

#define MEMORY_MAXHANDLES 2000

typedef enum Mem_HandleFlags
{
    MEMORY_HANDLE_FLAG_NONE = 0,
    MEMORY_HANDLE_FLAG_USED = 1,
} Mem_HandleFlags;

typedef struct Mem_Handle
{
    void* addr;
    Mem_HandleFlags flags;
} Mem_Handle, *lpMem_Handle;

typedef struct Mem_Globs
{
    Mem_Handle handleList[MEMORY_MAXHANDLES];
    U32 flags; // unused?
} Mem_Globs, *lpMem_Globs;

extern Mem_Globs memGlobs;

#ifdef _DEBUG
#define Mem_DebugTrash(a, v, s) Mem_Debug_FillWith((a), (v), (s))
#else
#define Mem_DebugTrash(a, v, s)
#endif // _DEBUG

extern void Mem_Initialize();
extern void Mem_Shutdown(B32 freeAll);

#ifdef _DEBUG
extern void* Mem_AllocDebug(U32 size, U32 line, const char* file);
extern U32 Mem_AllocHandleDebug(U32 size, U32 line, const char* file);
extern void Mem_Debug_FillWith(void* addr, U32 value, U32 size);
#endif

inline void* Mem_Alloc(U32 size)
{
    return malloc(size);
}

extern U32 Mem_AllocHandle(U32 size);

inline void* Mem_ReAlloc(void* addr, U32 size)
{
    return realloc(addr, size);
}

inline void Mem_Free(void* addr)
{
    free(addr);
}

extern void Mem_FreeHandle(U32 handle);
extern void* Mem_AddressHandle(U32 handle);
