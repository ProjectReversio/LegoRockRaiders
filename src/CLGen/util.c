#include "util.h"
#include <string.h>

// TODO: Cleanup Decompiled Code

int __cdecl Util_StringSplit(char *input, char **out_parts, const char *delimiter)
{
    char *input_; // esi
    size_t delimiterLength; // ebx
    int v6; // ebp
    char **v7; // edi

    input_ = input;
    delimiterLength = strlen(delimiter);
    if ( !*input )
        return 0;
    v6 = 1;
    *out_parts = input;
    if ( !*input )
        return v6;
    v7 = out_parts + 1;
    do
    {
        if ( !strncmp(delimiter, input_, delimiterLength) )
        {
            *input_ = 0;
            *v7 = &input_[delimiterLength];
            ++v6;
            ++v7;
        }
    }
    while ( *++input_ );
    return v6;
}
