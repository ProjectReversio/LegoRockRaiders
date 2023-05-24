#pragma once

#include "types.h"
#include "common.h"
#include "maths.h"
#include "fonts.h"

extern void Loader_Initialize(const char* loadScreenName, const char* shutdownScreenName, lpFont font5HI, const char* profileName, Direction progressDirection, const char* progressBarName, Area2F* progressWindow, const char* loadingText);
extern void Loader_Display_Loading_Bar(const char* section);
