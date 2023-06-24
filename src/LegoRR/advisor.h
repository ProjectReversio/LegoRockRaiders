#pragma once

#include "platform.h"
#include "lego_camera.h"
#include "viewport.h"
#include "config.h"

extern void Advisor_Initialize(const char* gameName, lpLegoCamera cameraMain, lpViewport viewMain);
extern void Advisor_InitViewport(F32 param_unused);
extern void Advisor_LoadAnims(lpConfig config, const char* gameName);
extern void Advisor_LoadPositions(lpConfig config, const char* gameName, S32 screenWidth, S32 screenHeight);
