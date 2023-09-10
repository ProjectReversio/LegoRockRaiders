#pragma once

#include "platform.h"

typedef enum Direction
{
    DIRECTION_UP = 0,
    DIRECTION_RIGHT = 1,
    DIRECTION_DOWN = 2,
    DIRECTION_LEFT = 3,
    DIRECTION_COUNT = 4,
} Direction;

typedef struct Coord2U
{
    U16 x;
    U16 y;
} Coord2U, *lpCoord2U;
