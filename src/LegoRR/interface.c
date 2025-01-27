#include "interface.h"

#include "lego.h"
#include "map3d.h"
#include "mem.h"
#include "message.h"
#include "tooltip.h"

Interface_Globs interfaceGlobs = {};

void Interface_Initialize(U32 x, U32 y, lpFont font)
{
    ToolTip_SetFlag10(ToolTip_InterfaceMenu, TRUE);
    interfaceGlobs.currMenuPosition.x = (F32)x;
    interfaceGlobs.currMenuPosition.y = (F32)y;
    interfaceGlobs.font = font;

    interfaceGlobs.menuItemName[0] = "Interface_MenuItem_BackToDefault";
    interfaceGlobs.menuItemName[1] = "Interface_MenuItem_TeleportMan";
    interfaceGlobs.menuItemName[2] = "Interface_MenuItem_BuildBuilding";
    interfaceGlobs.menuItemName[3] = "Interface_MenuItem_BuildSmallVehicle";
    interfaceGlobs.menuItemName[4] = "Interface_MenuItem_BuildLargeVehicle";
    interfaceGlobs.menuItemName[5] = "Interface_MenuItem_LayPath";
    interfaceGlobs.menuItemName[6] = "Interface_MenuItem_RemovePath";
    interfaceGlobs.menuItemName[7] = "Interface_MenuItem_RepairLava";
    interfaceGlobs.menuItemName[8] = "Interface_MenuItem_GeologistTest";
    interfaceGlobs.menuItemName[9] = "Interface_MenuItem_ClearRubble";
    interfaceGlobs.menuItemName[10] = "Interface_MenuItem_Dam";
    interfaceGlobs.menuItemName[11] = "Interface_MenuItem_Dig";
    interfaceGlobs.menuItemName[12] = "Interface_MenuItem_Reinforce";
    interfaceGlobs.menuItemName[13] = "Interface_MenuItem_Dynamite";
    interfaceGlobs.menuItemName[14] = "Interface_MenuItem_PlaceFence";
    interfaceGlobs.menuItemName[15] = "Interface_MenuItem_DeselectDig";
    interfaceGlobs.menuItemName[16] = "Interface_MenuItem_CancelConstruction";
    interfaceGlobs.menuItemName[17] = "Interface_MenuItem_SelectMan";
    interfaceGlobs.menuItemName[18] = "Interface_MenuItem_SelectVehicle";
    interfaceGlobs.menuItemName[19] = "Interface_MenuItem_SelectBuilding";
    interfaceGlobs.menuItemName[20] = "Interface_MenuItem_VehiclePickUp";
    interfaceGlobs.menuItemName[21] = "Interface_MenuItem_MinifigurePickUp";
    interfaceGlobs.menuItemName[22] = "Interface_MenuItem_UnLoadVehicle";
    interfaceGlobs.menuItemName[23] = "Interface_MenuItem_UnLoadMinifigure";
    interfaceGlobs.menuItemName[24] = "Interface_MenuItem_GetIn";
    interfaceGlobs.menuItemName[25] = "Interface_MenuItem_GetOut";
    interfaceGlobs.menuItemName[26] = "Interface_MenuItem_GotoDock";
    interfaceGlobs.menuItemName[27] = "Interface_MenuItem_LegoManGoto";
    interfaceGlobs.menuItemName[28] = "Interface_MenuItem_VehicleGoto";
    interfaceGlobs.menuItemName[29] = "Interface_MenuItem_VehicleDig";
    interfaceGlobs.menuItemName[30] = "Interface_MenuItem_LegoManDig";
    interfaceGlobs.menuItemName[31] = "Interface_MenuItem_GoFeed";
    interfaceGlobs.menuItemName[32] = "Interface_MenuItem_DeleteMan";
    interfaceGlobs.menuItemName[33] = "Interface_MenuItem_DropBirdScarer";
    interfaceGlobs.menuItemName[34] = "Interface_MenuItem_PowerOn";
    interfaceGlobs.menuItemName[35] = "Interface_MenuItem_PowerOff";
    interfaceGlobs.menuItemName[36] = "Interface_MenuItem_Repair";
    interfaceGlobs.menuItemName[37] = "Interface_MenuItem_MakeTeleporterPrimary";
    interfaceGlobs.menuItemName[38] = "Interface_MenuItem_EjectCrystal";
    interfaceGlobs.menuItemName[39] = "Interface_MenuItem_EjectOre";
    interfaceGlobs.menuItemName[40] = "Interface_MenuItem_DeleteBuilding";
    interfaceGlobs.menuItemName[42] = "Interface_MenuItem_DeleteElectricFence";
    interfaceGlobs.menuItemName[41] = "Interface_MenuItem_DeleteVehicle";
    interfaceGlobs.menuItemName[43] = "Interface_MenuItem_Attack";
    interfaceGlobs.menuItemName[44] = "Interface_MenuItem_GotoTopView";
    interfaceGlobs.menuItemName[45] = "Interface_MenuItem_GotoFirstPerson";
    interfaceGlobs.menuItemName[46] = "Interface_MenuItem_GotoSecondPerson";
    interfaceGlobs.menuItemName[47] = "Interface_MenuItem_TrackObject";
    interfaceGlobs.menuItemName[48] = "Interface_MenuItem_GetTool";
    interfaceGlobs.menuItemName[49] = "Interface_MenuItem_GetDrill";
    interfaceGlobs.menuItemName[50] = "Interface_MenuItem_GetSpade";
    interfaceGlobs.menuItemName[51] = "Interface_MenuItem_GetHammer";
    interfaceGlobs.menuItemName[52] = "Interface_MenuItem_GetSpanner";
    interfaceGlobs.menuItemName[53] = "Interface_MenuItem_GetLaser";
    interfaceGlobs.menuItemName[54] = "Interface_MenuItem_GetPusherGun";
    interfaceGlobs.menuItemName[55] = "Interface_MenuItem_GetFreezerGun";
    interfaceGlobs.menuItemName[56] = "Interface_MenuItem_GetBirdScarer";
    interfaceGlobs.menuItemName[57] = "Interface_MenuItem_TrainSkill";
    interfaceGlobs.menuItemName[58] = "Interface_MenuItem_TrainDriver";
    interfaceGlobs.menuItemName[59] = "Interface_MenuItem_TrainEngineer";
    interfaceGlobs.menuItemName[60] = "Interface_MenuItem_TrainGeologist";
    interfaceGlobs.menuItemName[61] = "Interface_MenuItem_TrainPilot";
    interfaceGlobs.menuItemName[62] = "Interface_MenuItem_TrainSailor";
    interfaceGlobs.menuItemName[63] = "Interface_MenuItem_TrainDynamite";
    interfaceGlobs.menuItemName[64] = "Interface_MenuItem_UpgradeMan";
    interfaceGlobs.menuItemName[65] = "Interface_MenuItem_UpgradeBuilding";
    interfaceGlobs.menuItemName[66] = "Interface_MenuItem_UpgradeVehicle";
    interfaceGlobs.menuItemName[67] = "Interface_MenuItem_UpgradeEngine";
    interfaceGlobs.menuItemName[68] = "Interface_MenuItem_UpgardeDrill";
    interfaceGlobs.menuItemName[69] = "Interface_MenuItem_UpgardeScan";
    interfaceGlobs.menuItemName[70] = "Interface_MenuItem_UpgardeCarry";
    interfaceGlobs.menuItemName[71] = "Interface_MenuItem_Encyclopedia";
    interfaceGlobs.menuItemName[72] = "Interface_MenuItem_ClearSelection";
    interfaceGlobs.menuItemName[73] = "Interface_MenuItem_Build";

    Interface_AddAllMenuItems();
    Interface_SetScrollParameters(x, y, appWidth() + 10, y, 750.0f);
    Interface_ResetMenu();
}

void Interface_InitBuildItems()
{
    // TODO: Implement Interface_InitBuildItems
}

void Interface_LoadMenuItems(lpConfig config, const char* gameName)
{
    // TODO: Implement Interface_LoadMenuItems
}

void Interface_LoadItemPanels(lpConfig config, const char* gameName)
{
    // TODO: Implement Interface_LoadItemPanels
}

void Interface_LoadPlusMinusImages(const char* plusName, const char* minusName)
{
    interfaceGlobs.dependenciesPlusImage = Image_LoadBMPScaled(plusName, 0, 0);
    if (interfaceGlobs.dependenciesPlusImage != NULL)
        Image_SetupTrans(interfaceGlobs.dependenciesPlusImage, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f);

    interfaceGlobs.dependenciesMinusImage = Image_LoadBMPScaled(minusName, 0, 0);
    if (interfaceGlobs.dependenciesMinusImage != NULL)
        Image_SetupTrans(interfaceGlobs.dependenciesMinusImage, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f);
}

void Interface_FUN_0041b3c0()
{
    // TODO: Implement Interface_FUN_0041b3c0
}

void Interface_FUN_0041b860(F32 elapsedAbs)
{
    // TODO: Implement Interface_FUN_0041b860
}

void Interface_SelectBlock(Interface_MenuType menuType, Point2I* blockPos)
{
    if (interfaceGlobs.nextMenuType == Interface_Menu_Ground || interfaceGlobs.nextMenuType == Interface_Menu_Erode || interfaceGlobs.nextMenuType == Interface_Menu_Wall || interfaceGlobs.nextMenuType == Interface_Menu_Rubble || interfaceGlobs.nextMenuType == Interface_Menu_PlaceFence || interfaceGlobs.nextMenuType == Interface_Menu_Construction)
    {
        S32 bx = interfaceGlobs.highlightBlockPos.x;
        S32 by = interfaceGlobs.highlightBlockPos.y;

        WallHighlightType type = Map3D_GetBlockHighlight(Lego_GetMap(), bx, by);
        if (type == WALLHIGHLIGHT_NONE)
        {
            interfaceGlobs.origWallHighlight = WALLHIGHLIGHT_NONE;
        }
        else
        {
            Map3D_SetBlockHighlight(Lego_GetMap(), bx, by, interfaceGlobs.origWallHighlight);
        }
    }

    if (menuType == Interface_Menu_Ground || menuType == Interface_Menu_Erode || menuType == Interface_Menu_Wall || menuType == Interface_Menu_Rubble || menuType == Interface_Menu_PlaceFence || menuType == Interface_Menu_Construction)
    {
        S32 bx = blockPos->x;
        S32 by = blockPos->y;

        WallHighlightType oldHighlight = Map3D_SetBlockHighlight(Lego_GetMap(), bx, by, WALLHIGHLIGHT_SELECTED);

        if (oldHighlight != WALLHIGHLIGHT_SELECTED)
        {
            interfaceGlobs.origWallHighlight = oldHighlight;
        }
    }
}

void Interface_OpenMenu_FUN_0041b200(Interface_MenuType menuType, Point2I* blockPos)
{
    Lego_ClearSomeFlags3_FUN_00435950();
    Interface_SelectBlock(menuType, blockPos);
    Interface_UnkSlideOffScreen_FUN_0041e980_internal(menuType, blockPos);
}

void Interface_UnkSlideOffScreen_FUN_0041e980_internal(Interface_MenuType menuType, Point2I* optBlockPos)
{
    if ((interfaceGlobs.flags & INTERFACE_GLOB_FLAG_UNK_80) == INTERFACE_GLOB_FLAG_NONE || interfaceGlobs.currMenuType != menuType)
    {
        interfaceGlobs.flags &= ~INTERFACE_GLOB_FLAG_UNK_80 | INTERFACE_GLOB_FLAG_UNK_100;
        SFX_Random_PlaySoundNormal(SFX_InterfaceSlideOffScreen, FALSE);
    }
    else
    {
        interfaceGlobs.currMenuType = menuType;
        if (optBlockPos == NULL)
        {
            interfaceGlobs.nextMenuType = menuType;
            return;
        }

        interfaceGlobs.selBlockPos.x = optBlockPos->x;
        interfaceGlobs.selBlockPos.y = optBlockPos->y;
    }

    if (optBlockPos != NULL)
    {
        interfaceGlobs.highlightBlockPos.x = optBlockPos->x;
        interfaceGlobs.highlightBlockPos.y = optBlockPos->y;
    }

    interfaceGlobs.nextMenuType = menuType;
}

void Interface_BackToMain_IfSelectedWall_IsBlockPos(Point2I *blockPos)
{
    if (interfaceGlobs.currMenuType == Interface_Menu_Wall && interfaceGlobs.selBlockPos.x == blockPos->x && interfaceGlobs.selBlockPos.y == blockPos->y)
    {
        Interface_BackToMain();
    }
}

void Interface_BackToMain()
{
    Message_AddMessageAction(Message_ClearSelection, NULL, 0, NULL);
    Interface_OpenMenu_FUN_0041b200(Interface_Menu_Main, NULL);
}

void Interface_AddAllMenuItems()
{
    Interface_AddMenuItems(Interface_Menu_Main, 4, Interface_MenuItem_TeleportMan, Interface_MenuItem_BuildBuilding, Interface_MenuItem_BuildSmallVehicle, Interface_MenuItem_BuildLargeVehicle);
    Interface_AddMenuItems(Interface_Menu_Ground,3,Interface_MenuItem_LayPath,Interface_MenuItem_RemovePath,Interface_MenuItem_PlaceFence);
    Interface_AddMenuItems(Interface_Menu_Erode,1,Interface_MenuItem_RepairLava);
    Interface_AddMenuItems(Interface_Menu_PlaceFence,1,Interface_MenuItem_PlaceFence);
    Interface_AddMenuItems(Interface_Menu_Construction,1,Interface_MenuItem_CancelConstruction);
    Interface_AddMenuItems(Interface_Menu_Rubble,2,Interface_MenuItem_ClearRubble,Interface_MenuItem_PlaceFence);
    Interface_AddMenuItems(Interface_Menu_Wall,4,Interface_MenuItem_Dig,Interface_MenuItem_Reinforce,Interface_MenuItem_Dynamite,Interface_MenuItem_DeselectDig);
    Interface_AddMenuItems(Interface_Menu_Tracker,2,Interface_MenuItem_Attack,Interface_MenuItem_TrackObject);
    Interface_AddMenuItems(Interface_Menu_LandVehicle,7,Interface_MenuItem_UnLoadVehicle,Interface_MenuItem_VehiclePickUp,Interface_MenuItem_UpgradeVehicle,Interface_MenuItem_GetOut,Interface_MenuItem_GotoFirstPerson, Interface_MenuItem_GotoSecondPerson,Interface_MenuItem_DeleteVehicle);
    Interface_AddMenuItems(Interface_Menu_WaterVehicle,6,Interface_MenuItem_UnLoadVehicle,Interface_MenuItem_VehiclePickUp,Interface_MenuItem_GetOut,Interface_MenuItem_GotoFirstPerson, Interface_MenuItem_GotoSecondPerson,Interface_MenuItem_GotoDock);
    Interface_AddMenuItems(Interface_Menu_UnmannedVehicle,2,Interface_MenuItem_GetIn,Interface_MenuItem_DeleteVehicle);
    Interface_AddMenuItems(Interface_Menu_LegoMan,10,Interface_MenuItem_GoFeed,Interface_MenuItem_UnLoadMinifigure,Interface_MenuItem_MinifigurePickUp,Interface_MenuItem_GetTool,Interface_MenuItem_DropBirdScarer, Interface_MenuItem_UpgradeMan,Interface_MenuItem_TrainSkill,Interface_MenuItem_GotoFirstPerson,Interface_MenuItem_GotoSecondPerson,Interface_MenuItem_DeleteMan);
    Interface_AddMenuItems(Interface_Menu_Building,4,Interface_MenuItem_Repair,Interface_MenuItem_PowerOn,Interface_MenuItem_UpgradeBuilding,Interface_MenuItem_DeleteBuilding);
    Interface_AddMenuItems(Interface_Menu_ElectricFence,1,Interface_MenuItem_DeleteElectricFence);
    Interface_AddMenuItems(Interface_Menu_FP,3,Interface_MenuItem_GotoTopView,Interface_MenuItem_GotoSecondPerson,Interface_MenuItem_GotoFirstPerson);
    Interface_AddMenuItems(Interface_Menu_GetTool,8,Interface_MenuItem_GetDrill,Interface_MenuItem_GetSpade,Interface_MenuItem_GetHammer,Interface_MenuItem_GetSpanner,Interface_MenuItem_GetLaser, Interface_MenuItem_GetPusherGun,Interface_MenuItem_GetFreezerGun,Interface_MenuItem_GetBirdScarer);
    Interface_AddMenuItems(Interface_Menu_TrainSkill,6,Interface_MenuItem_TrainDriver,Interface_MenuItem_TrainEngineer,Interface_MenuItem_TrainGeologist,Interface_MenuItem_TrainPilot,Interface_MenuItem_TrainSailor, Interface_MenuItem_TrainDynamite);
    Interface_AddMenuItems(Interface_Menu_UpgradeVehicle,4,Interface_MenuItem_UpgradeEngine,Interface_MenuItem_UpgardeDrill,Interface_MenuItem_UpgardeScan,Interface_MenuItem_UpgardeCarry);
}

void Interface_AddMenuItems(Interface_MenuType menuType, U32 numItems, ...)
{
    va_list va;
    va_start(va, numItems);

    interfaceGlobs.menuList[menuType].iconCount = numItems;
    interfaceGlobs.menuList[menuType].iconList = (Interface_MenuItemType*)Mem_Alloc(numItems * sizeof(Interface_MenuItemType));

    for (S32 i = 0; i < numItems; i++)
    {
        interfaceGlobs.menuList[menuType].iconList[i] = va_arg(va, Interface_MenuItemType);
    }

    va_end(va);
}

void Interface_SetScrollParameters(U32 xEnd, U32 yEnd, U32 xStart, U32 yStart, F32 timerFloat_750)
{
    interfaceGlobs.slideStartPosition.x = (F32)xStart;
    interfaceGlobs.slideStartPosition.y = (F32)yStart;
    interfaceGlobs.slideEndPosition.x = (F32)xEnd;
    interfaceGlobs.slideEndPosition.y = (F32)yEnd;
    interfaceGlobs.slideSpeed = timerFloat_750 * 0.04f;
}

void Interface_ResetMenu()
{
    interfaceGlobs.currMenuType = Interface_Menu_Main;
    interfaceGlobs.currMenuPosition.x = interfaceGlobs.slideEndPosition.x;
    interfaceGlobs.flags |= (INTERFACE_GLOB_FLAG_UNK_4 | INTERFACE_GLOB_FLAG_UNK_80);
}
