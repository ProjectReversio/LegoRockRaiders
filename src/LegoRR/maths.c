#include <stdlib.h>
#include "maths.h"

S16 Maths_Rand()
{
    return rand();
}

F32 Maths_RandRange(F32 low, F32 high)
{
    F32 val = (F32) Maths_Rand();
    val /= RAND_MAX;

    val *= (high - low);
    return low + val;
}

lpPoint3F Maths_PlaneNormal(lpPoint3F n, lpPoint3F p1, lpPoint3F p2, lpPoint3F p3)
{
    Point3F v1, v2;

    Maths_Vector3DSubtract(&v1, p2, p1);
    Maths_Vector3DSubtract(&v2, p3, p2);
    Maths_Vector3DCrossProduct(n, &v1, &v2);
    return Maths_Vector3DNormalize(n);
}
