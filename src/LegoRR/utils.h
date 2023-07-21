#pragma once

#include "platform.h"

#define UTIL_DEFSTRINGLEN   512

extern const char* Util_StrIStr(const char* str1, const char* str2);
extern U32 Util_GetBoolFromString(const char* string);
extern U32 Util_Tokenize(const char* string, char** argv, const char* sep);

#ifdef _DEBUG
extern char* Util_StrCpyDebug(const char* string, const char* file, U32 line);
#define Util_StrCpy(s) Util_StrCpyDebug((s), __FILE__, __LINE__)
#else
extern char* Util_StrCpy(const char* string);
#endif

extern U32 Util_HashString(const char* str, B32 bIgnoreBlanks, B32 upperCase);
