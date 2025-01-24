#include <stdlib.h>
#include "maths.h"

#include <d3drmdef.h>

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

B32 Maths_PointInsidePoly(lpPoint2F point, lpPoint2F fromList, lpPoint2F toList, U32 count)
{
    // Count the edges to the right of the point and return TRUE if it is odd...

    U32 loop, rightCount = 0;
    F32 inter, alt, slope, line;

    for (loop = 0; loop < count; loop++)
    {
        if ((fromList[loop].y <= point->y && toList[loop].y >= point->y) ||
            (fromList[loop].y >= point->y && toList[loop].y <= point->y))
        {
            alt = toList[loop].y - fromList[loop].y;
            slope = toList[loop].x - fromList[loop].x;
            line = point->y - fromList[loop].y;

            inter = fromList[loop].x + ((line / alt) * slope);

            if (inter > point->x)
                rightCount++;
        }
    }

    return (rightCount & 0x00000001);
}

B32 Maths_RayPlaneIntersection(lpPoint3F endPoint, lpPoint3F rayOrigin, lpPoint3F ray, lpPoint3F planePoint, lpPoint3F planeNormal)
{
    Point3F r = *ray, n = *planeNormal;
    F32 dist;

    Maths_Vector3DNormalize(&r);
    Maths_Vector3DNormalize(&n);

    if (Maths_RayPlaneDistance(&dist, rayOrigin, &r, planePoint, &n))
    {
        Maths_RayEndPoint(endPoint, rayOrigin, &r, dist);
        return TRUE;
    }

    return FALSE;
}

B32 Maths_RayPlaneDistance(F32* dist, lpPoint3F rayOrigin, lpPoint3F ray, lpPoint3F planePoint, lpPoint3F planeNormal)
{
    Point3F point;
    F32 t;

    if ((t = Maths_Vector3DDotProduct(planeNormal, ray)) != 0.0f)
    {
        // Translate the point in the plane so that the ray would originate from (0, 0, 0).
        Maths_Vector3DSubtract(&point, planePoint, rayOrigin);

        *dist = Maths_Vector3DDotProduct(planeNormal, &point) / t;

        return TRUE;
    }

    return FALSE;
}

void Maths_RayEndPoint(lpPoint3F endPoint, lpPoint3F rayOrigin, lpPoint3F ray, F32 dist)
{
    Point3F temp;

    Maths_Vector3DScale(&temp, ray, dist);
    Maths_Vector3DAdd(endPoint, rayOrigin, &temp);
}

lpPoint3F Maths_Vector3DRotate(lpPoint3F r, lpPoint3F v, lpPoint3F axis, F32 theta)
{
    return (lpPoint3F) D3DRMVectorRotate((LPD3DVECTOR) r, (LPD3DVECTOR) v, (LPD3DVECTOR) axis, theta);
}
