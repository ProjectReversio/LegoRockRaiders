#pragma once

#include "types.h"

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

extern void Config_Initialize();
extern void Config_Shutdown();
