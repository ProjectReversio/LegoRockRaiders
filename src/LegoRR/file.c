#include <stdlib.h>
#include <stdio.h>
#include <direct.h>
#include <string.h>
#include <stdarg.h>
#include "file.h"
#include "error.h"
#include "wad.h"
#include "mem.h"
#include "utils.h"

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

void File_Error(char* msg, ...)
{
    va_list args;
    char buff[1024];
    va_start(args, msg);
    vsprintf(buff, msg, args);
    OutputDebugString(buff);

#ifdef LEGORR_FIX_CLION_CONSOLE_OUTPUT
    // TEMP: Not in original source, but needed for CLion console output
    {
        setvbuf(stdout, NULL, _IONBF, 0);
        printf("%s", buff);
    }
#endif

    va_end(args);
}

void* _File_Malloc(S32 size)
{
    return Mem_Alloc(size);
}

void _File_Dealloc(lpFile file)
{
    if (file)
    {
        if (_File_GetSystem(file) == FileSys_Standard)
        {
            if (file->std)
                fclose(StdFile(file));
            _File_Free(file);
        } else if (_File_GetSystem(file) == FileSys_Wad)
        {
            if (file->wad)
            {
                Wad_FileClose(file->wad->hFile);
                _File_Free(file->wad);
            }
            _File_Free(file);
        }
    }
}

void _File_Free(void* ptr)
{
    Mem_Free(ptr);
}

const char *_File_GetWadName(char *fName)
{
    static char wadedName[MAX_WAD_BASE_PATH];

    if (basePathSet)
    {
        S32 fLen = strlen(fName);
        S32 wLen = strlen(wadBasePath);
        if (fLen <= wLen)
            return fName;
        else
        {
            char copy = fName[wLen], *ptr;
            fName[wLen] = 0;
            if (!_stricmp(fName, wadBasePath))
            {
                ptr = fName + wLen + 1;
                sprintf(wadedName, "%s", ptr);
                fName[wLen] = copy;
                return wadedName;
            } else {
                fName[wLen] = copy;
                return fName;
            }
        }
    } else
    {
        return fName;
    }
}

B32 File_Exists(const char* fName)
{
    const char* fullName = File_VerifyFilename(fName);
    FileSys fs = _File_CheckSystem(fullName, "r");
    if (fs == FileSys_Standard)
    {
        FILE *f;
        f = fopen(fullName, "r");
        if (f)
        {
            fclose(f);
            return TRUE;
        } else {
            char cdName[FILE_MAXPATH];
            if (File_GetCDFilePath(cdName, fName))
            {
                if ((f = fopen(cdName, "r")))
                {
                    fclose(f);
                    return TRUE;
                }
            }
            return FALSE;
        }
    } else if (fs == FileSys_Wad)
    {
        return Wad_IsFileInWad(fullName, currWadHandle);
    } else {
        return FALSE;
    }
}

char* File_VerifyFilename(const char* filename)
{
    static char full[_MAX_PATH];
    char part[_MAX_PATH];
    char *temp;

    if (filename != NULL)
    {
        // REMOVE THE LEADING FORWARD SLASH IF ANY
        temp = filename;
        if (*temp == '\\')
            temp++;

        if (*temp == '@')
        {
            temp++;
            sprintf(full, "%s\\%s", fileGlobs.dataDir, temp);
            return full;
        } else {
            sprintf(part, "%s\\%s", fileGlobs.dataDir, temp);
        }

        if (_fullpath(full, part, _MAX_PATH) != NULL)
        {
            if (strncmp(full, fileGlobs.dataDir, strlen(fileGlobs.dataDir)) == 0)
                return full;
        }
    }

    Error_Warn(TRUE, Error_Format("Cannot verify file name \"%s\".", filename));
    Error_LogLoadError(TRUE, Error_Format("%d\t%s", Error_LoadError_UnableToVerifyName, filename));

    return NULL;
}

FileSys _File_CheckSystem(const char* fName, const char* mode)
{
    if (!fName || !mode || !strlen(fName) || !strlen(mode))
        return FileSys_Error;

    if (mode[0] == 'w' || mode[0] == 'W')
    {
        // File must be opened as stdC
        return FileSys_Standard;
    } else {
        if (Wad_IsFileInWad(_File_GetWadName(fName), currWadHandle) != WAD_ERROR)
        {
            // The file is in the wad so we can use the wad version
            return FileSys_Wad;
        } else {
            // Otherwise we will try the normal file system
            return FileSys_Standard;
        }
    }
}

FileSys _File_GetSystem(lpFile f)
{
    return f->type;
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

B32 File_GetCDFilePath(const char* path, const char* fname)
{
    // TODO: Implement File_GetCDFilePath
    return FALSE;
}

lpFile _File_Alloc(FileSys fType)
{
    if (fType == FileSys_Standard)
    {
        lpFile f;
        f = (lpFile) _File_Malloc(sizeof(File));
        f->type = fType;
        return f;
    } else if (fType == FileSys_Wad)
    {
        lpFile f = (lpFile) _File_Malloc(sizeof(File));
        if (f)
        {
            f->type = fType;
            f->wad = (WadFile*) _File_Malloc(sizeof(WadFile));
            if (f->wad)
                return f;
            else
                _File_Free(f);
        }
    }
    return NULL;
}

B32 _File_OpenWad(WadFile* wad, const char* fName)
{
    wad->eof = FALSE;
    wad->streamPos = 0;
    wad->hFile = Wad_FileOpen(fName, currWadHandle);
    return wad->hFile == WAD_ERROR ? FALSE : TRUE;
}

lpFile File_Open(const char* fName, char* mode)
{
    U8* fullName = File_VerifyFilename(fName);
    FileSys fs = _File_CheckSystem(fullName, mode);
    lpFile file;

    switch (fs)
    {
        case FileSys_Standard:
            file = _File_Alloc(fs);
            if (!file)
                return NULL;
            file->std = fopen(fullName, mode);
            if (file->std)
            {
                File_ErrorFile("STD Load %s\n", fullName);
                return file;
            } else {
                if (Util_StrIStr(mode, "w"))
                {
                    char cdName[FILE_MAXPATH];

                    if (File_GetCDFilePath(cdName, fName))
                    {
                        if ((file->std = fopen(cdName, mode)))
                            return file;
                    }
                }

                File_ErrorFile("STD Fail %s\n", fullName);
                _File_Dealloc(file);
                return NULL;
            }
            break;

        case FileSys_Wad:
            file = _File_Alloc(fs);
            if (!file)
                return NULL;

            if (_File_OpenWad(file->wad, _File_GetWadName(fullName)) != FALSE)
            {
                File_ErrorFile("WAD Load %s\n", _File_GetWadName(fullName));
                return file;
            } else {
                File_ErrorFile("WAD Fail %s\n", _File_GetWadName(fullName));
                _File_Dealloc(file);
                return NULL;
            }
            break;

        case FileSys_Error:
        default:
            File_Error("%s(%i) : Error in call to %s\n", __FILE__, __LINE__, "File_Open");
            break;
    }
    return NULL;
}

S32 File_Close(lpFile f)
{
    FileSys fs = _File_GetSystem(f);
    switch (fs)
    {
        case FileSys_Standard:
        case FileSys_Wad:
            _File_Dealloc(f);
            break;
        case FileSys_Error:
        default:
            File_Error("%s(%i) : Unknown file system in call to %s", __FILE__, __LINE__, "File_Close");
            break;
    }
    return 0;
}

void File_CheckRedundantFiles(const char* logName)
{
    // TODO: Implement File_CheckRedundantFiles
}

S32 File_Seek(lpFile f, S32 pos, S32 mode)
{
    FileSys fs = _File_GetSystem(f);
    switch (fs)
    {
        case FileSys_Standard:
            return fseek(StdFile(f), pos, mode);
            break;
        case FileSys_Wad:
            switch(mode)
            {
                case SEEK_SET:
                    WadFile(f)->streamPos = pos;
                    if (WadFile(f)->streamPos > Wad_hLength(WadFile(f)->hFile))
                        WadFile(f)->streamPos = Wad_hLength(WadFile(f)->hFile);
                    if (WadFile(f)->streamPos < 0)
                        WadFile(f)->streamPos = 0;
                    break;
                case SEEK_CUR:
                    WadFile(f)->streamPos += pos;
                    if (WadFile(f)->streamPos > Wad_hLength(WadFile(f)->hFile))
                        WadFile(f)->streamPos = Wad_hLength(WadFile(f)->hFile);
                    if (WadFile(f)->streamPos < 0)
                        WadFile(f)->streamPos = 0;
                    break;
                case SEEK_END:
                    WadFile(f)->streamPos = Wad_hLength(WadFile(f)->hFile) + pos;
                    break;
                default:
                    File_Error("Unknown seek mode (%i)", mode);
            }
            break;
        case FileSys_Error:
        default:
            File_Error("%s(%i) : Unknown file system in call to %s", __FILE__, __LINE__, "File_Seek");
            break;
    }
    return 0;
}

S32 File_Tell(lpFile f)
{
    FileSys fs = _File_GetSystem(f);
    switch (fs)
    {
        case FileSys_Standard:
            return ftell(StdFile(f));
            break;
        case FileSys_Wad:
            return WadFile(f)->streamPos;
            break;
        case FileSys_Error:
        default:
            File_Error("%s(%i) : Unknown file system in call to %s", __FILE__, __LINE__, "File_Tell");
            break;
    }

    return 0;
}

S32 File_Read(void* buffer, S32 size, S32 count, lpFile f)
{
    FileSys fs = _File_GetSystem(f);
    switch (fs)
    {
        case FileSys_Standard:
            return fread(buffer, size, count, StdFile(f));
            break;
        case FileSys_Wad:
        {
            S32 len = Wad_hLength(WadFile(f)->hFile);
            S32 amountToCopy;
            if ((WadFile(f)->streamPos + (size * count)) > len)
                amountToCopy = len - WadFile(f)->streamPos;
            else
                amountToCopy = size * count;
            memcpy(buffer, (char*)Wad_hData(WadFile(f)->hFile) + WadFile(f)->streamPos, amountToCopy);
            WadFile(f)->streamPos += amountToCopy;
            return amountToCopy / size;
        }
        break;
        case FileSys_Error:
        default:
            File_Error("%s(%i) : Unknown file system in call to %s", __FILE__, __LINE__, "File_Read");
            break;
    }
    return 0;
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
    lpFile ifp;
    U32 size;
    U8* buffer;

    if ((ifp = File_Open(filename, binary ? "rb" : "r")))
    {
        File_Seek(ifp, 0, File_SeekEnd);
        size = File_Tell(ifp);

#ifdef _DEBUG
        Error_Fatal(size == 0, Error_Format("Attempt to File_LoadBinary() a file of zero length \"%s\"", filename));
#endif // _DEBUG

        if (fileGlobs.loadCallback)
            fileGlobs.loadCallback(filename, size, fileGlobs.loadCallbackData);

        if ((buffer = Mem_Alloc(size)))
        {
            File_Seek(ifp, 0, File_SeekSet);
            File_Read(buffer, sizeof(U8), size, ifp);
            if (sizeptr != NULL)
                *sizeptr = size;

            File_Close(ifp);
            return buffer;
        }
        File_Close(ifp);
    }
    return NULL;
}

void File_SetLoadCallback(void (*callback)(const char* filename, U32 filesize, void* data), void* data)
{
    fileGlobs.loadCallback = callback;
    fileGlobs.loadCallbackData = data;
}

S32 File_GetC(lpFile f)
{
    FileSys fs = _File_GetSystem(f);
    S32 len;
    switch (fs)
    {
        case FileSys_Standard:
            return fgetc(StdFile(f));
            break;
        case FileSys_Wad:
            len = Wad_hLength(WadFile(f)->hFile);
            if (WadFile(f)->streamPos >= len - 1)
                return EOF;
            else
            {
                S32 c = *((int*)((char*)Wad_hData(WadFile(f)->hFile) + WadFile(f)->streamPos));
                WadFile(f)->streamPos++;
                return c;
            }
            break;
        case FileSys_Error:
        default:
            File_Error("%s(%i) : Unknown file system in call to %s", __FILE__, __LINE__, "File_GetC");
            break;
    }
    return 0;
}

char* File_InternalFGetS(char* fgetsBuffer, S32 num, lpFile f)
{
    S32 pos = 0;
    while ((fgetsBuffer[pos] = (char)File_GetC(f)) != 0 && fgetsBuffer[pos] != '\n' && fgetsBuffer[pos] != EOF && pos != num)
    {
        pos++;
    }
    fgetsBuffer[pos + 1] = 0;
    if (pos == 0)
        return 0;
    else
        return fgetsBuffer;
}

char* File_GetS(char* fgetsBuffer, S32 num, lpFile f)
{
    FileSys fs = _File_GetSystem(f);
    switch (fs)
    {
        case FileSys_Standard:
            return fgets(fgetsBuffer, num, StdFile(f));
            break;
        case FileSys_Wad:
            return File_InternalFGetS(fgetsBuffer, num, f);
            break;
        case FileSys_Error:
        default:
            File_Error("%s(%i) : Unknown file system in call to %s", __FILE__, __LINE__, "File_GetS");
            break;
    }
    return NULL;
}
