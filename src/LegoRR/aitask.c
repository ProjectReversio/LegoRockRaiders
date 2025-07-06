#include "aitask.h"

#include <stdio.h>

#include "lego.h"
#include "lego_object.h"
#include "level.h"
#include "map3d.h"
#include "mem.h"
#include "message.h"
#include "bubble.h"

AITask_Globs aiGlobs = { NULL };

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

// levelCleanup is only true when called by `AITask_Callback_Remove`.
void AITask_Remove(lpAITask aiTask, B32 levelCleanup)
{
    if (!levelCleanup)
    {
        // Extra removal behaviour that should NOT be called during level cleanup.
        AITask_RemoveGetToolReferences(aiTask);
    }

    if (aiTask->unitList != NULL &&
        (aiTask->flags & AITASK_FLAG_CLONED) == AITASK_FLAG_NONE)
    {
        Mem_Free(aiTask->unitList);
    }

    aiTask->nextFree = aiGlobs.freeList;
    aiGlobs.freeList = aiTask;
}

lpAITask AITask_Clone(lpAITask aiTask)
{
    AITask* newAITask = AITask_Create(aiTask->taskType);
    AITask* nextFree = newAITask->nextFree;

    memcpy(newAITask, aiTask, sizeof(AITask));

    newAITask->nextFree = nextFree;
    newAITask->flags |= AITASK_FLAG_CLONED;
    newAITask->next = NULL;

    return newAITask;
}

// Returns the last task in the object's linked list of tasks.
// Or null if the object has no tasks.
lpAITask AITask_GetObjectTaskListTail(struct LegoObject* liveObj)
{
    lpAITask task = liveObj->aiTask;
    if (task != NULL)
    {
        for (lpAITask taskNext = task->next; taskNext != NULL; taskNext = taskNext->next)
        {
            task = taskNext;
        }
    }

    return task;
}

void AITask_RemoveGetToolReferences(lpAITask aiTask)
{
    // TODO: Implement AITask_RemoveGetToolReferences
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
    if (liveObj->aiTask != NULL && liveObj->aiTask->taskType == AITask_Type_AnimationWait)
    {
        AITask_LiveObject_FUN_00404110(liveObj);
    }
}

B32 AITask_RemoveAttackPathReferences(Point2I* blockPos)
{
    // TODO: Implement AITask_RemoveAttackPathReferences

    return FALSE;
}

B32 AITask_Delay_RemoveClearReferences(Point2I* blockPos, B32 delay)
{
    // TODO: Implement AITask_Delay_RemoveClearReferences

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
    if (!Message_AnyUnitSelected())
        return;

    U32* outUnitsCount = &aiTask->unitListCount;
    lpLegoObject** outUnitsList = &aiTask->unitList;
    aiTask->flags |= (AITASK_FLAG_IMMEDIATESELECTION | AITASK_FLAG_DUPLICATE);
    Message_CopySelectedUnits(outUnitsList, outUnitsCount);

    U32 i = 0;
    if (*outUnitsCount != 0)
    {
        do
        {
            LegoObject_Route_End((*outUnitsList)[i], FALSE);
            if (dropCarried)
                LegoObject_DropCarriedObject((*outUnitsList)[i], FALSE);
            i++;
        } while (i < *outUnitsCount);
    }
}

void AITask_Route_End(struct LegoObject* liveObj, B32 completed)
{
    if (liveObj->aiTask != NULL)
    {
        if (!completed)
            liveObj->aiTask->flags &= ~AITASK_FLAG_PERFORMING;
        else if (liveObj->aiTask->taskType == AITask_Type_Goto)
        {
            AITask_LiveObject_FUN_00404110(liveObj);
            if (liveObj->type == LegoObject_RockMonster)
            {
                Message_AddMessageAction(Message_RockMonsterGotoComplete, liveObj, 0, NULL);
            }
        }
    }
}

void AITask_LiveObject_FUN_00403b30(struct LegoObject* holderObj, AITask_Type taskType, struct LegoObject* carriedObj)
{
    // TODO: Implement AITask_LiveObject_FUN_00403b30
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

void AITask_UpdateAll(F32 elapsedGame)
{
    AI_Priority finalPriority;

    const AI_Priority orePriority = aiGlobs.priorityValues[AI_Priority_Ore];
    const AI_Priority crystalPriority = aiGlobs.priorityValues[AI_Priority_Crystal];
    const AI_Priority constructionPriority = aiGlobs.priorityValues[AI_Priority_Construction];

    // Find the highest priority

    AI_Priority oreCrystalPriority = orePriority;

    // If Crystal has higher than or equal priority to Ore, use Crystal
    if (crystalPriority >= orePriority)
    {
        // Crystal has higher priority
        oreCrystalPriority = crystalPriority;
    }

    // Get the highest priority of Construction and Ore/Crystal
    if (constructionPriority >= oreCrystalPriority)
    {
        // Construction has higher priority
        finalPriority = constructionPriority;
    }
    else
    {
        // Ore/Crystal has higher priority
        finalPriority = crystalPriority;

        // If Ore has higher priority than Crystal, use Ore
        if (orePriority > crystalPriority)
        {
            // Ore has higher priority
            finalPriority = orePriority;
        }
    }

    // The final priority is the highest of Construction, Ore, and Crystal

    // TODO: Why is this setting the barrier priority to the final priority plus Ore priority?
    aiGlobs.priorityValues[AI_Priority_Barrier] = finalPriority + AI_Priority_Ore;

    if ((aiGlobs.flags & AITASK_GLOB_FLAG_DISABLEUPDATES) != AITASK_GLOB_FLAG_NONE)
        return;

    AITask_RunThroughLists(AITask_Callback_UpdateTask, &elapsedGame);
    for (lpAITask task = aiGlobs.pendingTaskList; task != NULL; task = task->next)
    {
        if ((task->taskType == AITask_Type_Repair && (task->flags & AITASK_FLAG_UPGRADEBUILDING) == AITASK_FLAG_NONE) &&
            (task->targetObject->health <= 0.0f || task->targetObject->health >= 100.0f))
        {
            task->flags |= AITASK_FLAG_REMOVING;
        }
    }

    AITask_FUN_00402240(&aiGlobs.pendingTaskList);
    AITask_FUN_00402240(&aiGlobs.creatureTaskList);
    aiGlobs.freeUnitCount = 0;
    aiGlobs.freeCreatureCount = 0;

    LegoObject_RunThroughListsSkipUpgradeParts(AITask_Callback_UpdateObject, &elapsedGame);

    AITask_FUN_00405b40();
    AITask_FUN_00405880();
}

B32 AITask_Callback_UpdateObject(lpLegoObject liveObj, void* context)
{
    F32* pElapsedGame = context;
    F32 elapsedGame = *pElapsedGame;

    aiGlobs.flags |= AITASK_GLOB_FLAG_UPDATINGOBJECT;

    if ((liveObj->flags3 & LIVEOBJ3_AITASK_UNK_400000) == LIVEOBJ3_NONE ||
        liveObj->health <= 0.0f ||
        Lego_IsFPObject(liveObj) ||
        (liveObj->driveObject != NULL && liveObj->type == LegoObject_MiniFigure) ||
        liveObj->carryingThisObject != NULL ||
        (liveObj->flags2 & (LIVEOBJ2_FROZEN | LIVEOBJ2_PUSHED)) != LIVEOBJ2_NONE ||
        (liveObj->flags1 & LIVEOBJ1_RUNNINGAWAY) != LIVEOBJ1_NONE)
    {
        goto endFunc;
    }

#ifdef _DEBUG
    AITask_Debug_UpdateInfo(liveObj);
#endif

    B32 someSpecialVar = FALSE;
    if (liveObj->aiTask == NULL)
    {
        Point2I pos;
        pos.x = 0;
        pos.y = 0;


        liveObj->aiTimer_338 += elapsedGame;
        Point2I blockPos;
        LegoObject_GetBlockPos(liveObj, &blockPos.x, &blockPos.y);

        if (liveObj->aiTimer_338 == elapsedGame ||
            (liveObj->flags4 & (LIVEOBJ4_ENTRANCEOCCUPIED | LIVEOBJ4_DOCKOCCUPIED)) != LIVEOBJ4_NONE ||
            !LegoObject_FUN_004439d0(liveObj, &blockPos, &pos, 0))
        {
            if (liveObj->aiTimer_338 <= 250.0f)
            {
                AITask* unkAiTask = NULL;
                AITask* aiTask = aiGlobs.pendingTaskList;
                if (aiGlobs.pendingTaskList == NULL)
                {
someLabel:
                    if (aiTask != NULL)
                    {
                        AITask_AssignTask(aiTask, unkAiTask, liveObj);
                        goto endFunc;
                    }
                }
                else
                {
                    AITask* task = aiTask;
                    do
                    {
                        AITaskFlags flags = task->flags;
                        if ((((flags & AITASK_FLAG_DISABLED) == AITASK_FLAG_NONE &&
                           ((flags & AITASK_FLAG_ACCEPTCARRYING) != AITASK_FLAG_NONE ||
                           (!LegoObject_VehicleMaxCarryChecksTime_FUN_00439c80(liveObj) &&
                           (flags & AITASK_FLAG_CARRYTASK) == AITASK_FLAG_NONE))) &&
                           (flags & AITASK_FLAG_IMMEDIATESELECTION) != AITASK_FLAG_NONE ||
                           !Message_LiveObject_Check_IsSelected_OrFlags3_200000(liveObj, NULL)) &&
                           ((flags & AITASK_FLAG_IMMEDIATESELECTION) == AITASK_FLAG_NONE ||
                           Message_FindIndexOfObject(task->unitList, task->unitListCount, liveObj, NULL)))
                        {
                            Point2I blockPos2 = task->blockPos;

                            B32 outBool;
                            if (AITask_FUN_00404ef0(task, liveObj, NULL, &blockPos2, &outBool, TRUE, TRUE) && !outBool)
                            {
                                if (flags & AITASK_FLAG_ACCEPTCARRYING != AITASK_FLAG_NONE)
                                {
                                    aiTask = task;
                                    goto someLabel;
                                }

                                Point2I objBlockPos;
                                LegoObject_GetBlockPos(liveObj, &objBlockPos.x, &objBlockPos.y);

                                AITask_Type taskType = task->taskType;
                                if (taskType == AITask_Type_Repair || taskType == AITask_Type_Reinforce || taskType == AITask_Type_Train ||
                                    taskType == AITask_Type_Upgrade ||
                                    (taskType == AITask_Type_Dig && (task->flags & AITASK_FLAG_DIGCONNECTION) == AITASK_FLAG_NONE))
                                {
                                    U32 uVar8;
                                    if (objBlockPos.x == blockPos2.x &&
                                       (uVar8 = blockPos2.y - objBlockPos.y >> 0x1f,
                                       (blockPos2.y - objBlockPos.y ^ uVar8) - uVar8 == 1) ||
                                       (blockPos2.y == objBlockPos.y &&
                                       (uVar8 = blockPos2.x - objBlockPos.x >> 0x1f,
                                       (blockPos2.x - objBlockPos.x ^ uVar8) - uVar8 == 1)))
                                    {
                                        aiTask = task;
                                        goto someLabel;
                                    }
                                }
                                else if (objBlockPos.x == blockPos2.x && objBlockPos.y == blockPos2.y)
                                {
                                    aiTask = task;
                                    goto someLabel;
                                }
                            }
                        }

                        if ((task->flags & AITASK_FLAG_CARRYTASK) != AITASK_FLAG_NONE &&
                            LegoObject_VehicleMaxCarryChecksTime_FUN_00439c80(liveObj) &&
                            (liveObj->flags3 & LIVEOBJ3_SELECTED) == LIVEOBJ3_NONE &&
                            liveObj->type != LegoObject_RockMonster)
                        {
                            // TODO: Implement AITask_Callback_UpdateObject
                        }

                        if ((task->flags & AITASK_FLAG_IMMEDIATESELECTION) != AITASK_FLAG_NONE &&
                            Message_FindIndexOfObject(task->unitList, task->unitListCount, liveObj, NULL))
                        {
                            aiTask = NULL;
                            goto someLabel;
                        }

                        task = task->next;
                    } while (task != NULL);
                }

                if (!LegoObject_VehicleMaxCarryChecksTime_FUN_00439c80(liveObj))
                {
                    if (aiGlobs.freeUnitCount < AITASK_FREE_UNIT_LIST_COUNT)
                    {
                        aiGlobs.freeUnitList[aiGlobs.freeUnitCount] = liveObj;
                        aiGlobs.freeUnitCount++;
                    }

                    if (aiGlobs.freeCreatureCount < AITASK_FREE_UNIT_LIST_COUNT &&
                        liveObj->type == LegoObject_RockMonster &&
                        (liveObj->flags3 & LIVEOBJ3_POWEROFF) == LIVEOBJ3_NONE)
                    {
                        aiGlobs.freeCreatureList[aiGlobs.freeCreatureCount] = liveObj;
                        aiGlobs.freeCreatureCount++;
                    }
                }
            }
            else
            {
                if (liveObj->type == LegoObject_MiniFigure && !Lego_IsFPObject(liveObj))
                {
                    LegoObject_TryWaiting(liveObj);
                }
                liveObj->aiTimer_338 = 0.0f;
            }
        }
        else if (pos.x != 0 && pos.y != 0)
        {
            liveObj->aiTask = AITask_Create(AITask_Type_Goto);
            liveObj->aiTask->blockPos.x = pos.x;
            liveObj->aiTask->blockPos.y = pos.y;
        }

        goto endFunc;
    }
    liveObj->aiTimer_338 = 0.0f;

    if ((liveObj->aiTask->flags & AITASK_FLAG_REMOVING) != AITASK_FLAG_NONE)
    {
        AITask_LiveObject_FUN_00404110(liveObj);
        LegoObject_Route_End(liveObj, FALSE);
        goto endFunc;
    }

    AITask_Type taskType = liveObj->aiTask->taskType;
    //Error_Debug(Error_Format("Task Type: %s\n", aiGlobs.aitaskName[taskType]));

    if (taskType != AITask_Type_Wait || liveObj->aiTask->time > 0.0f)
    {
        if (taskType == AITask_Type_Follow)
        {
            // TODO: Implement AITask_Callback_UpdateObject
        }
        else if (taskType == AITask_Type_FollowAttack)
        {
            // TODO: Implement AITask_Callback_UpdateObject
        }
        else if (((liveObj->aiTask->flags & AITASK_FLAG_PERFORMING) == AITASK_FLAG_NONE) &&
                ((liveObj->flags3 & LIVEOBJ3_SELECTED) == LIVEOBJ3_NONE))
        {
            B32 newBoolVar = TRUE;
            if (TRUE) // TODO: Why?
            {
                switch (taskType)
                {
                    case AITask_Type_Goto:
                    {
                        Point2I blockPos;
                        LegoObject_GetBlockPos(liveObj, &blockPos.x, &blockPos.y);
                        if (blockPos.x == liveObj->aiTask->blockPos.x &&
                            blockPos.y == liveObj->aiTask->blockPos.y)
                        {
                            newBoolVar = TRUE;
                            AITask_LiveObject_FUN_00404110(liveObj);
                        }
                        else
                        {
                            Point3F vertPoses[4];
                            Map3D_GetBlockVertexPositions(Lego_GetMap(), liveObj->aiTask->blockPos.x, liveObj->aiTask->blockPos.y, vertPoses);
                            Point2F rand;
                            rand.x = Maths_RandRange(vertPoses[0].x + 6.0f, vertPoses[2].x - 6.0f);
                            rand.y = Maths_RandRange(vertPoses[2].y + 6.0f, vertPoses[0].y - 6.0f);
                            StatsFlags1 statsFlags = StatsObject_GetStatsFlags1(liveObj);
                            // TODO: Double check this is correct
                            newBoolVar = AITask_LiveObject_FUN_00404d30(liveObj, &liveObj->aiTask->blockPos,
                                ((statsFlags & STATS1_ROUTEAVOIDANCE) != STATS1_NONE) ? &rand : NULL);
                        }
                        break;
                    }
                    // TODO: Implement AITask_Callback_UpdateObject
                    case AITask_Type_Dig:
                    {
                        StatsFlags1 sflags1;
                        if ((((liveObj->aiTask->flags & AITASK_FLAG_DIGCONNECTION) != AITASK_FLAG_NONE) &&
                            (sflags1 = StatsObject_GetStatsFlags1(liveObj),
                                (sflags1 & STATS1_SINGLEWIDTHDIG) != STATS1_NONE)) ||
                                ((liveObj->aiTask->flags & AITASK_FLAG_DIGCONNECTION) == AITASK_FLAG_NONE))
                        {
                            newBoolVar = LegoObject_RouteToDig_FUN_00447100(liveObj, liveObj->aiTask->blockPos.x, liveObj->aiTask->blockPos.y,
                                (liveObj->aiTask->flags & AITASK_FLAG_DIGCONNECTION) != AITASK_FLAG_NONE);
                        }
                        break;
                    }
                }
            }
            if (!newBoolVar)
            {
                someSpecialVar = TRUE;
            }
            else
            {
                liveObj->aiTask->flags |= AITASK_FLAG_PERFORMING;
            }
        }
    }
    else
    {
        AITask_LiveObject_FUN_00404110(liveObj);
    }

labth:
    if (someSpecialVar)
    {
        if ((liveObj->aiTask->taskType == AITask_Type_Dig) &&
            ((liveObj->aiTask->flags & AITASK_FLAG_DIGCONNECTION) != AITASK_FLAG_NONE))
        {
            liveObj->aiTask->timeIn = 100.0f; // (4 seconds)
        }

        if (((liveObj->aiTask->flags & AITASK_FLAG_REALLOCATE) != AITASK_FLAG_NONE) && (liveObj->aiTask->time < 0.0f))
        {
            liveObj->aiTask->time = 1500.0f;
        }

        AITask_LiveObject_FUN_00404110(liveObj);
    }

endFunc:
    aiGlobs.flags &= ~AITASK_GLOB_FLAG_UPDATINGOBJECT;
    return FALSE;
}

// Called during AITask_UpdateAll, to update two count-down timers for all tasks.
B32 AITask_Callback_UpdateTask(lpAITask aiTask, void* context)
{
    F32* pElapsedGame = context;
    F32 elapsedGame = *pElapsedGame;

    if ((aiTask->flags & AITASK_FLAG_NOTIMELIMIT) == AITASK_FLAG_NONE)
        aiTask->time -= elapsedGame;

    aiTask->timeIn -= elapsedGame;

    return FALSE;
}

B32 AITask_RunThroughLists(AITask_RunThroughListsCallback callback, void* data)
{
    for (U32 listIndex = 0; listIndex < aiGlobs.listCount; listIndex++)
    {
        if (aiGlobs.listSet[listIndex] != NULL)
        {
            U32 size = 1 << ((U8)listIndex & 0x1f);
            for (U32 i = 0; i < size; i++)
            {

                lpAITask aiTask = &aiGlobs.listSet[listIndex][i];
                if (aiTask != NULL && aiTask->nextFree == aiTask &&
                    callback(aiTask, data))
                {
                    return TRUE;
                }
            }
        }
    }

    return FALSE;
}

void AITask_FUN_00402240(lpAITask* refAiTask)
{
    // TODO: Implement AITask_FUN_00402240
}

void AITask_FUN_00405b40()
{
    B32 bVar1;

    U32 someThing;
    F32 someThingX;
    F32 someThingY;

    AITask* taskLast = NULL;
    F32 local_44 = 99999.0f;
    AITask* taskNext = aiGlobs.pendingTaskList;
    if (aiGlobs.pendingTaskList != NULL)
    {
        AITask* aiTask;
        do
        {
            aiTask = taskNext;
            if (aiTask->timeIn <= 0.0f &&
                (aiTask->flags & (AITASK_FLAG_DISABLED | AITASK_FLAG_CARRYTASK)) == AITASK_FLAG_NONE)
            {
                LegoObject* legoObj = NULL;
                LegoObject* legoObj2 = NULL;
                U32 iter = 0;
                if (aiGlobs.freeUnitCount != 0)
                {
                    LegoObject** freeUnitList = aiGlobs.freeUnitList;
                    LegoObject* objIter = legoObj;
                    do
                    {
                        legoObj = *freeUnitList;
                        if (legoObj != NULL)
                        {
                            if ((aiTask->flags & AITASK_FLAG_IMMEDIATESELECTION) == AITASK_FLAG_NONE)
                            {
                                B32 bVar3 = Message_LiveObject_Check_IsSelected_OrFlags3_200000(legoObj, NULL);
                                objIter = legoObj2;
                                if (!bVar3)
                                {
                                    if ((aiTask->flags & AITASK_FLAG_IMMEDIATESELECTION) != AITASK_FLAG_NONE)
                                    {
                                        goto lab1;
                                    }
                                    goto lab2;
                                }
                            }
                            else
                            {
lab1:
                                B32 bVar3 = Message_FindIndexOfObject(aiTask->unitList, aiTask->unitListCount, legoObj, NULL);
                                objIter = legoObj2;
                                if (bVar3)
                                {
lab2:
                                    F32 objX, objY;
                                    LegoObject_GetPosition(legoObj, &objX, &objY);

                                    Point2F somePoint;
                                    S32 someBool;
                                    B32 bVar3_ = AITask_FUN_00404ef0(aiTask, legoObj, &somePoint, NULL, &someBool, TRUE, TRUE);
                                    if (bVar3_)
                                    {
                                        Point2I newPoint;
                                        F32 someX = somePoint.x;
                                        F32 someY = somePoint.y;
                                        if (Map3D_WorldToBlockPos_NoZ(Lego_GetMap(), someX, someY, &newPoint.x, &newPoint.y))
                                        {
                                            Point2I newPoint2;
                                            someX = objX;
                                            someY = objY;
                                            if (Map3D_WorldToBlockPos_NoZ(Lego_GetMap(), someX, someY, &newPoint2.x, &newPoint2.y))
                                            {
                                                AITask_Type taskType = aiTask->taskType;
                                                bVar1 = TRUE;

                                                U32 uVar5;

                                                if (((taskType == AITask_Type_Dig && (aiTask->flags & AITASK_FLAG_DIGCONNECTION) == AITASK_FLAG_NONE) ||
                                                    taskType == AITask_Type_Repair || taskType == AITask_Type_Reinforce || taskType == AITask_Type_Train || taskType == AITask_Type_GetTool) &&
                                                    ((newPoint2.x != newPoint.x || (uVar5 = newPoint2.y - newPoint.y >> 0x1f, (newPoint2.y - newPoint.y ^ uVar5) - uVar5 != 1)) &&
                                                    (newPoint2.y != newPoint.y || (uVar5 = newPoint2.x - newPoint.x >> 0x1f, (newPoint2.x - newPoint.x ^ uVar5) - uVar5 != 1))))
                                                {
                                                    Point2I newPoint3;
                                                    newPoint3.x = newPoint.x;
                                                    newPoint3.y = newPoint.y;
                                                    if (!LiveObject_FUN_00431ba0(legoObj2, &newPoint3, &newPoint, TRUE))
                                                    {
                                                        bVar1 = FALSE;
                                                    }
                                                }

                                                if (bVar1)
                                                {
                                                    S32* local_30;
                                                    S32* local_38;
                                                    U32 count;
                                                    bVar3_ = LegoObject_Route_ScoreNoCallback_FUN_00440ef0(legoObj, newPoint2.x, newPoint2.y, newPoint.x, newPoint.y, &local_30, &local_38, &count);
                                                    if (!bVar3_)
                                                    {
                                                        aiTask->timeIn = 75.0f;
                                                    }
                                                    else
                                                    {
                                                        Mem_Free(local_30);
                                                        Mem_Free(local_38);
                                                    }
                                                }
                                            }
                                        }
                                    }

                                    if (someBool)
                                    {
                                        someThing = iter;
                                        someThingX = somePoint.x;
                                        someThingY = somePoint.y;
                                        break;
                                    }

                                    objIter = legoObj2;

                                    if (bVar3_)
                                    {
                                        F32 localF32 = somePoint.y - objY;
                                        F32 someSqrt = sqrtf(localF32 * localF32 + (somePoint.x - objX) * (somePoint.x - objX));
                                        Point2F dir;
                                        LegoObject_GetFaceDirection(legoObj, &dir);
                                        if (legoObj2 == NULL || someSqrt < local_44)
                                        {
                                            someThing = iter;
                                            someThingX = somePoint.x;
                                            someThingY = somePoint.y;
                                            objIter = legoObj;
                                            legoObj2 = legoObj;
                                            local_44 = someSqrt;
                                        }
                                    }
                                }
                            }
                        }

                        legoObj = objIter;
                        iter++;
                        freeUnitList++;
                        objIter = legoObj;
                    } while (iter < aiGlobs.freeUnitCount);
                }

                if (legoObj == NULL)
                {
                    Point2F thePoint;
                    if ((aiTask->flags & AITASK_FLAG_WAITINGFORTOOL) == AITASK_FLAG_NONE &&
                        aiGlobs.freeUnitCount != 0)
                    {
                        U32 i = 0;
                        do
                        {
                            LegoObject* theObj = aiGlobs.freeUnitList[i];
                            if (theObj != NULL)
                            {
                                if ((aiTask->flags & AITASK_FLAG_IMMEDIATESELECTION) == AITASK_FLAG_NONE)
                                {
                                    if (!Message_LiveObject_Check_IsSelected_OrFlags3_200000(theObj, NULL))
                                    {
                                        if ((aiTask->flags & AITASK_FLAG_IMMEDIATESELECTION) != AITASK_FLAG_NONE)
                                            goto dadLabel;

                                        goto grandpaLabel;
                                    }
                                }
                                else
                                {
dadLabel:
                                    if (Message_FindIndexOfObject(aiTask->unitList, aiTask->unitListCount, theObj, NULL))
                                    {
grandpaLabel:
                                        if (AITask_FUN_00404ef0(aiTask, theObj, NULL, NULL, NULL, FALSE, TRUE))
                                        {
                                            AITask_DoGetTool_FromTask(aiTask);
                                            break;
                                        }
                                    }
                                }
                            }
                            i++;
                        } while (i < aiGlobs.freeUnitCount);
                    }

                    if (aiTask->taskType != AITask_Type_Train &&
                        aiTask->trainFlags != LegoObject_AbilityType_Pilot &&
                        (aiTask->flags & AITASK_FLAG_WAITINGFORTRAIN) == AITASK_FLAG_NONE)
                    {
                        LegoObject* local_94 = NULL;
                        B32 theBool = FALSE;
                        if (aiGlobs.freeUnitCount != 0)
                        {
                            U32 i = 0;
                            do
                            {
                                LegoObject* theObj = aiGlobs.freeUnitList[i];
                                if (theObj != NULL)
                                {
                                    if ((aiTask->flags & AITASK_FLAG_IMMEDIATESELECTION) == AITASK_FLAG_NONE)
                                    {
                                        if (!Message_LiveObject_Check_IsSelected_OrFlags3_200000(theObj, NULL))
                                        {
                                            if ((aiTask->flags & AITASK_FLAG_IMMEDIATESELECTION) != AITASK_FLAG_NONE)
                                                goto momLabel;

                                            goto grandmaLabel;
                                        }
                                    }
                                    else
                                    {
momLabel:
                                        if (Message_FindIndexOfObject(aiTask->unitList, aiTask->unitListCount, theObj, NULL))
                                        {
grandmaLabel:
                                            B32 newBool = AITask_FUN_00404ef0(aiTask, theObj, &thePoint, NULL, NULL, FALSE, FALSE);
                                            if (newBool && (newBool = AITask_FUN_00404ef0(aiTask, theObj, &thePoint, NULL, NULL, FALSE, TRUE),
                                                local_94 = theObj, newBool != FALSE))
                                            {
                                                theBool = TRUE;
                                                break;
                                            }
                                        }
                                    }
                                }

                                i++;
                            } while (i < aiGlobs.freeUnitCount);
                        }

                        LegoObject* theLegoObj;
                        if (local_94 != NULL && !theBool &&
                            (theLegoObj = LegoObject_Search_FUN_00439110(NULL, &thePoint, aiTask->trainFlags), theLegoObj != NULL))
                        {
                            AITask_DoTrain_Target(theLegoObj, aiTask->trainFlags, aiTask->trainFlags != ABILITY_FLAG_REPAIR);
                            aiTask->flags |= AITASK_FLAG_WAITINGFORTRAIN;
                        }
                    }
                }
                else
                {
                    B32 theNewBool = TRUE;
                    Point2I theBlockPos;
                    LegoObject_GetBlockPos(legoObj, &theBlockPos.x, &theBlockPos.y);

                    Point2I theBlockPos2;
                    Map3D_WorldToBlockPos_NoZ(Lego_GetMap(), someThingX, someThingY, &theBlockPos2.x, &theBlockPos2.y);

                    AITask_Type taskType = aiTask->taskType;
                    U32 uVar5;
                    if ((taskType == AITask_Type_Clear || taskType == AITask_Type_Collect ||
                        taskType == AITask_Type_BuildPath || taskType == AITask_Type_Upgrade) &&
                        (theBlockPos.x != theBlockPos2.x || theBlockPos.y != theBlockPos2.y))
                    {
                        if (aiTask->assignedToObject == NULL)
                        {
                            Message_Type msgType;
                            if (TRUE) // likely a define in the original code
                            {
                                switch (taskType)
                                {
                                    case AITask_Type_Collect:
                                        msgType = Message_CollectCrystal;
                                    break;
                                    case AITask_Type_Clear:
                                        msgType = Message_Clear;
                                    break;
                                    case AITask_Type_Upgrade:
                                        msgType = Message_Upgrade;
                                    break;
                                    case AITask_Type_BuildPath:
                                        msgType = Message_BuildPath;
                                    break;
                                }
                            }
                            Message_AddMessageAction(msgType, legoObj, aiTask, &theBlockPos2);
                            if ((aiTask->flags & AITASK_FLAG_IMMEDIATESELECTION) == AITASK_FLAG_NONE)
                            {
                                aiTask->assignedToObject = legoObj;
                            }

                            if (aiTask->taskType == AITask_Type_Clear && legoObj->type == LegoObject_Vehicle)
                            {
                                AITask_Delay_RemoveClearReferences(&aiTask->blockPos, TRUE);
                                aiTask->timeIn = 0.0f;
                            }
                        }
nwLabel:
                        theNewBool = FALSE;
                    }
                    else if (((taskType == AITask_Type_Dig && (aiTask->flags & AITASK_FLAG_DIGCONNECTION) == AITASK_FLAG_NONE) ||
                            (taskType == AITask_Type_Repair || taskType == AITask_Type_Reinforce || taskType == AITask_Type_Train || taskType == AITask_Type_GetTool)) &&
                            (theBlockPos.x != theBlockPos2.x || (uVar5 = theBlockPos.y - theBlockPos2.y >> 0x1f, (theBlockPos.y - theBlockPos2.y ^ uVar5) - uVar5 != 1)) &&
                            (theBlockPos.y != theBlockPos2.y || (uVar5 = theBlockPos.x - theBlockPos2.x >> 0x1f, (theBlockPos.x - theBlockPos2.x ^ uVar5) - uVar5 != 1)))
                    {
                        if (aiTask->assignedToObject == NULL)
                        {
                            Message_Type msgType;
                            if (TRUE) // likely a define in the original code
                            {
                                switch (taskType)
                                {
                                    case AITask_Type_Dig:
                                        msgType = Message_Dig;
                                    break;
                                    case AITask_Type_Repair:
                                        msgType = Message_Repair;
                                    break;
                                    case AITask_Type_Reinforce:
                                        msgType = Message_Reinforce;
                                    break;
                                    case AITask_Type_GetTool:
                                        msgType = Message_CollectTool;
                                        break;
                                    case AITask_Type_Train:
                                        msgType = Message_Train;
                                    break;
                                }
                            }

                            Point2I blockOffPos;
                            if (!LiveObject_FUN_00431ba0(legoObj, &theBlockPos2, &blockOffPos, TRUE))
                            {
                                aiTask->priorityValue--;
                            }
                            else
                            {
                                Message_AddMessageAction(msgType, legoObj, aiTask, &blockOffPos);
                                if ((aiTask->flags & AITASK_FLAG_IMMEDIATESELECTION) == AITASK_FLAG_NONE)
                                {
                                    aiTask->assignedToObject = legoObj;
                                }
                            }
                        }
                        goto nwLabel;
                    }

                    if (theNewBool)
                    {
                        AITask_AssignTask(aiTask, taskLast, legoObj);
                    }

                    S32 freeIndex = aiGlobs.freeUnitCount - 1;
                    aiGlobs.freeUnitCount--;
                    aiGlobs.freeUnitList[someThing] = aiGlobs.freeUnitList[freeIndex];
                }
            }

            taskNext = aiTask->next;
            taskLast = aiTask;
        } while (aiTask->next != NULL);
    }
}

void AITask_FUN_00405880()
{
    // TODO: Implement AITask_FUN_00405880
}

void AITask_AssignTask(lpAITask aiTask1, lpAITask aiTask2, lpLegoObject liveObj)
{
    if ((aiTask1->flags & AITASK_FLAG_DUPLICATE) == AITASK_FLAG_NONE)
    {
        if (aiTask2 == NULL)
        {
            aiGlobs.pendingTaskList = aiTask1->next;
        }
        else
        {
            aiTask2->next = aiTask1->next;
        }
        aiTask1->next = NULL;
        liveObj->aiTask = aiTask1;
    }
    else
    {
        liveObj->aiTask = AITask_Clone(aiTask1);
        if ((aiTask1->flags & AITASK_FLAG_PAUSEDDUPLICATION) != AITASK_FLAG_NONE)
        {
            // 1 second
            aiTask1->timeIn = 25.0f;
        }
    }
    AITask_RemoveObject48References(liveObj);
    Bubble_LiveObject_MiniFigure_FUN_00407380(liveObj);
}

B32 AITask_FUN_00404ef0(lpAITask aiTask, lpLegoObject liveObj, Point2F* outPos, Point2I* blockPos, B32 *outBool, B32 param6, B32 param7)
{
    if (outBool != NULL)
        *outBool = FALSE;

    if ((aiTask->taskType != AITask_Type_Collect || aiTask->targetObject->type != LegoObject_Dynamite) && aiTask->timeIn > 0.0f)
    {
        return FALSE;
    }

    if ((StatsObject_GetStatsFlags1(liveObj) & STATS1_CANBEDRIVEN) != STATS1_NONE && liveObj->driveObject == NULL)
        return FALSE;

    if ((liveObj->flags1 & LIVEOBJ1_RUNNINGAWAY) != LIVEOBJ1_NONE)
        return FALSE;

    if ((liveObj->flags4 & LIVEOBJ4_UNK_400) != LIVEOBJ4_NONE)
        return FALSE;

    if ((legoGlobs.flags2 & GAME2_CALLTOARMS) != GAME2_NONE &&
        LegoObject_MiniFigureHasBeamEquipped2(liveObj) &&
        aiTask->taskType != AITask_Type_AttackRockMonster)
    {
        return FALSE;
    }

    AITask_Type taskType = aiTask->taskType;
    if (taskType == AITask_Type_AttackRockMonster)
    {
        // TODO: Implement AITask_FUN_00404ef0
    }
    else if (taskType == AITask_Type_FindLoad)
    {
        // TODO: Implement AITask_FUN_00404ef0
    }
    else if (taskType == AITask_Type_Collect)
    {
        // TODO: Implement AITask_FUN_00404ef0
    }
    else if (taskType == AITask_Type_Dig || taskType == AITask_Type_Goto ||
             taskType == AITask_Type_Reinforce || taskType == AITask_Type_Clear)
    {
        B32 someBool = TRUE;
        if (taskType == AITask_Type_Dig)
        {
            someBool = LiveObject_BlockCheck_FUN_004326a0(liveObj, aiTask->blockPos.x, aiTask->blockPos.y,
                aiTask->flags & AITASK_FLAG_DIGCONNECTION, TRUE);
        }

        if (param6 && (taskType = aiTask->taskType, taskType != AITask_Type_Goto) &&
            (taskType != AITask_Type_Dynamite &&
                !LegoObject_TaskHasTool_FUN_0044b780(liveObj, taskType)))
        {
            return FALSE;
        }

        if (aiTask->taskType == AITask_Type_Reinforce && liveObj->type != LegoObject_MiniFigure)
        {
            return FALSE;
        }

        if (aiTask->taskType == AITask_Type_Clear &&
            (StatsObject_GetStatsFlags1(liveObj) & STATS1_CANCLEARRUBBLE) == STATS1_NONE)
        {
            return FALSE;
        }

        if (aiTask->taskType == AITask_Type_Dig)
        {
            if ((liveObj->flags3 & LIVEOBJ3_CANDIG) == LIVEOBJ3_NONE)
                return FALSE;

            if (!someBool)
                return FALSE;

            if ((aiTask->flags & AITASK_FLAG_DIGCONNECTION) != AITASK_FLAG_NONE &&
                (StatsObject_GetStatsFlags1(liveObj) & STATS1_SINGLEWIDTHDIG) == STATS1_NONE)
            {
                return FALSE;
            }
        }

        if (outPos != NULL)
        {
            Map3D_BlockToWorldPos(Lego_GetMap(), aiTask->blockPos.x, aiTask->blockPos.y, &outPos->x, &outPos->y);
        }

        if (blockPos != NULL)
        {
            blockPos->x = aiTask->blockPos.x;
            blockPos->y = aiTask->blockPos.y;
        }
    }
    else
    {
        // TODO: Implement AITask_FUN_00404ef0
    }

    // TODO: Implement AITask_FUN_00404ef0

    return TRUE;
}

void AITask_DoGetTool_FromTask(lpAITask aiTask)
{
    // TODO: Implement AITask_DoGetTool_FromTask
}

// Removes references to object_48.
// But only from the `aiGlobs.AITaskUnkPtr` linked list.
void AITask_RemoveObject48References(lpLegoObject obj48)
{
    if (aiGlobs.pendingTaskList == NULL)
        return;

    AITask **pTask;
    AITask *task;

    task = aiGlobs.pendingTaskList;
    do
    {
        if (task->assignedToObject == obj48)
        {
            task->assignedToObject = NULL;
        }
        pTask = &task->next;
        task = *pTask;
    } while (*pTask != NULL);
}

lpLegoObject LegoObject_Search_FUN_00439110(lpLegoObject liveObj, Point2F* optWorldPos, LegoObject_AbilityFlags trainedType)
{
    // TODO: Implement LegoObject_Search_FUN_00439110

    return NULL;
}

void AITask_DoTrain_Target(struct LegoObject* targetObj, LegoObject_AbilityFlags trainFlags, B32 param3)
{
    // TODO: Implement AITask_DoTrain_Target
}

void AITask_LiveObject_SetAITaskUnk(lpLegoObject liveObj, AITask_Type taskType, lpLegoObject liveObj2, B32 param4)
{
    // TODO: Implement AITask_LiveObject_SetAITaskUnk
}

void AITask_LiveObject_FUN_00404110(lpLegoObject liveObj)
{
    if (liveObj->aiTask == NULL)
        return;

    lpAITask aiTask = liveObj->aiTask;
    liveObj->aiTask = liveObj->aiTask->next;

    if ((aiTask->flags & AITASK_FLAG_REALLOCATE) != AITASK_FLAG_NONE &&
        (aiTask->flags & AITASK_FLAG_REMOVING) == AITASK_FLAG_NONE)
    {
        // Prepend to linked list.
        aiTask->next = aiGlobs.pendingTaskList;
        aiGlobs.pendingTaskList = aiTask;
        return;
    }

    if ((aiTask->flags & AITASK_FLAG_CREATUREREALLOCATE) != AITASK_FLAG_NONE &&
        (aiTask->flags & AITASK_FLAG_REMOVING) == AITASK_FLAG_NONE)
    {
        // Prepend to linked list.
        aiTask->next = aiGlobs.creatureTaskList;
        aiGlobs.creatureTaskList = aiTask;
        return;
    }

    AITask_Remove(aiTask, FALSE);
}

void AITask_DoAnimationWait(struct LegoObject* liveObj)
{
    if (liveObj->aiTask == NULL || liveObj->aiTask->taskType != AITask_Type_AnimationWait)
    {
        AITask* newAITask = AITask_Create(AITask_Type_AnimationWait);
        newAITask->next = liveObj->aiTask;
        liveObj->aiTask = newAITask;
    }
}

void AITask_Game_PTL_GotoOrRMGoto(struct LegoObject* liveObj, Point2I* blockPos, AITask* referrerTask)
{
    lpAITask newAITask = AITask_Create(AITask_Type_Goto);
    newAITask->blockPos.x = blockPos->x;
    newAITask->blockPos.y = blockPos->y;
    newAITask->referrerTask = referrerTask;

    lpAITask tail = AITask_GetObjectTaskListTail(liveObj);
    if (tail != NULL)
    {
        tail->next = newAITask;
        return;
    }
    // If no tail, then this is the first task for this object.
    liveObj->aiTask = newAITask;
    Bubble_LiveObject_MiniFigure_FUN_00407380(liveObj);
}

void AITask_QueueGotoBlock_Group(struct LegoObject** unitList, U32 unitCount, Point2I* blockPos, B32 reEvalTasks)
{
    // TODO: Implement AITask_QueueGotoBlock_Group
}

B32 AITask_LiveObject_FUN_00404d30(struct LegoObject* liveObj, Point2I* blockPos, Point2F* pos2D)
{
    Point2I pos;
    LegoObject_GetBlockPos(liveObj, &pos.x, &pos.y);
    U32 count;
    S32* newX;
    S32* newY;
    if (pos.x == blockPos->x && pos.y == blockPos->y)
    {
        newX = &pos.x;
        newY = &pos.y;
        count = 1;
    }
    else
    {
        // TODO: Sometimes count is an absolutely massive number (max int?)
        if (!LegoObject_Route_ScoreNoCallback_FUN_00440ef0(liveObj, pos.x, pos.y, blockPos->x, blockPos->y, &newX, &newY, &count))
        {
            return FALSE;
        }
    }

    LegoObject_Route_AllocPtr_FUN_004419c0(liveObj, count, newX, newY, pos2D);
    if (newX != &pos.x)
    {
        Mem_Free(newX);
        Mem_Free(newY);
    }

    return TRUE;
}

#ifdef LEGORR_DEBUG_SHOW_INFO
#define AITask_Debug_InfoStringsCount 30
static char* sAITask_Debug_InfoStrings[AITask_Debug_InfoStringsCount] = { NULL };
static char* sAITask_Debug_InfoString = NULL;
static U32 sAITask_Debug_InfoStringCount = 0;
void AITask_Debug_UpdateInfo(lpLegoObject liveObject)
{
    if (liveObject->index >= AITask_Debug_InfoStringsCount)
        return;

    if (sAITask_Debug_InfoStrings[liveObject->index] != NULL)
    {
        Mem_Free(sAITask_Debug_InfoStrings[liveObject->index]);
        sAITask_Debug_InfoStrings[liveObject->index] = NULL;
    }

    if (liveObject->aiTask == NULL)
    {
        char buffer[256];
        sprintf_s(buffer, 256, "%d(%s): No AI Task", liveObject->index, LegoObject_Debug_GetTypeName(liveObject->type));
        U32 len = strlen(buffer) + 1;
        sAITask_Debug_InfoStrings[liveObject->index] = Mem_Alloc(len);
        strcpy_s(sAITask_Debug_InfoStrings[liveObject->index], len, buffer);
        return;
    }

    char buffer[256];
    sprintf_s(buffer, 256, "%d(%s): %s", liveObject->index, LegoObject_Debug_GetTypeName(liveObject->type), aiGlobs.aitaskName[liveObject->aiTask->taskType]);
    U32 len = strlen(buffer) + 1;
    sAITask_Debug_InfoStrings[liveObject->index] = Mem_Alloc(len);
    strcpy_s(sAITask_Debug_InfoStrings[liveObject->index], len, buffer);

    AITask_Debug_UpdateInfoString();
}

void AITask_Debug_UpdateInfoString()
{
    if (sAITask_Debug_InfoString != NULL)
    {
        Mem_Free(sAITask_Debug_InfoString);
        sAITask_Debug_InfoString = NULL;
    }

    sAITask_Debug_InfoStringCount = 0;

    U32 size = 0;
    for (U32 i = 0; i < AITask_Debug_InfoStringsCount; i++)
    {
        if (sAITask_Debug_InfoStrings[i] != NULL)
        {
            // +1 for newline
            size += strlen(sAITask_Debug_InfoStrings[i]) + 1;

            sAITask_Debug_InfoStringCount++;
        }
    }

    sAITask_Debug_InfoString = Mem_Alloc(size + 1);
    sAITask_Debug_InfoString[0] = '\0';

    for (U32 i = 0; i < AITask_Debug_InfoStringsCount; i++)
    {
        const char* str = sAITask_Debug_InfoStrings[i];
        if (str != NULL)
        {
            strcat_s(sAITask_Debug_InfoString, size + 1, str);
            strcat_s(sAITask_Debug_InfoString, size + 1, "\n");
        }
    }

    sAITask_Debug_InfoString[size] = '\0';
}

void AITask_Debug_GetInfoString(char* outString, U32* length)
{
    if (sAITask_Debug_InfoString == NULL || sAITask_Debug_InfoStringCount == 0)
    {
        const char* test = "No Tasks";
        *length = strlen(test);
        if (outString != NULL)
        {
            strcpy(outString, test);
        }
        return;
    }

    *length = strlen(sAITask_Debug_InfoString);
    if (outString != NULL)
    {
        strcpy(outString, sAITask_Debug_InfoString);
    }
}

void AITask_Debug_GetPendingTasksString(char* outString, U32* length)
{
    const U32 limit = 4;
    U32 size = 0;
    AITask* task = aiGlobs.pendingTaskList;
    U32 i = 0;
    do
    {
        if (i >= limit)
            break;
        const char* name;
        if (task == NULL)
            name = "NULL";
        else
            name = aiGlobs.aitaskName[task->taskType];
        size += strlen(name) + 2; // +2 for comma and space

        if (task == NULL)
            break;
        task = task->next;

        i++;
    } while (task != NULL);

    *length = size + 1; // +1 for null-terminator

    if (!outString)
        return;

    outString[0] = '\0';

    task = aiGlobs.pendingTaskList;
    i = 0;
    do
    {
        if (i >= limit)
            break;
        const char* name;
        if (task == NULL)
            name = "NULL";
        else
        name = aiGlobs.aitaskName[task->taskType];

        strcat_s(outString, *length, name);
        strcat_s(outString, *length, ", ");

        if (task == NULL)
            break;
        task = task->next;

        i++;
    } while (task != NULL);
}
#endif
