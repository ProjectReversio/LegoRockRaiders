#pragma once

#include "platform.h"
#include "fonts.h"
#include "config.h"

extern void Interface_Initialize(U32 x, U32 y, lpFont font);

extern void Interface_InitBuildItems();
extern void Interface_LoadMenuItems(lpConfig config, const char* gameName);
extern void Interface_LoadItemPanels(lpConfig config, const char* gameName);
extern void Interface_LoadPlusMinusImages(const char* plusName, const char* minusName);
