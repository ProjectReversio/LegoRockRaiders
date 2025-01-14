#pragma once

#include "platform.h"

extern void MapShared_GetDimensions(U32 memHandle, U32* outWidth, U32* outHeight);

extern U32 MapShared_GetBlock(U32 memHandle, S32 x, S32 y);
