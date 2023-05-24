#pragma once

#include "types.h"

extern const char* Util_StrIStr(const char* str1, const char* str2);
extern U32 Util_GetBoolFromString(const char* string);
extern U32 Util_Tokenize(const char* string, char** argv, const char* sep);
