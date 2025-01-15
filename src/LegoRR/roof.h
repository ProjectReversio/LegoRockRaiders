#pragma once

#include "platform.h"
#include "container.h"

extern B32 Roof_Initialize(lpContainer root, U32 width, U32 height);
extern void Roof_Hide(B32 hide);

extern B32 Roof_SetTexture(const char* filename);
