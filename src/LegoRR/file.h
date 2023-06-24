#pragma once

#include "platform.h"

#define FILE_DATADIRNAME "Data"
#define FILE_KEYFILENAME "cd.key"
#define FILE_DELMEFILENAME "delme.dat"
#define FILE_MAXPATH 260

#define WAD_ERROR -1

#define StdFile(f) ((FILE*)((f)->std))
#define WadFile(f) ((WadFile*)((f)->wad))
#ifdef __cplusplus
extern "C"
{
#endif
enum {
    File_SeekSet,
    File_SeekCur,
    File_SeekEnd
};

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
    void (*loadCallback)(const char*, U32, void*);
    void* loadCallbackData;
    char cdLetter;
    U8 padding1[3];
} File_Globs, *lpFile_Globs;

extern File_Globs fileGlobs;

extern void File_Initialize(const char* programName, B32 insistOnCD, const char* registryLocation);

extern void File_LogFileAccess(B32 mode);
extern void File_ErrorFile(char* msg, ...);

extern void File_Error(char* msg, ...);

extern void* _File_Malloc(S32 size);
extern void _File_Dealloc(lpFile file);
extern void _File_Free(void* ptr);

extern const char *_File_GetWadName(char *fName);

extern B32 File_Exists(const char* fName);

extern char* File_VerifyFilename(const char* filename);
extern FileSys _File_CheckSystem(const char* fName, const char* mode);
extern FileSys _File_GetSystem(lpFile f);

extern S32 File_LoadWad(char* fName);
extern void File_CloseWad(S32 hWad);
extern void File_SetActiveWad(S32 hWad);
extern B32 File_SetBaseSearchPath(const char* basePath);

extern B32 File_GetCDFilePath(const char* path, const char* fname);

extern lpFile _File_Alloc(FileSys fType);

extern B32 _File_OpenWad(WadFile* wad, const char* fName);

extern lpFile File_Open(const char* fName, char* mode);
extern S32 File_Close(lpFile f);
extern void File_CheckRedundantFiles(const char* logName);

extern S32 File_Seek(lpFile f, S32 pos, S32 mode);
extern S32 File_Tell(lpFile f);
extern S32 File_Read(void* buffer, S32 size, S32 count, lpFile f);

extern U32 File_VPrintF(lpFile file, const char* msg, va_list args);
extern S32 File_Flush(lpFile file);

extern void* File_LoadBinary(const char* filename, U32* sizeptr);
extern void* File_LoadASCII(const char* filename, U32* sizeptr);
extern void* File_Load(const char*  filename, U32* sizeptr, B32 binary);

extern void File_SetLoadCallback(void (*callback)(const char* filename, U32 filesize, void* data), void* data);

extern char* File_GetS(char* fgetsBuffer, S32 num, lpFile f);
extern S32 File_GetC(lpFile f);

#ifdef __cplusplus
}
#endif