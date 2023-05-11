#include "registry.h"

// TODO: Cleanup Decompiled Code

BOOL __cdecl Registry_SetValue_OnLocalMachine(const char *subKey, const char *valueName, RegistryType valueType, const void *in_buffer, unsigned int bufferSize)
{
    return Registry_SetValue(HKEY_LOCAL_MACHINE, subKey, valueName, valueType, in_buffer, bufferSize);
}

BOOL __cdecl Registry_SetValue(HKEY hKey, LPCSTR subKey, LPCSTR valueName, RegistryType valueType, const void *in_buffer, unsigned int bufferSize)
{
    const char *v6; // esi
    BOOL result; // eax
    BOOL v8; // esi
    CHAR SubKey[100]; // [esp+4h] [ebp-64h] BYREF

    v6 = Registry_SplitRootKey(subKey, SubKey);
    if ( SubKey[0] )
    {
        RegCreateKeyA(hKey, SubKey, (PHKEY)&subKey);
        result = (BOOL)subKey;
        if ( subKey )
        {
            v8 = Registry_SetValue((HKEY)subKey, v6, valueName, valueType, in_buffer, bufferSize);
            RegCloseKey((HKEY)subKey);
            result = v8;
        }
    }
    else if ( valueType )
    {
        if ( valueType == REGISTRY_NUMBER )
        {
            RegSetValueExA(hKey, valueName, 0, REG_DWORD, (const BYTE *)in_buffer, bufferSize);
            result = 1;
        }
        else
        {
            result = 0;
        }
    }
    else
    {
        RegSetValueExA(hKey, valueName, 0, REG_SZ, (const BYTE *)in_buffer, bufferSize);
        result = 1;
    }
    return result;
}

const char *__cdecl Registry_SplitRootKey(const char *fullKey, char *out_rootKey)
{
    char *v2; // edx
    const char *result; // eax
    char i; // cl

    v2 = out_rootKey;
    result = fullKey;
    *out_rootKey = 0;
    for ( i = *fullKey; *result; i = *result )
    {
        if ( i == '\\' )
            break;
        ++result;
        *v2++ = i;
    }
    if ( *result == '\\' )
        ++result;
    *v2 = 0;
    return result;
}