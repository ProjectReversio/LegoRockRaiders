#pragma once

#include "types.h"

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

extern void Mem_Initialize();
extern void Mem_Shutdown(B32 freeAll);
