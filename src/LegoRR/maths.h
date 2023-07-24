#pragma once

#include <math.h>
#include "platform.h"

#define M_PI 3.14159265358979323846f

typedef F32 Matrix4F[4][4];
typedef F32 Matrix3F[3][3];

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

typedef struct Point4F
{
    F32 x, y, z, w;
} Point4F, *lpPoint4F;

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

#define Maths_Vector3DLength(v) Maths_Vector3DModulus((v))

extern lpPoint3F Maths_PlaneNormal(lpPoint3F n, lpPoint3F p1, lpPoint3F p2, lpPoint3F p3);

inline lpPoint3F Maths_Vector3DCrossProduct(lpPoint3F r, lpPoint3F a, lpPoint3F b)
{
    r->x = a->y * b->z - a->z * b->y;
    r->y = a->z * b->x - a->x * b->z;
    r->z = a->x * b->y - a->y * b->x;
    return r;
}

inline lpPoint3F Maths_Vector3DSubtract(lpPoint3F r, lpPoint3F a, lpPoint3F b)
{
    r->x = a->x - b->x;
    r->y = a->y - b->y;
    r->z = a->z - b->z;
    return r;
}

inline lpPoint3F Maths_Vector3DAdd(lpPoint3F r, lpPoint3F a, lpPoint3F b)
{
    r->x = a->x + b->x;
    r->y = a->y + b->y;
    r->z = a->z + b->z;
    return r;
}

inline F32 Maths_Vector3DModulus(lpPoint3F v)
{
    return (F32)sqrt(v->x * v->x + v->y * v->y + v->z * v->z);
}

inline lpPoint3F Maths_Vector3DScale(lpPoint3F r, lpPoint3F a, F32 f)
{
    r->x = a->x * f;
    r->y = a->y * f;
    r->z = a->z * f;
    return r;
}

inline lpPoint3F Maths_Vector3DNormalize(lpPoint3F r)
{
    Maths_Vector3DScale(r, r, 1.0f / Maths_Vector3DLength(r));
    return r;
}
