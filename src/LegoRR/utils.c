#include <string.h>
#include "utils.h"

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
