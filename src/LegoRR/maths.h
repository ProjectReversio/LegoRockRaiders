#pragma once

#include "platform.h"

#define M_PI 3.14159265358979323846f

typedef struct Point2F
{
    F32 x, y;
} Point2F, *lpPoint2F;

typedef struct Point2I
{
    S32 x, y;
} Point2I, *lpPoint2I;

typedef struct Point3F
{
    F32 x, y, z;
} Point3F, *lpPoint3F;

typedef struct Size2F
{
    F32 width, height;
} Size2F, *lpSize2F;

typedef struct Size2I
{
    S32 width, height;
} Size2I, *lpSize2I;

typedef struct Area2F
{
    F32 x, y;
    F32 width, height;
} Area2F, *lpArea2F;

typedef struct Area2I
{
    S32 x, y;
    S32 width, height;
} Area2I, *lpArea2I;

typedef struct Range2F
{
    F32 min, max;
} Range2F, *lpRange2F;

extern S16 Maths_Rand();
