#include <stdio.h>
#include <string.h>
#include "wad.h"
#include "file.h"
#include "compress.h"

// Number of WADS you can have at one time
#define MAX_OPEN_FILES 100
#define MAX_FILE_COMPRESSION_SIZE (1024*1024)

#define WAD(which) wadGlobs.wads[which]
#define CURWAD wadGlobs.wads[wadGlobs.activeWad]



struct
{
    void* data;             // Pointer to the file data
    B32 active;             // Is this handle active already
    S32 wadFile;            // Wad file this handle uses
    S32 indexOfFileInWad;   // Index of the file in the wad structure
} fileHandles[MAX_OPEN_FILES];

struct
{
    Wad wads[MAX_WADS];         // Wad structures
    S32 references[MAX_WADS];   // Current count of references to the wad file
} wadGlobs;

S32 Wad_Load(char* fName)
{
    S32 newWad, i, counter = 0;
    S8 header[4];
    FILE *f;

    newWad = Wad_GetFreeWadSlot();
    if (newWad == WAD_ERROR)
        return WAD_ERROR;

    // Load the wad
    f = Wad_Get(newWad)->fWad = fopen(fName, "rb");
    if (!f)
        return WAD_ERROR;

    Wad_Get(newWad)->active = TRUE;

    // Check to see if we actually have a wad file.
    if (fread(header, 1, 4, f) != 4)
    {
        fclose(f);
        return WAD_ERROR;
    }
    if (memcmp(header, "WWAD", 4) != 0)
    {
        fclose(f);
        return WAD_ERROR;
    }

    // Get the number of files to load
    if (fread(&(Wad_Get(newWad)->numFiles), 1, sizeof(int), f) != sizeof(int))
    {
        fclose(f);
        return WAD_ERROR;
    }

    // The names are first
    Wad_Get(newWad)->wadNames = (char**) malloc(sizeof(char*) * Wad_Get(newWad)->numFiles);
    Wad_Get(newWad)->fileNames = (char**) malloc(sizeof(char*) * Wad_Get(newWad)->numFiles);
    if (!Wad_Get(newWad)->wadNames || !Wad_Get(newWad)->fileNames)
    {
        fclose(f);
        if (Wad_Get(newWad)->wadNames)
            free(Wad_Get(newWad)->wadNames);
        if (Wad_Get(newWad)->fileNames)
            free(Wad_Get(newWad)->fileNames);
        return WAD_ERROR;
    }
    memset(Wad_Get(newWad)->wadNames, 0, sizeof(char*) * Wad_Get(newWad)->numFiles);
    memset(Wad_Get(newWad)->fileNames, 0, sizeof(char*) * Wad_Get(newWad)->numFiles);

    // Reads the names of the files in the wad and on disk
    while(counter < Wad_Get(newWad)->numFiles)
    {
        char buffer[4096]; // Nice big buffer to read into likes.
        if (!GetFileName(f, buffer))
        {
            if (Wad_Get(newWad)->wadNames)
            {
                for (i = 0; i < Wad_Get(newWad)->numFiles; i++)
                {
                    if (Wad_Get(newWad)->wadNames[i])
                        free(Wad_Get(newWad)->wadNames[i]);
                }
                free(Wad_Get(newWad)->wadNames);
            }
            if (Wad_Get(newWad)->fileNames)
                free(Wad_Get(newWad)->fileNames);
            fclose(f);
            return WAD_ERROR;
        } else {
            Wad_Get(newWad)->wadNames[counter] = (char*) malloc(strlen(buffer) + 1);
            strcpy(Wad_Get(newWad)->wadNames[counter], buffer);

            Wad_Error("%s\n", Wad_Get(newWad)->wadNames[counter]);
        }

        counter++;
    }
    counter = 0;

    while(counter < Wad_Get(newWad)->numFiles)
    {
        char buffer[4096]; // Nice big buffer to read into likes.
        if (!GetFileName(f, buffer))
        {
            if (Wad_Get(newWad)->wadNames)
            {
                for (i = 0; i < Wad_Get(newWad)->numFiles; i++)
                {
                    if (Wad_Get(newWad)->wadNames[i])
                        free(Wad_Get(newWad)->wadNames[i]);
                }
                free(Wad_Get(newWad)->wadNames);
            }
            if (Wad_Get(newWad)->fileNames)
            {
                for (i = 0; i < Wad_Get(newWad)->numFiles; i++)
                {
                    if (Wad_Get(newWad)->fileNames[i])
                        free(Wad_Get(newWad)->fileNames[i]);
                }
                free(Wad_Get(newWad)->fileNames);
            }
            fclose(f);
            return WAD_ERROR;
        } else {
            Wad_Get(newWad)->fileNames[counter] = (char*) malloc(strlen(buffer) + 1);
            strcpy(Wad_Get(newWad)->fileNames[counter], buffer);

            Wad_Error("%s\n", Wad_Get(newWad)->fileNames[counter]);
        }

        counter++;
    }

    // Now we get the data.  Address of file, compression
    Wad_Get(newWad)->wadEntries = (WadEntry*) malloc(sizeof(WadEntry) * Wad_Get(newWad)->numFiles);
    if (!Wad_Get(newWad)->wadEntries)
    {
        if (Wad_Get(newWad)->wadNames)
        {
            for (i = 0; i < Wad_Get(newWad)->numFiles; i++)
            {
                if (Wad_Get(newWad)->wadNames[i])
                    free(Wad_Get(newWad)->wadNames[i]);
            }
            free(Wad_Get(newWad)->wadNames);
        }
        if (Wad_Get(newWad)->fileNames)
        {
            for (i = 0; i < Wad_Get(newWad)->numFiles; i++)
            {
                if (Wad_Get(newWad)->fileNames[i])
                    free(Wad_Get(newWad)->fileNames[i]);
            }
            free(Wad_Get(newWad)->fileNames);
        }

        fclose(f);
        return WAD_ERROR;
    }

    if (fread(Wad_Get(newWad)->wadEntries, 1, sizeof(WadEntry) * Wad_Get(newWad)->numFiles, f) != (S32)(sizeof(WadEntry) * Wad_Get(newWad)->numFiles))
    {
        if (Wad_Get(newWad)->wadNames)
        {
            for (i = 0; i < Wad_Get(newWad)->numFiles; i++)
            {
                if (Wad_Get(newWad)->wadNames[i])
                    free(Wad_Get(newWad)->wadNames[i]);
            }
            free(Wad_Get(newWad)->wadNames);
        }
        if (Wad_Get(newWad)->fileNames)
        {
            for (i = 0; i < Wad_Get(newWad)->numFiles; i++)
            {
                if (Wad_Get(newWad)->fileNames[i])
                    free(Wad_Get(newWad)->fileNames[i]);
            }
            free(Wad_Get(newWad)->fileNames);
        }

        if (Wad_Get(newWad)->wadEntries)
            free(Wad_Get(newWad)->wadEntries);
        fclose(f);
        return WAD_ERROR;
    }

    return newWad;
}

void Wad_Close(S32 wadNo)
{
    S32 i;
    if (wadNo != WAD_ERROR)
    {
        if (Wad_Get(wadNo)->fileNames)
        {
            for (i = 0; i < Wad_Get(wadNo)->numFiles; i++)
            {
                if (Wad_Get(wadNo)->wadNames[i])
                    free(Wad_Get(wadNo)->wadNames[i]);
                free(Wad_Get(wadNo)->wadNames);
            }
        }
        if (Wad_Get(wadNo)->fileNames)
        {
            for (i = 0; i < Wad_Get(wadNo)->numFiles; i++)
            {
                if (Wad_Get(wadNo)->fileNames[i])
                    free(Wad_Get(wadNo)->fileNames[i]);
                free(Wad_Get(wadNo)->fileNames);
            }
        }
        if (Wad_Get(wadNo)->wadEntries)
            free(Wad_Get(wadNo)->wadEntries);
        fclose(Wad_Get(wadNo)->fWad);
    }
}

void Wad_FileClose(S32 handle)
{
    if (fileHandles[handle].active)
    {
        free(fileHandles[handle].data);
        fileHandles[handle].active = FALSE;
    }
}

S32 Wad_FindFreeFileHandle()
{
    S32 i;
    for (i = 0; i < MAX_OPEN_FILES; i++)
    {
        if (!fileHandles[i].active)
            return i;
    }
    return WAD_ERROR;
}

S32 Wad_GetFreeWadSlot()
{
    for (S32 i = 0; i < MAX_WADS; i++)
    {
        if (!WAD(i).active)
        {
            // Zero out the slot
            memset(&WAD(i), 0, sizeof(Wad));
            return i;
        }
    }

    return WAD_ERROR;
}

Wad* Wad_Get(int wadNo)
{
    return &(wadGlobs.wads[wadNo]);
}

B32 GetFileName(FILE* f, char* buffer)
{
    char c;
    while (c = fgetc(f))
    {
        *buffer = c;
        buffer++;
    }
    *buffer = 0;
    return TRUE;
}

S32 Wad_FileLength(S32 wadNo, S32 fileNo)
{
    return Wad_Get(wadNo)->wadEntries[fileNo].decompressedLength;
}

S32 Wad_FileCompressedLength(S32 wadNo, S32 fileNo)
{
    return Wad_Get(wadNo)->wadEntries[fileNo].fileLength;
}

S32 _Wad_IsFileInWad(const char *fName, S32 wadNo)
{
    S32 i;
    for (i = 0; i < wadGlobs.wads[wadNo].numFiles; i++)
    {
        if (!_stricmp(wadGlobs.wads[wadNo].wadNames[i], fName))
            return i;
    }
    return WAD_ERROR;
}

S32 Wad_IsFileInWad(const char *fName, S32 wadNo)
{
    if (wadNo == WAD_ERROR)
    {
        S32 i;
        for (i = MAX_WADS - 1; i >= 0; i--)
        {
            if (Wad_Get(i)->active)
            {
                S32 res = _Wad_IsFileInWad(fName, i);
                if (res != WAD_ERROR)
                    return res;
            }
        }
        return WAD_ERROR;
    } else {
        return _Wad_IsFileInWad(fName, wadNo);
    }
}

S32 Wad_FileOpen(const char *fName, S32 wadNo)
{
    if (wadNo == WAD_ERROR)
    {
        S32 i;
        // Look in all wads (backwards)
        for (i = MAX_WADS - 1; i >= 0; i--)
        {
            if (Wad_Get(i)->active)
            {
                S32 res = _Wad_FileOpen(fName, i);
                if (res != WAD_ERROR)
                    return res;
            }
        }
        return WAD_ERROR;
    } else {
        return _Wad_FileOpen(fName, wadNo);
    }
}

S32 _Wad_FileOpen(const char *fName, S32 wadNo)
{
    S32 indexOfFileInWad, fileHandle;
    void* ptr;

    // Search the file handles for a free one.
    if ((fileHandle = Wad_FindFreeFileHandle()) == WAD_ERROR)
        return WAD_ERROR;

    // Search the wad for the file and get its index
    if ((indexOfFileInWad = Wad_IsFileInWad(fName, wadNo)) == WAD_ERROR)
        return WAD_ERROR;

    // Allocate a block of memory for the file and read it from the disk
    ptr = malloc(Wad_FileCompressedLength(wadNo, indexOfFileInWad));
    if (!ptr)
        return WAD_ERROR;

    fseek(Wad_Get(wadNo)->fWad, Wad_Get(wadNo)->wadEntries[indexOfFileInWad].addr, SEEK_SET);
    if (fread(ptr, Wad_FileCompressedLength(wadNo, indexOfFileInWad), 1, Wad_Get(wadNo)->fWad) != 1)
    {
        free(ptr);
        return WAD_ERROR;
    }

    // If the file is compressed then it must be decompressed first
    if (Wad_Get(wadNo)->wadEntries[indexOfFileInWad].compression & WAD_FILE_RNCOMPRESSED)
    {
        void *newBuffer = 0;
        RNC_Uncompress(ptr, &newBuffer);
        free(ptr);
        ptr = newBuffer;
    }

    // Fill out the file handle data
    fileHandles[fileHandle].data = ptr;
    fileHandles[fileHandle].active = TRUE;
    fileHandles[fileHandle].wadFile = wadNo;
    fileHandles[fileHandle].indexOfFileInWad = indexOfFileInWad;

    return fileHandle;
}

void* Wad_FileGetPointer(S32 handle)
{
    if (fileHandles[handle].active)
        return fileHandles[handle].data;

    return NULL;
}

S32 Wad_hLength(S32 handle)
{
    return Wad_FileLength(fileHandles[handle].wadFile, fileHandles[handle].indexOfFileInWad);
}

void* Wad_hData(S32 handle)
{
    return Wad_FileGetPointer(handle);
}

void Wad_Error(char *msg, ...)
{
    char buffer[1024];
    va_list arg;
    va_start(arg, msg);
    vsprintf(buffer, msg, arg);
    va_end(arg);
    OutputDebugString(buffer);
//#ifdef LEGORR_FIX_CLION_CONSOLE_OUTPUT
//    // TEMP: Not in original source, but needed for CLion console output
//    {
//        setvbuf(stdout, NULL, _IONBF, 0);
//        printf("%s", buffer);
//    }
//#endif
}
