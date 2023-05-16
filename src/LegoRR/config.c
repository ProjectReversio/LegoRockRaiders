#include <string.h>
#include <stdarg.h>
#include "config.h"
#include "mem.h"

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
