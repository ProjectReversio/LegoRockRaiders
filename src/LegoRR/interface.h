#pragma once

#include "platform.h"
#include "advisor.h"
#include "fonts.h"
#include "config.h"
#include "input.h"
#include "lego.h"

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

typedef enum Interface_MenuItemType // : S32 // "Back" has no official name
{
    Interface_MenuItem_Back                  = 0,
	Interface_MenuItem_TeleportMan           = 1,
	Interface_MenuItem_BuildBuilding         = 2,
	Interface_MenuItem_BuildSmallVehicle     = 3,
	Interface_MenuItem_BuildLargeVehicle     = 4,
	Interface_MenuItem_LayPath               = 5,
	Interface_MenuItem_RemovePath            = 6,
	Interface_MenuItem_RepairLava            = 7,
	Interface_MenuItem_GeologistTest         = 8,
	Interface_MenuItem_ClearRubble           = 9,
	Interface_MenuItem_Dam                   = 10,
	Interface_MenuItem_Dig                   = 11,
	Interface_MenuItem_Reinforce             = 12,
	Interface_MenuItem_Dynamite              = 13,
	Interface_MenuItem_PlaceFence            = 14,
	Interface_MenuItem_DeselectDig           = 15,
	Interface_MenuItem_CancelConstruction    = 16,
	Interface_MenuItem_SelectMan             = 17,
	Interface_MenuItem_SelectVehicle         = 18,
	Interface_MenuItem_SelectBuilding        = 19,
	Interface_MenuItem_VehiclePickUp         = 20,
	Interface_MenuItem_MinifigurePickUp      = 21,
	Interface_MenuItem_UnLoadVehicle         = 22,
	Interface_MenuItem_UnLoadMinifigure      = 23,
	Interface_MenuItem_GetIn                 = 24,
	Interface_MenuItem_GetOut                = 25,
	Interface_MenuItem_GotoDock              = 26,
	Interface_MenuItem_LegoManGoto           = 27,
	Interface_MenuItem_VehicleGoto           = 28,
	Interface_MenuItem_VehicleDig            = 29,
	Interface_MenuItem_LegoManDig            = 30,
	Interface_MenuItem_GoFeed                = 31,
	Interface_MenuItem_DeleteMan             = 32,
	Interface_MenuItem_DropBirdScarer        = 33,
	Interface_MenuItem_PowerOn               = 34,
	Interface_MenuItem_PowerOff              = 35,
	Interface_MenuItem_Repair                = 36,
	Interface_MenuItem_MakeTeleporterPrimary = 37,
	Interface_MenuItem_EjectCrystal          = 38,
	Interface_MenuItem_EjectOre              = 39,
	Interface_MenuItem_DeleteBuilding        = 40,
	Interface_MenuItem_DeleteVehicle         = 41,
	Interface_MenuItem_DeleteElectricFence   = 42,
	Interface_MenuItem_Attack                = 43,
	Interface_MenuItem_GotoTopView           = 44,
	Interface_MenuItem_GotoFirstPerson       = 45,
	Interface_MenuItem_GotoSecondPerson      = 46,
	Interface_MenuItem_TrackObject           = 47,
	Interface_MenuItem_GetTool               = 48,
	Interface_MenuItem_GetDrill              = 49,
	Interface_MenuItem_GetSpade              = 50,
	Interface_MenuItem_GetHammer             = 51,
	Interface_MenuItem_GetSpanner            = 52,
	Interface_MenuItem_GetLaser              = 53,
	Interface_MenuItem_GetPusherGun          = 54,
	Interface_MenuItem_GetFreezerGun         = 55,
	Interface_MenuItem_GetBirdScarer         = 56,
	Interface_MenuItem_TrainSkill            = 57,
	Interface_MenuItem_TrainDriver           = 58,
	Interface_MenuItem_TrainEngineer         = 59,
	Interface_MenuItem_TrainGeologist        = 60,
	Interface_MenuItem_TrainPilot            = 61,
	Interface_MenuItem_TrainSailor           = 62,
	Interface_MenuItem_TrainDynamite         = 63,
	Interface_MenuItem_UpgradeMan            = 64,
	Interface_MenuItem_UpgradeBuilding       = 65,
	Interface_MenuItem_UpgradeVehicle        = 66,
	Interface_MenuItem_UpgradeEngine         = 67,
	Interface_MenuItem_UpgardeDrill          = 68,
	Interface_MenuItem_UpgardeScan           = 69,
	Interface_MenuItem_UpgardeCarry          = 70,
	Interface_MenuItem_Encyclopedia          = 71,
	Interface_MenuItem_ClearSelection        = 72,
	Interface_MenuItem_Build                 = 73,
	Interface_MenuItem_Type_Count            = 74,
	Interface_MenuItem_Type_Invalid          = -1,
} Interface_MenuItemType;

typedef enum Interface_MenuItemFlags
{
    INTERFACE_MENUITEM_FLAG_NONE  = 0,
    INTERFACE_MENUITEM_FLAG_UNK_1 = 0x1,
    INTERFACE_MENUITEM_FLAG_UNK_2 = 0x2,
    INTERFACE_MENUITEM_FLAG_FLASH = 0x4,
    INTERFACE_MENUITEM_FLAG_UNK_8 = 0x8,
} Interface_MenuItemFlags;

typedef enum Interface_GlobFlags
{
    INTERFACE_GLOB_FLAG_NONE     = 0,
    INTERFACE_GLOB_FLAG_UNK_4    = 0x4,
    INTERFACE_GLOB_FLAG_UNK_20   = 0x20,
    INTERFACE_GLOB_FLAG_UNK_40   = 0x40,
    INTERFACE_GLOB_FLAG_UNK_80   = 0x80,
    INTERFACE_GLOB_FLAG_UNK_100  = 0x100,
    INTERFACE_GLOB_FLAG_UNK_200  = 0x200,
    INTERFACE_GLOB_FLAG_UNK_400  = 0x400,
    INTERFACE_GLOB_FLAG_UNK_800  = 0x800,
    INTERFACE_GLOB_FLAG_UNK_1000 = 0x1000,
    INTERFACE_GLOB_FLAG_UNK_2000 = 0x2000,
    INTERFACE_GLOB_FLAG_UNK_4000 = 0x4000,
} Interface_GlobFlags;

// Simple list of icons-by-enum for an interface menu.
typedef struct Interface_Menu
{
	Interface_MenuItemType* iconList;
	U32 iconCount;
} Interface_Menu;

typedef struct Interface_Globs
{
    lpFont font;
    lpImage iconPanelImages[11]; // (cfg: InterfaceSurroundImages[0]) g_InterfaceSurroundImages
    Point2F iconPanelIconOffsets[11]; // (cfg: InterfaceSurroundImages[1, 2])
    lpImage iconPanelNoBackImages[11]; // (cfg: InterfaceSurroundImages[5])
    Point2F iconPanelNoBackIconOffsets[11]; // (cfg: InterfaceSurroundImages[6, 7])
    lpImage backButtonImage_hl; // (cfg: InterfaceBackButton[2])
    lpImage backButtonImage_pr; // (cfg: InterfaceBackButton[3])
    Point2F iconPanelBackButtonOffsets[11]; // (cfg: InterfaceSurroundImages[3, 4]) Point2F_ARRAY_004dde6c
    Size2I backButtonSize; // (cfg: InterfaceBackButton[0, 1])
    const char* backButtonText; // (cfg: InterfaceBackButton[4])
    const char* menuItemName[74];
    lpImage menuItemIcons[74];
    lpImage menuItemIcons_no[74];
    lpImage menuItemIcons_pr[74];
    const char* langMenuItemTexts[74];
    SFX_ID sfxMenuItemTexts[74];
    Keys8 menuItemF2keys[74];

    U16 padding1;

    Interface_MenuItemFlags menuItemFlags[74]; // (0x4: flashing)

    lpImage* vehicleItemIcons;
    lpImage* buildingItemIcons;
    lpImage* vehicleItemIcons_no;
    lpImage* buildingItemIcons_no;
    lpImage* vehicleItemIcons_pr;
    lpImage* buildingItemIcons_pr;
    const char* langMenuItemTexts_no[74];
    SFX_ID sfxMenuItemTexts_no[74];
    Keys8* vehicleItemF2keys;
    Keys8* buildingItemF2keys;
    Interface_MenuItemFlags* vehicleItemFlags;
    Interface_MenuItemFlags* buildingItemFlags;

    U32 menuItemClicks[74];
    U32* vehicleItemClicks;
    U32* buildingItemClicks;
    B32 menuItemUnkBools[74]; // (default: false)

    Interface_Menu menuList[24];

    Point2I selBlockPos;
    Interface_MenuType currMenuType;
    Point2F currMenuPosition; // (init: 565, 18) Current sliding position of menu. Point2F_004decd0
    LegoObject_Type objType_f80;
    LegoObject_ID objID_f84;
    Point2F slideStartPosition; // Point2F_004dece0
    Point2F slideEndPosition; // (init: pointf_f78) Point2F_004dece8
    F32 slideSpeed; // (init: 750.0f / 25.0f)
    Point2I highlightBlockPos;
    Interface_MenuType nextMenuType;
    Interface_MenuItemType menuItemType_fa8;
    U32 field_fac;
    U32 field_fb0;
    Area2F areaf_fb4;
    B32 flashingState; // For block and icon flashing. false = off, true = on
    F32 timer_fc8;
    Advisor_Type advisorType_fcc;
    Interface_MenuItemType menuItemType_fd0;

    LegoObject_Type objType_fd4;
    LegoObject_ID objID_fd8;
    B32 objectBools[20][15];
    lpImage dependenciesPlusImage;
    lpImage dependenciesMinusImage;
    F32 float_1494;
    WallHighlightType origWallHighlight; // (backup for when a wall has a queued action, but we want it to show the selected colour)
    SFX_ID sfxType_149c;
    Interface_GlobFlags flags;
    SFX_ID sfxType_14a4;
    B32 sfxPlaying;

} Interface_Globs;

extern Interface_Globs interfaceGlobs;

extern void Interface_Initialize(U32 x, U32 y, lpFont font);

extern void Interface_InitBuildItems();
extern void Interface_LoadMenuItems(lpConfig config, const char* gameName);
extern void Interface_LoadItemPanels(lpConfig config, const char* gameName);
extern void Interface_LoadPlusMinusImages(const char* plusName, const char* minusName);

extern void Interface_FUN_0041b3c0();

extern void Interface_FUN_0041b860(F32 elapsedAbs);

extern void Interface_FUN_0041e9f0(F32 elapsedAbs);

extern void Interface_SelectBlock(Interface_MenuType menuType, Point2I* blockPos);

extern void Interface_OpenMenu_FUN_0041b200(Interface_MenuType menuType, Point2I* blockPos);

extern void Interface_UnkSlideOffScreen_FUN_0041e980_internal(Interface_MenuType menuType, Point2I* optBlockPos);

extern void Interface_BackToMain_IfSelectedWall_IsBlockPos(Point2I *blockPos);

extern void Interface_BackToMain();

extern void Interface_AddAllMenuItems();

extern void Interface_AddMenuItems(Interface_MenuType menuType, U32 numItems, ...);

extern void Interface_SetScrollParameters(U32 xEnd, U32 yEnd, U32 xStart, U32 yStart, F32 timerFloat_750);

extern void Interface_ResetMenu();
