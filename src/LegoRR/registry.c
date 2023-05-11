#include "registry.h"

B32 Registry_GetValue(const char* path, const char* key, RegistryType dataType, char* out_data, U32 dataSize)
{
    return Registry_GetValue_Recursive(HKEY_LOCAL_MACHINE, path, key, dataType, out_data, dataSize);
}

B32 Registry_GetValue_Recursive(HKEY parent, const char* path, const char* key, RegistryType dataType, char* out_data, U32 dataSize)
{
    char str[100];
    const char* currentKey = Registry_GetKeyFromPath(path, str);

    if (str[0] != '\0')
    {
        // Open the key and recurse
        HKEY theKey;
        RegCreateKeyA(parent, str, &theKey);
        if (theKey == NULL)
            return FALSE;

        B32 result = Registry_GetValue_Recursive(theKey, currentKey, key, dataType, out_data, dataSize);
        RegCloseKey(theKey);
        return result;
    }

    U32 type;
    if (dataType == REGISTRY_STRING_VALUE || dataType == REGISTRY_DWORD_VALUE)
        return RegQueryValueExA(parent, key, NULL, (LPDWORD)&type, out_data, (LPDWORD)&dataSize) == 0;

    return 0;
}

const char* Registry_GetKeyFromPath(const char* path, char* out_str)
{
    // TODO: Cleanup Decompiled Code

    char cVar1;

    *out_str = '\0';
    cVar1 = *path;
    while ((cVar1 != '\0' && (cVar1 != '\\'))) {
        path = path + 1;
        *out_str = cVar1;
        out_str = out_str + 1;
        cVar1 = *path;
    }
    if (*path == '\\') {
        path = path + 1;
    }
    *out_str = '\0';
    return path;
}
