#pragma once

#include "types.h"

typedef struct Area2F
{
    F32 x, y;
    F32 width, height;
} Area2F, *lpArea2F;

typedef struct Point2F
{
    F32 x, y;
} Point2F, *lpPoint2F;

typedef struct Point2I
{
    S32 x, y;
} Point2I, *lpPoint2I;

typedef struct Size2F
{
    F32 width, height;
} Size2F, *lpSize2F;
