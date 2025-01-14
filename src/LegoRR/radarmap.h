#pragma once

#include "draw.h"
#include "map3d.h"
#include "platform.h"

typedef struct RadarMap
{
    lpMap3D map;
    Area2F screenRect; // Passed value of (16.0f, 13.0f, 151.0f, 151.0f).
    F32 zoom; // Always assigned a value in the range [10,20], with 15 being the default. Higher values being more zoomed-in.
    F32 blockSize;
    Point2F centerPos;
    Area2F worldRect;
    Draw_Rect* drawRectList;
    U32 drawRectCount;
} RadarMap, *lpRadarMap;

extern void RadarMap_Initialize();

extern lpRadarMap RadarMap_Create(lpMap3D map, lpArea2F radarViewRect, F32 tvTiltOrZoom);
