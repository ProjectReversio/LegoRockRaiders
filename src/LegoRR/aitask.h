#pragma once

#include "platform.h"
#include "maths.h"
#include "message.h"

typedef enum AITask_Type
{
    AITask_Type_Goto              = 0,
    AITask_Type_Follow            = 1,
    AITask_Type_FollowAttack      = 2,
    AITask_Type_Collect           = 3,
    AITask_Type_Gather            = 4,
    AITask_Type_Deposit           = 5,
    AITask_Type_Dump              = 6,
    AITask_Type_Request           = 7,
    AITask_Type_Dig               = 8,
    AITask_Type_Dynamite          = 9,
    AITask_Type_Repair            = 10,
    AITask_Type_Reinforce         = 11,
    AITask_Type_Clear             = 12,
    AITask_Type_Wait              = 13,
    AITask_Type_AnimationWait     = 14,
    AITask_Type_ElecFence         = 15,
    AITask_Type_Eat               = 16,
    AITask_Type_GotoEat           = 17,
    AITask_Type_FindDriver        = 18,
    AITask_Type_GetTool           = 19,
    AITask_Type_BirdScarer        = 20,
    AITask_Type_Upgrade           = 21,
    AITask_Type_BuildPath         = 22,
    AITask_Type_Train             = 23,
    AITask_Type_Depart            = 24,
    AITask_Type_AttackPath        = 25,
    AITask_Type_AttackRockMonster = 26,
    AITask_Type_Recharge          = 27,
    AITask_Type_Dock              = 28,
    AITask_Type_AttackObject      = 29,
    AITask_Type_FindLoad          = 30,
    AITask_Type_Count             = 31,
} AITask_Type;

typedef enum AI_Priority //: S32
{
    AI_Priority_Crystal           = 0,
    AI_Priority_Ore               = 1,
    AI_Priority_DefaultCollect    = 2,
    AI_Priority_Destruction       = 3,
    AI_Priority_Construction      = 4,
    AI_Priority_Request           = 5,
    AI_Priority_Reinforce         = 6,
    AI_Priority_Repair            = 7,
    AI_Priority_Clearing          = 8,
    AI_Priority_Storage           = 9,
    AI_Priority_Refining          = 10,
    AI_Priority_HealthLow         = 11,
    AI_Priority_GetIn             = 12,
    AI_Priority_Upgrade           = 13,
    AI_Priority_BuildPath         = 14,
    AI_Priority_AttackRockMonster = 15,
    AI_Priority_Barrier           = 16,
    AI_Priority_FindLoad          = 17,
    AI_Priority_Recharge          = 18,
    AI_Priority_UpgradeBuilding   = 19,
    AI_Priority_Gather            = 20,
    AI_Priority_Steal             = 21,
    AI_Priority_Punch             = 22,
    AI_Priority_Depart            = 23,
    AI_Priority_AttackPath        = 24,
    AI_Priority_AttackObject      = 25,
    AI_Priority_Congregate        = 26,
    AI_Priority_Count             = 27,
} AI_Priority;

typedef enum AITaskFlags //: U32
{
    AITASK_FLAG_NONE               = 0,
    AITASK_FLAG_PERFORMING         = 0x1,
    AITASK_FLAG_UPDATED            = 0x2,
    AITASK_FLAG_VOLATILE           = 0x4,
    AITASK_FLAG_DIGCONNECTION      = 0x8,
    AITASK_FLAG_REALLOCATE         = 0x10,
    AITASK_FLAG_REMOVING           = 0x20,
    AITASK_FLAG_DUPLICATE          = 0x40,
    AITASK_FLAG_NOTIMELIMIT        = 0x80,
    AITASK_FLAG_IMMEDIATESELECTION = 0x100,
    AITASK_FLAG_CLONED             = 0x200,
    AITASK_FLAG_CARRYTASK          = 0x400,
    AITASK_FLAG_FACEOUT            = 0x800,
    AITASK_FLAG_WAITINGFORTOOL     = 0x1000,
    AITASK_FLAG_CREATUREREALLOCATE = 0x2000,
    AITASK_FLAG_FALLINCLEAR        = 0x4000,
    AITASK_FLAG_WAITINGFORTRAIN    = 0x8000,
    AITASK_FLAG_MAXIMUMPRIORITY    = 0x10000,
    AITASK_FLAG_MANUALYSELECTED    = 0x20000,
    AITASK_FLAG_PAUSEDDUPLICATION  = 0x40000,
    AITASK_FLAG_DISABLED           = 0x80000,
    AITASK_FLAG_ACCEPTCARRYING     = 0x100000,
    AITASK_FLAG_UPGRADEBUILDING    = 0x200000,
} AITaskFlags;

typedef enum AITask_GlobFlags
{
    AITASK_GLOB_FLAG_NONE           = 0,
    AITASK_GLOB_FLAG_INITIALIZED    = 0x1,
    AITASK_GLOB_FLAG_REMOVING       = 0x2,
    AITASK_GLOB_FLAG_UPDATINGOBJECT = 0x4,
    AITASK_GLOB_FLAG_DISABLEUPDATES = 0x80000000,
} AITask_GlobFlags;

typedef struct AITask
{
    AITask_Type taskType;
    struct AITask* referrerTask; // Passed from certain Message_Events. Assigned but never used.
    Point2I blockPos;
    lpLegoObject targetObject;
    F32 time; // Count-down timer.
    F32 timeIn; // Count-down timer.
    S32 priorityValue; // Field is known to be signed (range: 0,99).
    LegoObject_Type objType;
    LegoObject_ID objID;
    U32 objLevel;
    U32 constructHandle;
    Point2F blockOffset;
    LegoObject_ToolType toolType;
    LegoObject_AbilityType trainFlags;
    lpLegoObject* unitList;
    U32 unitListCount;
    lpLegoObject assignedToObject; // Likely related to tasks with a two-object relation. Like training at a building.
    AI_Priority priorityType;
    U32 creationTime; // Timestamp of task creation in `AITask_Create`, obtained from `Main_GetTime()`.
    Message_Type completeAction;
    struct AITask* getToolTask; // (bi-directional link between GetTool and GetTool_FromText)
    AITaskFlags flags;
    struct AITask* next; // Next in linked lists for `aiGlobs.AITaskUnkPtr`, `aiGlobs.AITaskDataNext`, and `LegoObject::aiTask`.
    struct AITask* nextFree; // (for listSet)
} AITask, *lpAITask;

typedef struct AITask_Globs
{
    lpAITask listSet[12];
    lpAITask freeList;
    U32 listCount;
    const char* aitaskName[31];
    const char* priorityName[AI_Priority_Count];
    AI_Priority priorityValues[AI_Priority_Count];
    lpAITask pendingTaskList;
    lpAITask creatureTaskList;
    lpLegoObject freeUnitList[50];
    U32 freeUnitCount;
    lpLegoObject freeCreatureList[50];
    U32 freeCreatureCount;
    U32 requestObjCounts[20][15][16];
    B32 disabledPriorities[AI_Priority_Count];
    AITask_GlobFlags flags;
} AITask_Globs;

extern AITask_Globs aiGlobs;

extern void AITask_Initialize();

extern void AITask_AddList();

extern lpAITask AITask_Create(AITask_Type taskType);

// levelCleanup is only true when called by `AITask_Callback_Remove`.
extern void AITask_Remove(lpAITask aiTask, B32 levelCleanup);

extern lpAITask AITask_Clone(lpAITask aiTask);

extern void AITask_RemoveGetToolReferences(lpAITask aiTask);

extern void AITask_Game_SetNoGather(B32 noGather);

extern void AITask_DoCollect(struct LegoObject* liveObj, F32 param2);

extern void AITask_LiveObject_Unk_UpdateAITask_AnimationWait(struct LegoObject* liveObj);

extern B32 AITask_RemoveAttackPathReferences(Point2I* blockPos);

extern void AITask_DoClear_AtPosition(Point2I* blockPos, Message_Type completeAction);

extern void AITask_DoDig_AtBlockPos(Point2I* blockPos, B32 param2, B32 param3);

extern void AITask_UnkInitRouting_FUN_00402530(lpAITask aiTask, B32 dropCarried);

extern lpAITask AITask_InitTask_1(lpAITask aiTask, AI_Priority priorityType);

extern void AITask_UpdateAll(F32 elapsedGame);

extern B32 AITask_Callback_UpdateObject(struct LegoObject* liveObj, void* context);

// Called during AITask_UpdateAll, to update two count-down timers for all tasks.
extern B32 AITask_Callback_UpdateTask(lpAITask aiTask, void* context);

typedef B32 (*AITask_RunThroughListsCallback)(lpAITask aiTask, void* data);
extern B32 AITask_RunThroughLists(AITask_RunThroughListsCallback callback, void* data);

extern void AITask_FUN_00402240(lpAITask* refAiTask);

extern void AITask_FUN_00405b40();

extern void AITask_FUN_00405880();

extern void AITask_FUN_00406290(lpAITask aiTask1, lpAITask aiTask2, lpLegoObject liveObj);

// Removes references to object_48.
// But only from the `aiGlobs.AITaskUnkPtr` linked list.
extern void AITask_RemoveObject48References(lpLegoObject obj48);

extern void AITask_LiveObject_SetAITaskUnk(struct LegoObject* liveObj, AITask_Type taskType, struct LegoObject* liveObj2, B32 param4);

extern void AITask_LiveObject_FUN_00404110(struct LegoObject* liveObj);

extern void AITask_DoAnimationWait(struct LegoObject* liveObj);

#ifdef LEGORR_DEBUG_SHOW_INFO
extern void AITask_Debug_UpdateInfo(struct LegoObject* liveObj);
extern void AITask_Debug_UpdateInfoString();
extern void AITask_Debug_GetInfoString(char* outString, U32* length);
#endif
