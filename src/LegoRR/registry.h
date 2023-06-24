#pragma once

#include <windows.h>
#include "platform.h"

typedef enum RegistryType
{
    REGISTRY_STRING_VALUE = 0,
    REGISTRY_DWORD_VALUE = 1,
} RegistryType;

extern B32 Registry_GetValue(const char* path, const char* key, RegistryType dataType, char* out_data, U32 dataSize);
extern B32 Registry_GetValue_Recursive(HKEY parent, const char* path, const char* key, RegistryType dataType, char* out_data, U32 dataSize);
extern const char* Registry_GetKeyFromPath(const char* path, char* out_str);
