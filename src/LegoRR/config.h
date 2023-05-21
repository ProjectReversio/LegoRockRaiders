#pragma once

#include "types.h"

#define CONFIG_MAXLISTS 32
#define CONFIG_MAXDEPTH 100

#define CONFIG_SEPARATOR "::"

#ifdef _DEBUG
#define Config_CheckInit() \
    if (!(configGlobs.flags & CONFIG_GLOB_FLAG_INITIALIZED)) \
        Error_Fatal(TRUE, "Error: Config_Intitialise() Has Not Been Called");
#else
#define Config_CheckInit()
#endif

typedef enum Config_GlobFlags
{
    CONFIG_GLOB_FLAG_NONE = 0,
    CONFIG_GLOB_FLAG_INITIALIZED = 1,
    CONFIG_GLOB_FLAG_LOADINGCONFIG = 2,
} Config_GlobFlags;

typedef struct Config
{
    U8* fileData;
    U8* itemName;
    U8* dataString;
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

#ifdef _DEBUG
    lpConfig debugLastFind;
#endif

} Config_Globs, *lpConfig_Globs;

extern Config_Globs configGlobs;

extern void Config_Initialize();
extern void Config_Shutdown();

extern U32 Config_Tokenize(char* string, const char** argv, const char* sep);

extern lpConfig Config_FindItem(lpConfig conf, const char* stringID);

#ifdef _DEBUG
extern const char* Config_GetStringValueDebug(lpConfig root, const char* stringID, U32 line, const char* file);
#define Config_GetStringValue(c,s) Config_GetStringValueDebug((c),(s),__LINE__,__FILE__)
#else
extern const char* Config_GetStringValue(lpConfig root, const char* stringID);
#endif

extern const char* Config_BuildStringID(const char* s, ...);
extern U32 Config_GetBoolValue(lpConfig root, const char* stringID);

#ifdef _DEBUG
extern const char* Config_Debug_GetLastFind();
#endif // _DEBUG

extern lpConfig Config_Load(const char* filename);

extern lpConfig Config_Create(lpConfig prev);
extern void Config_Remove(lpConfig dead);

extern void Config_AddList();
