#pragma once

#include "platform.h"
#include "fonts.h"
#include "config.h"

typedef enum Interface_MenuType
{
    Interface_Menu_Main              = 0,
    Interface_Menu_Ground            = 1,
    Interface_Menu_Erode             = 2,
    Interface_Menu_PlaceFence        = 3,
    Interface_Menu_Construction      = 4,
    Interface_Menu_Rubble            = 5,
    Interface_Menu_UNK_6             = 6,
    Interface_Menu_Wall              = 7,
    Interface_Menu_Tracker           = 8,
    Interface_Menu_LegoMan           = 9,
    Interface_Menu_LandVehicle       = 10,
    Interface_Menu_WaterVehicle      = 11,
    Interface_Menu_UnmannedVehicle   = 12,
    Interface_Menu_Building          = 13,
    Interface_Menu_ElectricFence     = 14,
    Interface_Menu_FP                = 15,
    Interface_Menu_GetTool           = 16,
    Interface_Menu_TrainSkill        = 17,
    Interface_Menu_UpgradeVehicle    = 18,
    Interface_Menu_BuildBuilding     = 19,
    Interface_Menu_BuildSmallVehicle = 20,
    Interface_Menu_BuildLargeVehicle = 21,
    Interface_Menu_Type_Count        = 22,
    Interface_Menu_Encyclopedia      = 23,
} Interface_MenuType;

extern void Interface_Initialize(U32 x, U32 y, lpFont font);

extern void Interface_InitBuildItems();
extern void Interface_LoadMenuItems(lpConfig config, const char* gameName);
extern void Interface_LoadItemPanels(lpConfig config, const char* gameName);
extern void Interface_LoadPlusMinusImages(const char* plusName, const char* minusName);

extern void Interface_FUN_0041b3c0();

extern void Interface_FUN_0041b860(F32 elapsedAbs);

extern void Interface_SelectBlock(Interface_MenuType menuType, Point2I* blockPos);

extern void Interface_OpenMenu_FUN_0041b200(Interface_MenuType menuType, Point2I* blockPos);
