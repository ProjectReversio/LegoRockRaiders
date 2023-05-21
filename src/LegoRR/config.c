#include <string.h>
#include <stdarg.h>
#include <stdio.h>
#include "config.h"
#include "mem.h"
#include "fonts.h"
#include "error.h"
#include "utils.h"

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

U32 Config_Tokenize(char* string, const char** argv, const char* sep)
{
    char* s = string;
    U32 count = 0;

    if (!*s)
        return 0;

    argv[count++] = string;

    while (*s)
    {
        if (!strncmp(sep, s, strlen(sep)))
        {
            *s = '\0';
            argv[count++] = s + strlen(sep);
        }
        s++;
    }
    return count;
}

lpConfig Config_FindItem(lpConfig conf, const char* stringID)
{
    // Search the list for the given item.

    const char* argv[CONFIG_MAXDEPTH];
    U32 count, currDepth, i;
    lpConfig backConf, foundConf = NULL;
    char* tempstring = (char*)Mem_Alloc(strlen(stringID) + 1);

    strcpy(tempstring, stringID);

    count = Config_Tokenize(tempstring, argv, "::");

    // First find anything that matches the depth of the request
    // then see if the hierarchy matches the request.

    while (conf)
    {
        if (conf->depth == count - 1)
        {
            B32 wildcard = FALSE;

            if (count == 1)
            {
                const char* s;
                U32 index = 0;
                for (s = conf->itemName; *s != '\0'; s++)
                {
                    if (*s == '*')
                        break;
                    index++;
                }
                if (*s == '*')
                    wildcard = _strnicmp(argv[count - 1], conf->itemName, index) == 0;
            }

            if (wildcard || stricmp(argv[count - 1], conf->itemName) == 0)
            {
                wildcard = FALSE;

                // Look backwards down the list to check the hierarchy.
                currDepth = count - 1;
                backConf = conf;
                while (backConf)
                {
                    if (backConf->depth == currDepth - 1)
                    {
                        if (currDepth == 1)
                        {
                            const char* s;
                            U32 index = 0;
                            for (s = backConf->itemName; *s != '\0'; s++)
                            {
                                if (*s == '*')
                                    break;
                                index++;
                            }
                            if (*s == '*')
                                wildcard = _strnicmp(argv[currDepth - 1], backConf->itemName, index) == 0;
                        }

                        if (wildcard || stricmp(argv[currDepth - 1], backConf->itemName) == 0)
                        {
                            currDepth--;
                        } else
                        {
                            break;
                        }
                    }
                    backConf = backConf->linkPrev;
                }

                if (currDepth == 0)
                {
                    foundConf = conf;
                    if (!wildcard)
                        break;
                }
            }
        }
        conf = conf->linkNext;
    }
    Mem_Free(tempstring);
    return foundConf;
}

#ifdef _DEBUG
const char* Config_GetStringValueDebug(lpConfig root, const char* stringID, U32 line, const char* file)
{
    lpConfig conf;
    const char* str;

    if ((conf = Config_FindItem(root, stringID)))
    {
        if (conf->dataString)
        {
            str = Mem_AllocDebug(strlen(conf->dataString) + 1, line, file);
            strcpy(str, conf->dataString);
            return str;
        }
    }

    return NULL;
}
#else
const char* Config_GetStringValue(lpConfig root, const char* stringID)
{
    lpConfig conf;
    const char* str;

    if ((conf = Config_FindItem(root, stringID)))
    {
        if (conf->dataString)
        {
            str = Mem_Alloc(strlen(conf->dataString) + 1);
            strcpy(str, conf->dataString);
            return str;
        }
    }

    return NULL;
}
#endif // _DEBUG

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

U32 Config_GetBoolValue(lpConfig root, const char* stringID)
{
    const char* string;
    U32 res = 2;

    if ((string = Config_GetStringValue(root, stringID)))
    {
        res = Util_GetBoolFromString(string);
        Mem_Free(string);
    }

    return res;
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
