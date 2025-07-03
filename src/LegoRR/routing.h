#pragma once

#include "platform.h"
#include "maths.h"

typedef enum RouteFlags
{
    ROUTE_FLAG_NONE         = 0,
    ROUTE_DIRECTION_MASK    = 0x3,
    ROUTE_FLAG_GOTOBUILDING = 0x4,
    ROUTE_FLAG_UNK_8        = 0x8,
    ROUTE_UNK_MASK_c        = 0xc,
    ROUTE_FLAG_UNK_10       = 0x10,
    ROUTE_FLAG_UNK_20       = 0x20,
    ROUTE_FLAG_RUNAWAY      = 0x40,
} RouteFlags;

typedef enum RouteAction
{
    ROUTE_ACTION_NONE        = 0,
    ROUTE_ACTION_UNK_1       = 1,
    ROUTE_ACTION_REINFORCE   = 2,
    ROUTE_ACTION_GATHERROCK  = 3,
    ROUTE_ACTION_CLEAR       = 4,
    ROUTE_ACTION_UNK_5       = 5,
    ROUTE_ACTION_REPAIRDRAIN = 6,
    ROUTE_ACTION_STORE       = 7,
    ROUTE_ACTION_DROP        = 8,
    ROUTE_ACTION_PLACE       = 9,
    ROUTE_ACTION_UNK_10      = 10,
    ROUTE_ACTION_EAT         = 11,
    ROUTE_ACTION_UNK_12      = 12,
    ROUTE_ACTION_UNK_13      = 13,
    ROUTE_ACTION_TRAIN       = 14,
    ROUTE_ACTION_UPGRADE     = 15,
    ROUTE_ACTION_UNK_16      = 16,
    ROUTE_ACTION_UNK_17      = 17,
    ROUTE_ACTION_RECHARGE    = 18,
    ROUTE_ACTION_DOCK        = 19,
    ROUTE_ACTION_ATTACK      = 20,
    ROUTE_ACTION_21          = 21,
} RouteAction;

typedef struct BezierCurve
{
    U32 count;
    Point2F points[50];
    F32 distances[50];
} BezierCurve, *lpBezierCurve;

typedef struct RoutingBlock
{
    Point2I blockPos;
    Point2F blockOffset; // In range of [0.0, 1.0], where 0.5 is the center of the block.
    RouteFlags flagsByte;
    RouteAction actionByte;

    // TODO: Implement RoutingBlock
} RoutingBlock, *lpRoutingBlock;

extern U32 BezierCurve_Interpolate(lpBezierCurve curve, F32 currentDist, Point2F* out_r);
