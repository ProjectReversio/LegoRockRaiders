#pragma once

#include "platform.h"
#include "fonts.h"
#include "config.h"
#include "lego_object.h"

extern void HelpWindow_SetFont(lpFont font);
extern void HelpWindow_Initialize(lpConfig config, const char* gameName);

extern void HelpWindow_IfFlag4_AndParam_Clear1_Set2_Else_Clear3(B32 state);

extern void HelpWindow_RecallDependencies(LegoObject_Type objType, LegoObject_ID objID, U32 objLevel, B32 noHelpWindow);
