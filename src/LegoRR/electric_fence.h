#pragma once

#include "common.h"
#include "lego_object.h"

struct Lego_Level;

typedef struct ElectricFence_Block
{
    lpLegoObject attachedObject;
    Point2I blockPos;
    F32 timer;
    struct ElectricFence_Block* nextFree; // (for listSet)
} ElectricFence_Block, *lpElectricFence_Block;

extern lpElectricFence_Block ElectricFence_CreateFence(lpLegoObject liveObj);

extern void ElectricFence_Restart(struct Lego_Level* level);
