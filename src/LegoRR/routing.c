#include "routing.h"

void BezierCurve_BuildPoints(BezierCurve* curve, Point2F* p0, Point2F* p1, Point2F *p2, Point2F* p3, U32 count)
{
    // the original code checks max - 1, but it should probably be max. Oh well.
    if (count > BEZIERCURVE_MAX_POINTS - 1)
        count = BEZIERCURVE_MAX_POINTS;

    curve->count = count;
    curve->points[0].x = p0->x;
    curve->points[0].y = p0->y;

    for (U32 index = 1; index < count; index++)
    {
        BezierCurve_Curve(&curve->points[index], p0, p1, p2, p3, (F32)index * (1.0f / (F32)(count - 1)));
    }
}

void BezierCurve_Curve(Point2F* out_r, Point2F* p0, Point2F* p1, Point2F* p2, Point2F* p3, F32 t)
{
    // TODO: Cleanup this decompiled code.

    float xVar0 = p1->x * 3.0f + p0->x;
    float t2 = t * t;
    float xVar1 = (xVar0 - p0->x) * 3.0f;
    float fVar1 = ((p2->x - p3->x * 3.0f) - xVar0) * 3.0f - xVar1;
    float p3__y = p3->y;
    float yVar0 = p1->y * 3.0f + p0->y;
    float p2__y = p2->y;

    float yVar1 = (yVar0 - p0->y) * 3.0f;
    float fVar2 = ((p2__y - p3__y * 3.0f) - yVar0) * 3.0f - yVar1;

    out_r->x = xVar1 * t + fVar1 * t2 + (((p2->x - p0->x) - xVar1) - fVar1) * t2 * t + p0->x;
    out_r->y = fVar2 * t2 + yVar1 * t + (((p2->y - p0->y) - yVar1) - fVar2) * t2 * t + p0->y;
}

F32 BezierCurve_UpdateDistances(lpBezierCurve curve)
{
    F32 totalDist = 0.0f;
    curve->distances[0] = 0.0f;

    for (U32 index = 1; index < curve->count; index++)
    {
        // Calculate distance from previous point to current point.
        F32 dist = BezierCurve_Vector2DDistance(&curve->points[index - 1], &curve->points[index]);
        totalDist += dist;
        curve->distances[index] = dist;
    }

    return totalDist;
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
    F32 endDist = 0.0f;
    U32 index = 1;
    for (; index < curve->count; index++)
    {
        endDist += curve->distances[index];
        if (endDist > currentDist)
            break;
    }

    if (index < curve->count)
    {
        // This assignment has no effect
        out_r->x = curve->points[index - 1].x;
        out_r->y = curve->points[index - 1].y;

        F32 currY = curve->points[index].y;
        F32 prevY = curve->points[index - 1].y;
        F32 currX = curve->points[index].x;
        F32 prevX = curve->points[index - 1].x;

        F32 delta = (currentDist - (endDist - curve->distances[index])) / curve->distances[index];

        out_r->x = (currX - prevX) * delta + prevX;
        out_r->y = (currY - prevY) * delta + prevY;

        return index;
    }

    out_r->x = curve->points[index - 1].x;
    out_r->y = curve->points[index - 1].y;
    return index;
}
