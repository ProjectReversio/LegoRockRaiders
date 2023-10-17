#pragma once

#include "common.h"
#include "container.h"

extern void Water_Debug_LogContainerMesh(B32 logWithValues);

struct Lego_Level;
extern void Water_Initialize(lpContainer root, struct Lego_Level* level);
