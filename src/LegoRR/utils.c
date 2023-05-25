#include <string.h>
#include "utils.h"
#include "mem.h"

const char* Util_StrIStr(const char* str1, const char* str2)
{
    U32 len1, len2;
    U32 i;
    char *str;

    len1 = strlen(str1);
    len2 = strlen(str2);

    if (!len1 || !len2)
        return NULL;

    for (i = 0; i < len1; i++)
    {
        str = str1 + i;
        if (!strnicmp(str, str2, len2))
        {
            return str;
        }
    }

    return NULL;
}

U32 Util_GetBoolFromString(const char* string)
{
    if (stricmp(string, "YES") == 0)
        return TRUE;
    if (stricmp(string, "TRUE") == 0)
        return TRUE;
    if (stricmp(string, "ON") == 0)
        return TRUE;
    if (stricmp(string, "NO") == 0)
        return FALSE;
    if (stricmp(string, "FALSE") == 0)
        return FALSE;
    if (stricmp(string, "OFF") == 0)
        return FALSE;

    return 2;
}

U32 Util_Tokenize(const char* string, char** argv, const char* sep)
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

#ifdef _DEBUG
char* Util_StrCpyDebug(const char* string, const char* file, U32 line)
{
    char* newString = Mem_AllocDebug(strlen(string) + 1, line, file);
    strcpy(newString, string);
    return newString;
}
#else
char* Util_StrCpy(const char* string)
{
    char* newString = Mem_Alloc(strlen(string) + 1);
    strcpy(newString, string);
    return newString;
}
#endif
