#include "routing.h"

void BezierCurve_BuildPoints(BezierCurve* curve, Point2F* p0, Point2F* p1, Point2F *p2, Point2F* p3, U32 count)
{
    // TODO: Implement BezierCurve_BuildPoints
}

void BezierCurve_Curve(Point2F* out_r, Point2F* p0, Point2F* p1, Point2F* p2, Point2F* p3, F32 t)
{
    // TODO: Implement BezierCurve_Curve
}

F32 BezierCurve_UpdateDistances(lpBezierCurve curve)
{
    // TODO: Implement BezierCurve_UpdateDistances

    return 0.0f; // TEMP: Return a dummy value.
}

Point2F* BezierCurve_Vector2DChangeLength(Point2F* ref_r, F32 newLength)
{
    // ref_point = norm(ref_point) * newLength
    // NOTE: Unlike `Maths_Vector2DSetLength`, this function modifies the input point.

    F32 m = sqrtf(ref_r->x * ref_r->x + ref_r->y * ref_r->y) / newLength;

    ref_r->x /= m;
    ref_r->y /= m;
    return ref_r;
}

F32 BezierCurve_Vector2DDistance(Point2F *a, Point2F *b)
{
    // sqrt(((a.x-b.x)*(a.x-b.x)) + ((a.y-b.y)*(a.y-b.y)))

    F32 x = a->x - b->x;
    F32 y = a->y - b->y;

    return sqrtf(x * x + y * y);
}

U32 BezierCurve_Interpolate(lpBezierCurve curve, F32 currentDist, Point2F* out_r)
{
    // TODO: Implement BezierCurve_Interpolate

    // TEMP: Return a dummy value.
    out_r->x = 0.0f;
    out_r->y = 0.0f;
    return 0;
}
