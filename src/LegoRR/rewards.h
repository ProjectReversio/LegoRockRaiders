#pragma once

#include "platform.h"
#include "config.h"
#include "images.h"

typedef struct RewardLevel
{
    B32 Enabled;
    B32 saveHasCapture;
    Image saveCaptureImage;

    // TODO: Implement RewardLevel

} RewardLevel, *lpRewardLevel;

extern B32 Reward_LoadGraphics(lpConfig config, const char* gameName);
extern B32 Reward_Initialize();
