#pragma once

#include "types.h"

typedef enum FileSys
{
    FileSys_Wad,
    FileSys_Standard,
    FileSys_Error,
} FileSys;

typedef struct WadFile
{
    S32 hFile;
    S32 streamPos;
    B32 eof;
} WadFile;

typedef struct File
{
    FileSys type;
    union
    {
        void* std;
        WadFile* wad;
    };
} File, *lpFile;

extern void File_Initialize(const char* programName, B32 insistOnCD, const char* registryLocation);
