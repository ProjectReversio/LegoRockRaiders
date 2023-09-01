#pragma once

#include "platform.h"

struct Lego_Level;

extern void NERPs_SetHasNextButton(B32 hasNextButton);

extern B32 NERPsFile_LoadScriptFile(const char* filename);
extern B32 NERPsFile_LoadMessageFile(const char* filename);

extern void NERPs_InitBlockPointersTable(struct Lego_Level* level);
