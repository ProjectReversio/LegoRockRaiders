#pragma once

#include "types.h"

#define MAX_WADS 10

extern S32 Wad_Load(char* fName);
extern void Wad_Close(S32 wadNo);
