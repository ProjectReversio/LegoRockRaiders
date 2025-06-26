#include "message.h"

#include "aitask.h"
#include "ptl.h"

Message_Globs messageGlobs = { 0 };

void Message_Initialize()
{
    messageGlobs.messageName[0] = "Message_Null";
    messageGlobs.messageName[1] = "Message_Select";
    messageGlobs.messageName[2] = "Message_Selected";
    messageGlobs.messageName[3] = "Message_ClearSelection";
    messageGlobs.messageName[4] = "Message_Deselect";
    messageGlobs.messageName[5] = "Message_Goto";
    messageGlobs.messageName[6] = "Message_RockMonsterGoto";
    messageGlobs.messageName[7] = "Message_RockMonsterGotoComplete";
    messageGlobs.messageName[8] = "Message_UserGoto";
    messageGlobs.messageName[9] = "Message_FirstPerson";
    messageGlobs.messageName[10] = "Message_TrackObject";
    messageGlobs.messageName[11] = "Message_TopView";
    messageGlobs.messageName[12] = "Message_PlaySample";
    messageGlobs.messageName[13] = "Message_Dig";
    messageGlobs.messageName[14] = "Message_DigComplete";
    messageGlobs.messageName[15] = "Message_Repair";
    messageGlobs.messageName[16] = "Message_RepairComplete";
    messageGlobs.messageName[17] = "Message_Reinforce";
    messageGlobs.messageName[18] = "Message_ReinforceComplete";
    messageGlobs.messageName[19] = "Message_RockFall";
    messageGlobs.messageName[20] = "Message_RockFallComplete";
    messageGlobs.messageName[21] = "Message_GenerateCrystal";
    messageGlobs.messageName[22] = "Message_GenerateCrystalComplete";
    messageGlobs.messageName[23] = "Message_CollectCrystal";
    messageGlobs.messageName[24] = "Message_CollectCrystalComplete";
    messageGlobs.messageName[25] = "Message_CrystalToRefinery";
    messageGlobs.messageName[26] = "Message_CrystalToRefineryComplete";
    messageGlobs.messageName[27] = "Message_GenerateOre";
    messageGlobs.messageName[28] = "Message_GenerateOreComplete";
    messageGlobs.messageName[29] = "Message_CollectOre";
    messageGlobs.messageName[30] = "Message_CollectOreComplete";
    messageGlobs.messageName[31] = "Message_GenerateRockMonster";
    messageGlobs.messageName[32] = "Message_GenerateRockMonsterComplete";
    messageGlobs.messageName[33] = "Message_GatherRock";
    messageGlobs.messageName[34] = "Message_GatherRockComplete";
    messageGlobs.messageName[35] = "Message_PickRandomFloor";
    messageGlobs.messageName[36] = "Message_PickRandomFloorComplete";
    messageGlobs.messageName[37] = "Message_AttackBuilding";
    messageGlobs.messageName[38] = "Message_AttackBuildingComplete";
    messageGlobs.messageName[39] = "Message_Clear";
    messageGlobs.messageName[40] = "Message_ClearComplete";
    messageGlobs.messageName[41] = "Message_GetIn";
    messageGlobs.messageName[42] = "Message_ManSelectedAndMonsterClicked";
    messageGlobs.messageName[43] = "Message_FollowAttack";
    messageGlobs.messageName[44] = "Message_CollectTool";
    messageGlobs.messageName[45] = "Message_ReduceSelection";
    messageGlobs.messageName[46] = "Message_ClearFallIn";
    messageGlobs.messageName[47] = "Message_ClearFallInComplete";
    messageGlobs.messageName[48] = "Message_BuildPath";
    messageGlobs.messageName[49] = "Message_BuildPathComplete";
    messageGlobs.messageName[50] = "Message_Train";
    messageGlobs.messageName[51] = "Message_TrainComplete";
    messageGlobs.messageName[52] = "Message_GenerateCrystalAndOre";
    messageGlobs.messageName[53] = "Message_GenerateCrystalAndOreComplete";
    messageGlobs.messageName[54] = "Message_GenerateFromCryOre";
    messageGlobs.messageName[55] = "Message_GenerateFromCryOreComplete";
    messageGlobs.messageName[56] = "Message_Upgrade";
    messageGlobs.messageName[57] = "Message_UpgradeComplete";
    messageGlobs.messageName[58] = "Message_ClearBuilding";
    messageGlobs.messageName[59] = "Message_ClearBuildingComplete";
    messageGlobs.messageName[60] = "Message_ClearInitial";
    messageGlobs.messageName[61] = "Message_ClearInitialComplete";
    messageGlobs.messageName[62] = "Message_ClearRemovePath";
    messageGlobs.messageName[63] = "Message_ClearRemovePathComplete";
    messageGlobs.messageName[64] = "Message_Debug_DestroyAll";
}

void Message_Debug_RegisterSelectedUnitHotkey(U8 key, Message_Type messageType, LegoObject* argumentObj, U32 arg2, Point2I* opt_position)
{
    // TODO: Implement Message_Debug_RegisterSelectedUnitHotkey
}

B32 Message_AnyUnitSelected()
{
    return FALSE;
}

B32 Message_CopySelectedUnits(lpLegoObject **outUnitsList, U32* outUnitsCount)
{
    // TODO: Implement Message_CopySelectedUnits

    return FALSE;
}

void Message_AddMessageAction(Message_Type messageType, lpLegoObject argument1Obj, lpAITask argument2, Point2I* opt_blockPos)
{
    // TODO: Implement Message_AddMessageAction
}

lpLegoObject Message_GetPrimarySelectedUnit()
{
    // TODO: Implement Message_GetPrimarySelectedUnit

    return NULL;
}

Message_Type Message_LookupPTLEventIndex(const char* ptlName)
{
    // TODO: Possible uninitialized variable bug here, just like in the original code.
    Message_Type cmpResult;
    for (Message_Type index = Message_Null; index < Message_Type_Count; index++)
    {
        cmpResult = _stricmp(messageGlobs.messageName[index] + 8, ptlName); // + 8 to skip "Message_" prefix
        if (!cmpResult)
            return index;
    }
    return cmpResult;
}

void Message_PTL_Update()
{
    // TODO: Implement Message_PTL_Update

    for (U32 i = 0; i < messageGlobs.eventCounts[messageGlobs.eventAB]; i++)
    {
        PTL_EventToAction(&messageGlobs.eventLists[messageGlobs.eventAB][i]);
    }

    B32 eventAB = messageGlobs.eventAB; // Store the current eventAB state
    U32 count = messageGlobs.eventCounts[messageGlobs.eventAB];
    messageGlobs.eventCounts[messageGlobs.eventAB] = 0;
    messageGlobs.eventAB = !messageGlobs.eventAB; // Toggle between 0 and 1
    messageGlobs.eventCounts[messageGlobs.eventAB] = 0;

    for (U32 i = 0; i < count; i++)
    {
        Message_Event event = messageGlobs.eventLists[eventAB][i];
        if (TRUE) // TODO: Why is this in the original code?
        {
            switch (event.type)
            {
                // TODO: Implement all the message types

                case Message_Goto:
                case Message_RockMonsterGoto:
                    AITask_Game_PTL_GotoOrRMGoto(event.argumentObj, &event.blockPos, (AITask*)event.argument2);
                    break;
                case Message_UserGoto:
                    AITask_QueueGotoBlock_Group(messageGlobs.selectedUnitList, messageGlobs.selectedUnitCount, &event.blockPos, (B32)event.argument2);
                    break;

                // TODO: Implement all the message types
            }
        }
    }
}

B32 Message_LiveObject_Check_IsSelected_OrFlags3_200000(lpLegoObject liveObj, U32* outIndex)
{
    // TODO: Implement Message_LiveObject_Check_IsSelected_OrFlags3_200000

    return FALSE;
}

B32 Message_FindIndexOfObject(lpLegoObject* objsTable, U32 objsCount, lpLegoObject thisObj, U32* outIndex)
{
    if (objsCount == 0)
        return FALSE;

    U32 i = 0;
    do
    {
        if (*objsTable == thisObj)
        {
            if (outIndex != NULL)
                *outIndex = i;

            return TRUE;
        }

        i++;
        objsTable++;
    } while (i < objsCount);

    return FALSE;
}
