#pragma once

#include "platform.h"
#include "maths.h"
#include "message.h"

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

extern B32 AITask_RemoveAttackPathReferences(Point2I* blockPos);

extern void AITask_DoClear_AtPosition(Point2I* blockPos, Message_Type completeAction);

extern void AITask_DoDig_AtBlockPos(Point2I* blockPos, B32 param2, B32 param3);
