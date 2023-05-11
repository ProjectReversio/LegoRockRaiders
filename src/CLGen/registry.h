#pragma once
#include <windows.h>

// TODO: Cleanup Decompiled Code

typedef enum RegistryType
{
    REGISTRY_STRING = 0x0,
    REGISTRY_NUMBER = 0x1,
} RegistryType;

extern BOOL __cdecl Registry_SetValue_OnLocalMachine(const char *subKey, const char *valueName, RegistryType valueType, const void *in_buffer, unsigned int bufferSize);
extern BOOL __cdecl Registry_SetValue(HKEY hKey, LPCSTR subKey, LPCSTR valueName, RegistryType valueType, const void *in_buffer, unsigned int bufferSize);
extern const char *__cdecl Registry_SplitRootKey(const char *fullKey, char *out_rootKey);