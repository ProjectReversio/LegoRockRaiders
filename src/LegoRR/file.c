#include <stdlib.h>
#include <stdio.h>
#include <direct.h>
#include <string.h>
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

void File_ErrorFile(char* msg, ...)
{
#ifdef _DEBUG
    // TODO: Implement File_ErrorFile
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
