#include "aitask.h"

#include "lego.h"
#include "lego_object.h"
#include "level.h"
#include "map3d.h"
#include "mem.h"
#include "message.h"

AITask_Globs aiGlobs = {};

void AITask_Initialize()
{
    memset(aiGlobs.listSet, 0, sizeof(aiGlobs.listSet));

    aiGlobs.aitaskName[0] = "AITask_Type_Goto";
    aiGlobs.aitaskName[1] = "AITask_Type_Follow";
    aiGlobs.aitaskName[2] = "AITask_Type_FollowAttack";
    aiGlobs.aitaskName[3] = "AITask_Type_Collect";
    aiGlobs.aitaskName[4] = "AITask_Type_Gather";
    aiGlobs.aitaskName[5] = "AITask_Type_Deposit";
    aiGlobs.aitaskName[6] = "AITask_Type_Dump";
    aiGlobs.aitaskName[7] = "AITask_Type_Request";
    aiGlobs.aitaskName[8] = "AITask_Type_Dig";
    aiGlobs.aitaskName[9] = "AITask_Type_Dynamite";
    aiGlobs.aitaskName[10] = "AITask_Type_Repair";
    aiGlobs.aitaskName[11] = "AITask_Type_Reinforce";
    aiGlobs.aitaskName[12] = "AITask_Type_Clear";
    aiGlobs.aitaskName[13] = "AITask_Type_Wait";
    aiGlobs.aitaskName[14] = "AITask_Type_AnimationWait";
    aiGlobs.aitaskName[15] = "AITask_Type_ElecFence";
    aiGlobs.aitaskName[16] = "AITask_Type_Eat";
    aiGlobs.aitaskName[17] = "AITask_Type_GotoEat";
    aiGlobs.aitaskName[18] = "AITask_Type_FindDriver";
    aiGlobs.aitaskName[19] = "AITask_Type_GetTool";
    aiGlobs.aitaskName[20] = "AITask_Type_BirdScarer";
    aiGlobs.aitaskName[21] = "AITask_Type_Upgrade";
    aiGlobs.aitaskName[22] = "AITask_Type_BuildPath";
    aiGlobs.aitaskName[23] = "AITask_Type_Train";
    aiGlobs.aitaskName[24] = "AITask_Type_Depart";
    aiGlobs.aitaskName[25] = "AITask_Type_AttackPath";
    aiGlobs.aitaskName[26] = "AITask_Type_AttackRockMonster";
    aiGlobs.aitaskName[27] = "AITask_Type_Recharge";
    aiGlobs.aitaskName[28] = "AITask_Type_Dock";
    aiGlobs.aitaskName[29] = "AITask_Type_AttackObject";
    aiGlobs.aitaskName[30] = "AITask_Type_FindLoad";

    aiGlobs.priorityName[0] = "AI_Priority_Crystal";
    aiGlobs.priorityName[1] = "AI_Priority_Ore";
    aiGlobs.priorityName[2] = "AI_Priority_DefaultCollect";
    aiGlobs.priorityName[3] = "AI_Priority_Destruction";
    aiGlobs.priorityName[4] = "AI_Priority_Construction";
    aiGlobs.priorityName[5] = "AI_Priority_Request";
    aiGlobs.priorityName[6] = "AI_Priority_Reinforce";
    aiGlobs.priorityName[7] = "AI_Priority_Repair";
    aiGlobs.priorityName[8] = "AI_Priority_Clearing";
    aiGlobs.priorityName[9] = "AI_Priority_Storage";
    aiGlobs.priorityName[10] = "AI_Priority_Refining";
    aiGlobs.priorityName[11] = "AI_Priority_HealthLow";
    aiGlobs.priorityName[12] = "AI_Priority_GetIn";
    aiGlobs.priorityName[13] = "AI_Priority_Upgrade";
    aiGlobs.priorityName[14] = "AI_Priority_BuildPath";
    aiGlobs.priorityName[15] = "AI_Priority_AttackRockMonster";
    aiGlobs.priorityName[16] = "AI_Priority_Barrier";
    aiGlobs.priorityName[17] = "AI_Priority_FindLoad";
    aiGlobs.priorityName[18] = "AI_Priority_Recharge";
    aiGlobs.priorityName[19] = "AI_Priority_UpgradeBuilding";
    aiGlobs.priorityName[20] = "AI_Priority_Gather";
    aiGlobs.priorityName[21] = "AI_Priority_Steal";
    aiGlobs.priorityName[22] = "AI_Priority_Punch";
    aiGlobs.priorityName[23] = "AI_Priority_Depart";
    aiGlobs.priorityName[24] = "AI_Priority_AttackPath";
    aiGlobs.priorityName[25] = "AI_Priority_AttackObject";
    aiGlobs.priorityName[26] = "AI_Priority_Congregate";

    memset(aiGlobs.priorityValues, 49, sizeof(aiGlobs.priorityValues));

    aiGlobs.priorityValues[0] = 55;
    // ...
    aiGlobs.priorityValues[3] = 40;
    aiGlobs.priorityValues[4] = 60;
    aiGlobs.priorityValues[5] = 20;
    aiGlobs.priorityValues[6] = 70;
    // ...
    aiGlobs.priorityValues[8] = 20;
    aiGlobs.priorityValues[9] = 5;
    aiGlobs.priorityValues[10] = 15;
    // ...
    aiGlobs.priorityValues[15] = 56;
    // ...
    aiGlobs.priorityValues[19] = 99;
    aiGlobs.priorityValues[20] = 49;
    aiGlobs.priorityValues[21] = 90;
    aiGlobs.priorityValues[22] = 49;
    aiGlobs.priorityValues[23] = 0;
    aiGlobs.priorityValues[24] = 20;
    aiGlobs.priorityValues[25] = 30;
    aiGlobs.priorityValues[26] = 99;
    
    aiGlobs.freeList = NULL;
    aiGlobs.listCount = 0;
    aiGlobs.flags = AITASK_GLOB_FLAG_INITIALIZED;
}

void AITask_AddList()
{
    // TODO: Clean up this code

    U32 size = 1 << ((U8)aiGlobs.listCount & 0x1F);
    aiGlobs.listSet[aiGlobs.listCount] = Mem_Alloc(size * sizeof(AITask));
    AITask* list = aiGlobs.listSet[aiGlobs.listCount];
    if (list != NULL)
    {
        aiGlobs.listCount++;
        if (size > 1)
        {
            S32 i = size - 1;
            AITask* task = list;
            do
            {
                task->nextFree = task + 1;
                i--;
                task++;
            } while (i != 0);
        }

        list[size - 1].nextFree = aiGlobs.freeList;
        aiGlobs.freeList = list;
    }
}

lpAITask AITask_Create(AITask_Type taskType)
{
    if (aiGlobs.freeList == NULL)
        AITask_AddList();

    // TODO: Verify if this code is correct

    lpAITask task = aiGlobs.freeList;
    aiGlobs.freeList = aiGlobs.freeList->nextFree;
    memset(task, 0, sizeof(AITask));

    task->nextFree = task;
    task->creationTime = Main_GetTime();
    task->taskType = taskType;

    return task;
}

void AITask_Game_SetNoGather(B32 noGather)
{
    // TODO: Implement AITask_Game_SetNoGather
}

void AITask_DoCollect(lpLegoObject liveObj, F32 param2)
{
    // TODO: Implement AITask_DoCollect
}

void AITask_LiveObject_Unk_UpdateAITask_AnimationWait(lpLegoObject liveObj)
{
    // TODO: Implement AITask_LiveObject_Unk_UpdateAITask_AnimationWait
}

B32 AITask_RemoveAttackPathReferences(Point2I* blockPos)
{
    // TODO: Implement AITask_RemoveAttackPathReferences

    return FALSE;
}

void AITask_DoClear_AtPosition(Point2I* blockPos, Message_Type completeAction)
{
    lpAITask task = AITask_Create(AITask_Type_Clear);
    task->blockPos.x = blockPos->x;
    task->blockPos.y = blockPos->y;
    task->completeAction = completeAction;
    AITask_InitTask_1(task, AI_Priority_Clearing);
}

void AITask_DoDig_AtBlockPos(Point2I* blockPos, B32 param2, B32 param3)
{
    if (Level_Block_IsImmovable(blockPos))
        return;

    if (Level_Block_IsLava(blockPos))
        return;

    if (!Level_Block_IsSurveyed(blockPos->x, blockPos->y))
        return;

    lpAITask task = AITask_Create(AITask_Type_Dig);
    task->blockPos.x = blockPos->x;
    task->blockPos.y = blockPos->y;

    if (param2)
        task->flags |= AITASK_FLAG_DIGCONNECTION;

    AITask_UnkInitRouting_FUN_00402530(task, TRUE);
    AITask_InitTask_1(task, AI_Priority_Destruction);
    task->flags |= AITASK_FLAG_NOTIMELIMIT;

    if (param3)
        task->flags |= AITASK_FLAG_MANUALYSELECTED;

    if (!param2)
    {
        lpLego_Level level = Lego_GetLevel();
        Map3D_SetBlockHighlight(level->map, blockPos->x, blockPos->y, WALLHIGHLIGHT_DIG);
        level->blocks[blockPos->y * level->width + blockPos->x].flags1 |= BLOCK1_DIGREQUEST;
    }
}

void AITask_UnkInitRouting_FUN_00402530(lpAITask aiTask, B32 dropCarried)
{
    // TODO: Implement AITask_UnkInitRouting_FUN_00402530
}

lpAITask AITask_InitTask_1(lpAITask aiTask, AI_Priority priorityType)
{
    // Prepend to linked list.
    aiTask->next = aiGlobs.pendingTaskList;
    aiGlobs.pendingTaskList = aiTask;
    aiTask->time = 1500.0f;
    aiTask->flags &= ~AITASK_FLAG_PERFORMING;
    aiTask->flags |= AITASK_FLAG_REALLOCATE;
    aiTask->priorityValue = aiGlobs.priorityValues[priorityType];
    aiTask->priorityType = priorityType;

    return aiTask;
}
