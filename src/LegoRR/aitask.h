#pragma once

#include "platform.h"
#include "lego_object.h"

typedef struct AITask
{
    // TODO: Implement AITask

    struct AITask* next; // Next in linked lists for `aiGlobs.AITaskUnkPtr`, `aiGlobs.AITaskDataNext`, and `LegoObject::aiTask`.
    struct AITask* nextFree; // (for listSet)
} AITask, *lpAITask;

extern void AITask_Initialize();

extern void AITask_Game_SetNoGather(B32 noGather);

extern void AITask_DoCollect(lpLegoObject liveObj, F32 param2);
