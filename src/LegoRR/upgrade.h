#pragma once

#include "platform.h"

typedef struct Upgrade_PartModel
{
    void* temp;
    // TODO: Implement Upgrade_PartModel
} Upgrade_PartModel, *lpUpgrade_PartModel;

extern F32 Upgrade_Part_MoveAnimation(lpUpgrade_PartModel upgradePart, F32 elapsed, U32 unkFrameNo);
