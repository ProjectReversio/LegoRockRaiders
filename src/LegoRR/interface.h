#pragma once

#include "platform.h"
#include "fonts.h"
#include "config.h"
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

typedef struct Interface_Globs
{
    lpFont font;

    // TODO: Implement Interface_Globs

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

    // TODO: Implement Interface_Globs

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

extern void Interface_SelectBlock(Interface_MenuType menuType, Point2I* blockPos);

extern void Interface_OpenMenu_FUN_0041b200(Interface_MenuType menuType, Point2I* blockPos);

extern void Interface_UnkSlideOffScreen_FUN_0041e980_internal(Interface_MenuType menuType, Point2I* optBlockPos);
