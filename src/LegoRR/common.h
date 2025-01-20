#pragma once

#include "platform.h"
#include "maths.h"

typedef enum Direction
{
    DIRECTION_UP = 0,
    DIRECTION_RIGHT = 1,
    DIRECTION_DOWN = 2,
    DIRECTION_LEFT = 3,
    DIRECTION_COUNT = 4,
} Direction;

#define DIRECTION_OPPOSITE(dir) ((dir + 2) % DIRECTION_COUNT)

typedef struct Coord2U
{
    U16 x;
    U16 y;
} Coord2U, *lpCoord2U;

typedef struct Vertex
{
    Point3F position; // Homogeneous coordinates
    Point3F normal; // Normal
    F32 tu; // Texture coordinates
    F32 tv; // Texture coordinates
    U32 color;
} Vertex, *lpVertex;
