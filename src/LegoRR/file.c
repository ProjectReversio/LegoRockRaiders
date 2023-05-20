#include <stdlib.h>
#include <stdio.h>
#include <direct.h>
#include <string.h>
#include <stdarg.h>
#include "file.h"
#include "error.h"
#include "wad.h"

File_Globs fileGlobs = { NULL };

#define MAX_WAD_BASE_PATH 1024
static char wadBasePath[MAX_WAD_BASE_PATH];
static B32 basePathSet = FALSE;
static B32 fileLogFileAccess = FALSE;

static S32 currWadHandle = WAD_ERROR; // Current wad handle.  All fopens are relative to this wad.

void File_Initialize(const char* programName, B32 insistOnCD, const char* registryLocation)
{
    S32 wad = WAD_ERROR;
    B32 foundCD = FALSE;
    B32 foundWad = FALSE;
    U8 cwd[_MAX_PATH];
    FILE* fp;
    U8 fname[1024];
    B32 failed = FALSE;

    _getcwd(cwd, _MAX_PATH);
    if ('\\' == cwd[strlen(cwd) - 1])
        cwd[strlen(cwd) - 1] = '\0';
    sprintf(fileGlobs.dataDir, "%s\\%s", cwd, FILE_DATADIRNAME);

    {
        U8 wadFile[FILE_MAXPATH];
        U8 cwd[FILE_MAXPATH];
        for (U32 i = 0; i < MAX_WADS; i++)
        {
            sprintf(wadFile, "%s%i.wad", programName, i);
            wad = File_LoadWad(wadFile);
            if (wad == WAD_ERROR)
            {
                Error_Warn(TRUE, Error_Format("Cannot load %s", wadFile));
            } else {
                foundWad = TRUE;
            }
        }
        _getcwd(cwd, FILE_MAXPATH);
        strcat(cwd, "\\Data");
        File_SetBaseSearchPath(cwd);
    }

    // TODO: Implement File_Initialize
}

void File_LogFileAccess(B32 mode)
{
    fileLogFileAccess = mode;
}

void File_ErrorFile(char* msg, ...)
{
#ifdef _DEBUG
    static FILE *f;
    va_list arg;

    if (fileLogFileAccess)
    {
        va_start(arg, msg);
        if (f)
        {
            if (msg)
                vfprintf(f, msg, arg);
            else
            {
                fclose(f);
                f = 0;
            }
        } else if (!msg)
        {
            f = fopen("file.log", "w");
            fprintf(f, "File access log\n");
        }
        va_end(arg);
    }
#endif
}

S32 File_LoadWad(char* fName)
{
    File_ErrorFile(0); // Open the wad error log
    return Wad_Load(fName);
}

void File_CloseWad(S32 hWad)
{
    File_ErrorFile(0); // Close the wad error log
    if (hWad == WAD_ERROR)
        Wad_Close(hWad);
}

void File_SetActiveWad(S32 hWad)
{
    currWadHandle = hWad;
}

B32 File_SetBaseSearchPath(const char* basePath)
{
    S32 len;
    if (!basePath || (len = strlen(basePath)) > MAX_WAD_BASE_PATH || !len)
    {
        memset(wadBasePath, 0, MAX_WAD_BASE_PATH);
        basePathSet = FALSE;
        return FALSE;
    } else {
        strcpy(wadBasePath, basePath);
        basePathSet = TRUE;
        return TRUE;
    }
}

lpFile File_Open(const char* fName, char* mode)
{
    // TODO: Implement File_Open
    return NULL;
}

S32 File_Close(lpFile f)
{
    // TODO: Implement File_Close
    return 0;
}

void File_CheckRedundantFiles(const char* logName)
{
    // TODO: Implement File_CheckRedundantFiles
}

U32 File_VPrintF(lpFile file, const char* msg, va_list args)
{
    // TODO: Implement File_VPrintF
    return 0;
}

S32 File_Flush(lpFile file)
{
    // TODO: Implement File_Flush
    return 0;
}

void* File_LoadBinary(const char* filename, U32* sizeptr)
{
    return File_Load(filename, sizeptr, TRUE);
}

void* File_LoadASCII(const char* filename, U32* sizeptr)
{
    return File_Load(filename, sizeptr, FALSE);
}

void* File_Load(const char*  filename, U32* sizeptr, B32 binary)
{
    // TODO: Implement File_Load
    return NULL;
}