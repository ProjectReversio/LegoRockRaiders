#include "message.h"

#include "aitask.h"

void Message_Initialize()
{
    // TODO: Implement Message_Initialize
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
    return Message_Null; // TODO: Implement Message_LookupPTLEventIndex
}

void Message_PTL_Update()
{
    // TODO: Implement Message_PTL_Update
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
