#pragma once

#include <windows.h>
#include "types.h"

#define MAX_WADS 10

typedef struct WadEntry
{
    S32 compression;
    S32 fileLength;
    S32 decompressedLength;
    union
    {
        S32 addr;
        void* ptr;
    };
} WadEntry;

typedef struct Wad
{
    char* fName;
    B32 active;
    HANDLE hFile, hFileMapping;
    FILE* fWad;

    char **fileNames;
    char **wadNames;
    WadEntry *wadEntries;
    S32 numFiles;
} Wad;

extern S32 Wad_Load(char* fName);
extern void Wad_Close(S32 wadNo);

extern void Wad_FileClose(S32 handle);

extern S32 Wad_GetFreeWadSlot();
extern Wad* Wad_Get(int wadNo);
extern B32 GetFileName(FILE* f, char* buffer);

extern S32 Wad_IsFileInWad(const char *fName, S32 wadNo);

extern void Wad_Error(char *msg, ...);
