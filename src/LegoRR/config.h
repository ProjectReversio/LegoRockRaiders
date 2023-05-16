#pragma once

#include "types.h"

#define CONFIG_MAXLISTS 32

#define CONFIG_SEPARATOR "::"

typedef enum Config_GlobFlags
{
    CONFIG_GLOB_FLAG_NONE = 0,
    CONFIG_GLOB_FLAG_INITIALIZED = 1,
    CONFIG_GLOB_FLAG_LOADINGCONFIG = 2,
} Config_GlobFlags;

typedef struct Config
{
    char* fileData;
    char* key;
    char* value;
    U32 depth;
    U32 itemHashCode;
    struct Config* linkNext;
    struct Config* linkPrev;

    struct Config* nextFree;
} Config, *lpConfig;

typedef struct Config_Globs
{
    lpConfig listSet[CONFIG_MAXLISTS];
    lpConfig freeList;
    U32 listCount;
    Config_GlobFlags flags;
} Config_Globs, *lpConfig_Globs;

extern Config_Globs configGlobs;

extern void Config_Initialize();
extern void Config_Shutdown();

extern const char* Config_BuildStringID(const char* s, ...);
