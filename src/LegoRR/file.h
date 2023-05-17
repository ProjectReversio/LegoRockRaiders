#pragma once

#include "types.h"

#define FILE_DATADIRNAME "Data"
#define FILE_KEYFILENAME "cd.key"
#define FILE_DELMEFILENAME "delme.dat"
#define FILE_MAXPATH 260

#define WAD_ERROR -1

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

typedef struct File_Globs
{
    char dataDir[FILE_MAXPATH];
    void (*loadCallback)();
    void* loadCallbackData;
    char cdLetter;
    U8 padding1[3];
} File_Globs, *lpFile_Globs;

extern File_Globs fileGlobs;

extern void File_Initialize(const char* programName, B32 insistOnCD, const char* registryLocation);

extern void File_LogFileAccess(B32 mode);
extern void File_ErrorFile(char* msg, ...);

extern S32 File_LoadWad(char* fName);
extern void File_CloseWad(S32 hWad);
extern void File_SetActiveWad(S32 hWad);
extern B32 File_SetBaseSearchPath(const char* basePath);
