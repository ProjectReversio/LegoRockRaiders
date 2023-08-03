#pragma once

#include "platform.h"
#include "fonts.h"
#include "config.h"

extern void HelpWindow_SetFont(lpFont font);
extern void HelpWindow_Initialize(lpConfig config, const char* gameName);

extern void HelpWindow_IfFlag4_AndParam_Clear1_Set2_Else_Clear3(B32 state);
