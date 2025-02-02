#pragma once

#include "platform.h"
#include "maths.h"
#include "lego_object.h"

typedef enum Message_Type
{
    Message_Null                          = 0,
    Message_Select                        = 1,
    Message_Selected                      = 2,
    Message_ClearSelection                = 3,
    Message_Deselect                      = 4,
    Message_Goto                          = 5,
    Message_RockMonsterGoto               = 6,
    Message_RockMonsterGotoComplete       = 7,
    Message_UserGoto                      = 8,
    Message_FirstPerson                   = 9,
    Message_TrackObject                   = 10,
    Message_TopView                       = 11,
    Message_PlaySample                    = 12,
    Message_Dig                           = 13,
    Message_DigComplete                   = 14,
    Message_Repair                        = 15,
    Message_RepairComplete                = 16,
    Message_Reinforce                     = 17,
    Message_ReinforceComplete             = 18,
    Message_RockFall                      = 19,
    Message_RockFallComplete              = 20,
    Message_GenerateCrystal               = 21,
    Message_GenerateCrystalComplete       = 22,
    Message_CollectCrystal                = 23,
    Message_CollectCrystalComplete        = 24,
    Message_CrystalToRefinery             = 25,
    Message_CrystalToRefineryComplete     = 26,
    Message_GenerateOre                   = 27,
    Message_GenerateOreComplete           = 28,
    Message_CollectOre                    = 29,
    Message_CollectOreComplete            = 30,
    Message_GenerateRockMonster           = 31,
    Message_GenerateRockMonsterComplete   = 32,
    Message_GatherRock                    = 33,
    Message_GatherRockComplete            = 34,
    Message_PickRandomFloor               = 35,
    Message_PickRandomFloorComplete       = 36,
    Message_AttackBuilding                = 37,
    Message_AttackBuildingComplete        = 38,
    Message_Clear                         = 39,
    Message_ClearComplete                 = 40,
    Message_GetIn                         = 41,
    Message_ManSelectedAndMonsterClicked  = 42,
    Message_FollowAttack                  = 43,
    Message_CollectTool                   = 44,
    Message_ReduceSelection               = 45,
    Message_ClearFallIn                   = 46,
    Message_ClearFallInComplete           = 47,
    Message_BuildPath                     = 48,
    Message_BuildPathComplete             = 49,
    Message_Train                         = 50,
    Message_TrainComplete                 = 51,
    Message_GenerateCrystalAndOre         = 52,
    Message_GenerateCrystalAndOreComplete = 53,
    Message_GenerateFromCryOre            = 54,
    Message_GenerateFromCryOreComplete    = 55,
    Message_Upgrade                       = 56,
    Message_UpgradeComplete               = 57,
    Message_ClearBuilding                 = 58,
    Message_ClearBuildingComplete         = 59,
    Message_ClearInitial                  = 60,
    Message_ClearInitialComplete          = 61,
    Message_ClearRemovePath               = 62,
    Message_ClearRemovePathComplete       = 63,
    Message_Debug_DestroyAll              = 64,
    Message_Type_Count                    = 65,
} Message_Type;

extern void Message_Initialize();

extern void Message_Debug_RegisterSelectedUnitHotkey(U8 key, Message_Type messageType, LegoObject* argumentObj, U32 arg2, Point2I* opt_position);

extern B32 Message_AnyUnitSelected();

extern B32 Message_CopySelectedUnits(lpLegoObject **outUnitsList, U32* outUnitsCount);

extern void Message_AddMessageAction(Message_Type messageType, lpLegoObject argument1Obj, U32 argument2, Point2I* opt_blockPos);

extern lpLegoObject Message_GetPrimarySelectedUnit();
