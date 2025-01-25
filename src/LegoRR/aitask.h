#pragma once

#include "platform.h"

typedef struct AITask
{
    // TODO: Implement AITask

    struct AITask* next; // Next in linked lists for `aiGlobs.AITaskUnkPtr`, `aiGlobs.AITaskDataNext`, and `LegoObject::aiTask`.
    struct AITask* nextFree; // (for listSet)
} AITask, *lpAITask;

extern void AITask_Initialize();

extern void AITask_Game_SetNoGather(B32 noGather);

extern void AITask_DoCollect(struct LegoObject* liveObj, F32 param2);

extern void AITask_LiveObject_Unk_UpdateAITask_AnimationWait(struct LegoObject* liveObj);
