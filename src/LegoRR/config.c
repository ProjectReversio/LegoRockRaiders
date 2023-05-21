#include <string.h>
#include <stdarg.h>
#include <stdio.h>
#include "config.h"
#include "mem.h"
#include "fonts.h"
#include "error.h"

Config_Globs configGlobs = { NULL };

void Config_Initialize()
{
    for (U32 loop = 0; loop < CONFIG_MAXLISTS; loop++)
    {
        configGlobs.listSet[loop] = NULL;
    }

    configGlobs.freeList = NULL;
    configGlobs.listCount = 0;
    configGlobs.flags = CONFIG_GLOB_FLAG_INITIALIZED;
}

void Config_Shutdown()
{
    for (U32 loop = 0; loop < CONFIG_MAXLISTS; loop++)
    {
        if (configGlobs.listSet[loop])
            Mem_Free(configGlobs.listSet[loop]);
    }

    configGlobs.freeList = NULL;
    configGlobs.flags = CONFIG_GLOB_FLAG_NONE;
}

const char* Config_BuildStringID(const char* s, ...)
{
    va_list args;
    static char string[1024];
    char* curr;

    strcpy(string, s);

    va_start(args, s);
    while ((curr = va_arg(args, char*)))
    {
        strcat(string, CONFIG_SEPARATOR);
        strcat(string, curr);
    }
    va_end(s);

    return string;
}

#ifdef _DEBUG
const char* Config_Debug_GetLastFind()
{
    static U8 msg[1024];

    if (configGlobs.debugLastFind)
    {
        sprintf(msg, "%s %s", configGlobs.debugLastFind->itemName, configGlobs.debugLastFind->dataString);
        return msg;
    }

    return NULL;
}
#endif // _DEBUG

#define IS_SPACE(c) (((c) == '\t') || ((c) == '\n') || ((c) == '\r') || ((c) == ' '))

lpConfig Config_Load(const char* filename)
{
    lpConfig conf, rootConf = NULL;
    U32 fileSize, loop;
    U8 *s, *fdata;
    B32 commentMode = FALSE;

    configGlobs.flags |= CONFIG_GLOB_FLAG_LOADINGCONFIG;

    if ((fdata = File_LoadBinary(filename, &fileSize)))
    {
        conf = rootConf = Config_Create(NULL);
        rootConf->fileData = fdata;

        // Change any return/tab/blah/blah characters to zero...
        // Clear anything after a semi-colon until the next return character.

        for (s = rootConf->fileData, loop = 0; loop < fileSize; loop++)
        {
            if (*s == ';')
                commentMode = TRUE;
            else if (*s == '\n')
                commentMode = FALSE;

            if (commentMode || IS_SPACE(*s))
                *s = '\0';

            s++;
        }

        // Replace the semi-colons that were removed by the language converter...

        for (loop = 0; loop < fileSize; loop++)
        {
            if (rootConf->fileData[loop] == FONT_LASTCHARACTER + 1)
                rootConf->fileData[loop] = ';';
        }

        // Run through the file data and point in the config structures

        for (s = rootConf->fileData, loop = 0; loop < fileSize; loop++)
        {
            if (*s != '\0')
            {
                if (*s == '}' && *(s+1) == '\0')
                    conf->depth--;
                else if (conf->itemName == NULL)
                {
                    Error_Fatal(*s == '{', "Config");
                    conf->itemName = s;
                } else {
                    conf->dataString = s;
                    conf = Config_Create(conf);
                    if (*s == '{' && *(s+1) == '\0')
                        conf->depth++;
                }

                for (; loop < fileSize; loop++)
                {
                    if (*(s++) == '\0')
                        break;
                }
            } else {
                s++;
            }
        }
    }

    configGlobs.flags &= ~CONFIG_GLOB_FLAG_LOADINGCONFIG;

    return rootConf;
}

lpConfig Config_Create(lpConfig prev)
{
    lpConfig newConfig;

    Config_CheckInit();

    if (configGlobs.freeList == NULL)
        Config_AddList();

    newConfig = configGlobs.freeList;
    configGlobs.freeList = newConfig->nextFree;
    newConfig->nextFree = newConfig;

    newConfig->itemName = NULL;
    newConfig->dataString = NULL;
    newConfig->linkNext = NULL;

    if (prev != NULL)
    {
        prev->linkNext = newConfig;
        newConfig->linkPrev = prev;
        newConfig->depth = prev->depth;
        newConfig->fileData = NULL;
    } else {
        newConfig->depth = 0;
        newConfig->linkPrev = NULL;
    }

    return newConfig;
}

void Config_Remove(lpConfig dead)
{
    Config_CheckInit();
    Error_Fatal(!dead, "NULL passed to Config_Remove()");

#ifdef _DEBUG
    if (configGlobs.debugLastFind == dead)
        configGlobs.debugLastFind = NULL;
#endif // _DEBUG

    dead->nextFree = configGlobs.freeList;
    configGlobs.freeList = dead;
}

void Config_AddList()
{
    lpConfig list;
    U32 loop, count;

    Config_CheckInit();

    Error_Fatal(configGlobs.listCount + 1 >= CONFIG_MAXLISTS, "Run out of lists");

    count = 0x00000001 << configGlobs.listCount;

    list = configGlobs.listSet[configGlobs.listCount] = Mem_Alloc(sizeof(Config) * count);
    if (list)
    {
        configGlobs.listCount++;

        for (loop = 1; loop < count; loop++)
        {
            list[loop - 1].nextFree = &list[loop];
        }

        list[count - 1].nextFree = configGlobs.freeList;
        configGlobs.freeList = list;

    } else {
        Error_Fatal(TRUE, Error_Format("Unable to allocate %d bytes of memory for new list.\n", sizeof(Config) * count));
    }
}
