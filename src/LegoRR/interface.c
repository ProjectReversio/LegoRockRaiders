#include "interface.h"

#include <stdio.h>

#include "aitask.h"
#include "encyclopedia.h"
#include "keys.h"
#include "lego.h"
#include "map3d.h"
#include "mem.h"
#include "message.h"
#include "panels.h"
#include "search.h"
#include "tooltip.h"
#include "utils.h"

Interface_Globs interfaceGlobs = { NULL };

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
    lpConfig conf = Config_FindArray(config, Config_BuildStringID(gameName, "InterfaceImages", 0));
    do
    {
        if (conf == NULL)
        {
            Interface_LoadBuildItems(config, gameName);
            Interface_LoadBackButton(config, gameName);
            return;
        }

        Interface_MenuItemType type;
        if (Interface_GetMenuItemType(conf->itemName, &type))
        {
            char buffer[1024];
            strcpy(buffer, conf->dataString);
            char* stringParts[20];
            B32 boolValue = 0;
            if (Util_Tokenize(buffer, stringParts, ":") == 7 && Util_GetBoolFromString(stringParts[6]) != BOOL3_ERROR)
                boolValue = Util_GetBoolFromString(stringParts[6]) == BOOL3_TRUE;

            interfaceGlobs.menuItemUnkBools[type] = boolValue;
            interfaceGlobs.menuItemIcons[type] = Image_LoadBMPScaled(stringParts[0], 0, 0);
            interfaceGlobs.menuItemIcons_no[type] = Image_LoadBMPScaled(stringParts[1], 0, 0);
            interfaceGlobs.menuItemIcons_pr[type] = Image_LoadBMPScaled(stringParts[2], 0, 0);

            const char* itemTexts[2];
            S32 num = Util_Tokenize(stringParts[3], itemTexts, "|");
            interfaceGlobs.langMenuItemTexts[type] = Util_StrCpy(itemTexts[0]);
            if (num == 2)
                SFX_GetType(itemTexts[1], &interfaceGlobs.sfxMenuItemTexts[type]);

            num = Util_Tokenize(stringParts[4], itemTexts, "|");
            interfaceGlobs.langMenuItemTexts_no[type] = Util_StrCpy(itemTexts[0]);
            if (num == 2)
                SFX_GetType(itemTexts[1], &interfaceGlobs.sfxMenuItemTexts_no[type]);

            Keys8 key;
            if (Key_Find(stringParts[5], &key))
                interfaceGlobs.menuItemF2keys[type] = key;
        }

        conf = Config_GetNextItem(conf);
    } while (TRUE);
}

void Interface_LoadItemPanels(lpConfig config, const char* gameName)
{
    for (S32 i = 0; i < INTERFACE_ICONPANEL_ITEM_COUNT; i++)
    {
        char str[20];
        _itoa(i + 1, str, 10);
        const char* interfaceSurroundImages = Config_GetTempStringValue(config, Config_BuildStringID(gameName, "InterfaceSurroundImages", str, 0));
        if (interfaceSurroundImages == NULL)
        {
            interfaceGlobs.iconPanelImages[i] = NULL;
        }
        else
        {
            char* stringParts[20];
            Util_Tokenize(interfaceSurroundImages, stringParts, ":");
            if (_stricmp(stringParts[0], "NULL") != 0)
            {
                interfaceGlobs.iconPanelImages[i] = Image_LoadBMPScaled(stringParts[0], 0, 0);
                if (interfaceGlobs.iconPanelImages[i] != NULL)
                    Image_SetupTrans(interfaceGlobs.iconPanelImages[i], 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f);

                interfaceGlobs.iconPanelIconOffsets[i].x = (F32)atoi(stringParts[1]);
                interfaceGlobs.iconPanelIconOffsets[i].y = (F32)atoi(stringParts[2]);

                // TODO: Verify this is correct
                interfaceGlobs.iconPanelBackButtonOffsets[i].x = (F32)atoi(stringParts[3]);
                interfaceGlobs.iconPanelBackButtonOffsets[i].y = (F32)atoi(stringParts[4]);
            }

            if (_stricmp(stringParts[5], "NULL") != 0)
            {
                // TODO: Verify this is correct
                interfaceGlobs.iconPanelNoBackImages[i] = Image_LoadBMPScaled(stringParts[5], 0, 0);
                if (interfaceGlobs.iconPanelNoBackImages[i] != NULL)
                    Image_SetupTrans(interfaceGlobs.iconPanelNoBackImages[i], 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f);

                // TODO: Verify this is correct
                interfaceGlobs.iconPanelNoBackIconOffsets[i].x = (F32)atoi(stringParts[6]);
                interfaceGlobs.iconPanelNoBackIconOffsets[i].y = (F32)atoi(stringParts[7]);
            }
        }
    }
}

void Interface_LoadBuildItems(lpConfig config, const char* gameName)
{
    // TODO: Implement Interface_LoadBuildItems
}

void Interface_LoadBackButton(lpConfig config, const char* gameName)
{
    const char* interfaceBackButton = Config_GetTempStringValue(config, Config_BuildStringID(gameName, "InterfaceBackButton", 0));
    if (interfaceBackButton == NULL)
        return;

    const char* stringParts[20];
    Util_Tokenize(interfaceBackButton, stringParts, ":");
    interfaceGlobs.backButtonSize.width = atoi(stringParts[0]);
    interfaceGlobs.backButtonSize.height = atoi(stringParts[1]);
    interfaceGlobs.backButtonImage_hl = Image_LoadBMPScaled(stringParts[2], 0, 0);
    interfaceGlobs.backButtonImage_pr = Image_LoadBMPScaled(stringParts[3], 0, 0);
    interfaceGlobs.backButtonText = Util_RemoveUnderscores(stringParts[4]);
    ToolTip_SetText(ToolTip_InterfaceMenuBackButton, interfaceGlobs.backButtonText);
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

B32 Interface_GetMenuItemType(const char* menuItemName, Interface_MenuItemType* outMenuItemType)
{
    S32 cmp;

    for (S32 i = 0; i < Interface_MenuItem_Type_Count; i++)
    {
        if (_stricmp(interfaceGlobs.menuItemName[i], menuItemName) == 0)
        {
            *outMenuItemType = i;
            return TRUE;
        }
    }
    return FALSE;
}

void Interface_FUN_0041b3c0()
{
    if ((interfaceGlobs.flags & INTERFACE_GLOB_FLAG_UNK_4) == INTERFACE_GLOB_FLAG_NONE)
        return;

    U32 buildMenuIconCount = Interface_GetBuildMenuIconCount(interfaceGlobs.currMenuType);
    if (!Interface_NotMainOrFirstPersonMenu(interfaceGlobs.currMenuType))
    {
        if (interfaceGlobs.iconPanelNoBackImages[buildMenuIconCount - 1] != NULL)
        {
            Point2F destPos;
            destPos.x = interfaceGlobs.iconPanelNoBackIconOffsets[buildMenuIconCount - 1].x + interfaceGlobs.currMenuPosition.x;
            destPos.y = interfaceGlobs.iconPanelNoBackIconOffsets[buildMenuIconCount - 1].y + interfaceGlobs.currMenuPosition.y;
            Image_DisplayScaled(interfaceGlobs.iconPanelNoBackImages[buildMenuIconCount - 1], NULL, &destPos, NULL);
        }
        goto endFunc;
    }

    if (interfaceGlobs.iconPanelImages[buildMenuIconCount - 1] != NULL)
    {
        Point2F destPos;
        destPos.x = interfaceGlobs.iconPanelIconOffsets[buildMenuIconCount - 1].x + interfaceGlobs.currMenuPosition.x;
        destPos.y = interfaceGlobs.iconPanelIconOffsets[buildMenuIconCount - 1].y + interfaceGlobs.currMenuPosition.y;
        Image_DisplayScaled(interfaceGlobs.iconPanelImages[buildMenuIconCount - 1], NULL, &destPos, NULL);
    }

    S32 flags = interfaceGlobs.menuItemFlags[0] & INTERFACE_MENUITEM_FLAG_FLASH;
    lpImage backImage = interfaceGlobs.backButtonImage_hl;
    if (!Interface_GetIconFlag8(Interface_MenuItem_Back))
    {
        // TODO: Verify this code is correct
        if ((interfaceGlobs.flags & INTERFACE_GLOB_FLAG_UNK_4000) == INTERFACE_GLOB_FLAG_NONE)
        {
            if (flags != INTERFACE_MENUITEM_FLAG_NONE)
            {
                if (interfaceGlobs.flashingState == 0)
                {
                    backImage = NULL;
                }
            }
            else
            {
                backImage = NULL;
            }
        }
        else if (flags != INTERFACE_MENUITEM_FLAG_NONE)
        {
            if (interfaceGlobs.flashingState == 0)
            {
                backImage = NULL;
            }
        }
    }
    else
    {
        backImage = interfaceGlobs.backButtonImage_pr;
        if (flags != INTERFACE_MENUITEM_FLAG_NONE && interfaceGlobs.flashingState)
            backImage = interfaceGlobs.backButtonImage_hl;
    }
    if (backImage != NULL)
    {
        Point2F destPos;
        destPos.x = interfaceGlobs.iconPanelBackButtonOffsets[buildMenuIconCount - 1].x + interfaceGlobs.currMenuPosition.x;
        destPos.y = interfaceGlobs.iconPanelBackButtonOffsets[buildMenuIconCount - 1].y + interfaceGlobs.currMenuPosition.y;
        Image_DisplayScaled(backImage, NULL, &destPos, NULL);
    }

    Interface_FUN_0041c420(Interface_MenuItem_Back, LegoObject_None, 0, NULL);
    Interface_SetIconFlag8(Interface_MenuItem_Back, FALSE);
    Interface_ChangeIconFlag1_FUN_0041c6e0(Interface_MenuItem_Back);

endFunc:
    Point2F destPos;
    destPos.x = interfaceGlobs.currMenuPosition.x;
    destPos.y = interfaceGlobs.currMenuPosition.y;
    Interface_FUN_0041b5b0(interfaceGlobs.currMenuType, Interface_Callback_FUN_0041b730, &destPos);

    if ((interfaceGlobs.flags & (INTERFACE_GLOB_FLAG_UNK_200 | INTERFACE_GLOB_FLAG_UNK_1000)) != INTERFACE_GLOB_FLAG_NONE)
    {
        Interface_DrawHoverOutline(&interfaceGlobs.areaf_fb4);
    }

    if ((interfaceGlobs.flags & (INTERFACE_GLOB_FLAG_UNK_1000 | INTERFACE_GLOB_FLAG_UNK_2000)) != INTERFACE_GLOB_FLAG_NONE)
    {
        Interface_FUN_0041ebd0(interfaceGlobs.areaf_fb4.x, interfaceGlobs.areaf_fb4.y);
        return;
    }

    Interface_SetFloat1494To25_AndUnsetFlags800();
}

void Interface_FUN_0041b860(F32 elapsedAbs)
{
    interfaceGlobs.timer_fc8 -= elapsedAbs;
    if (interfaceGlobs.timer_fc8 < 0.0f)
    {
        interfaceGlobs.flashingState = !interfaceGlobs.flashingState;
        interfaceGlobs.timer_fc8 = 15.0f;
    }

    Interface_FUN_0041e9f0(elapsedAbs);

    if (interfaceGlobs.float_1494 > 0.0f)
    {
        interfaceGlobs.float_1494 -= elapsedAbs;
        return;
    }

    interfaceGlobs.flags |= INTERFACE_GLOB_FLAG_UNK_800;
}

void Interface_FUN_0041e9f0(F32 elapsedAbs)
{
    if ((interfaceGlobs.flags & INTERFACE_GLOB_FLAG_UNK_80) != INTERFACE_GLOB_FLAG_NONE)
        return;

    if ((interfaceGlobs.flags & INTERFACE_GLOB_FLAG_UNK_100) == INTERFACE_GLOB_FLAG_NONE)
    {
        if ((interfaceGlobs.flags & INTERFACE_GLOB_FLAG_UNK_4) != INTERFACE_GLOB_FLAG_NONE &&
            (interfaceGlobs.currMenuPosition.x = interfaceGlobs.currMenuPosition.x - elapsedAbs * interfaceGlobs.slideSpeed,
                interfaceGlobs.currMenuPosition.x <= interfaceGlobs.slideEndPosition.x))
        {
            interfaceGlobs.flags |= INTERFACE_GLOB_FLAG_UNK_80;
            interfaceGlobs.currMenuPosition.x = interfaceGlobs.slideEndPosition.x;
        }
    }
    else
    {
        interfaceGlobs.currMenuPosition.x += interfaceGlobs.slideSpeed * elapsedAbs;
        if (interfaceGlobs.currMenuType == Interface_Menu_Type_Count && (panelGlobs.panelTable[8].flags & PANELDATA_FLAG_UNK2) != PANELDATA_FLAG_NONE)
        {
            Panel_ChangeFlags_BasedOnState(Panel_PriorityList);
            // TODO: Figure out which flag "1" is, might be PanelButton_TopPanel_Priorities or PanelButton_PriorityList_Disable2
            Panel_Button_SetFlag_8_OrUnset_c(Panel_TopPanel, 1, FALSE);
            interfaceGlobs.currMenuPosition.x = interfaceGlobs.slideStartPosition.x;
        }

        if (interfaceGlobs.slideStartPosition.x <= interfaceGlobs.currMenuPosition.x)
        {
            interfaceGlobs.flags &= ~INTERFACE_GLOB_FLAG_UNK_100;
            interfaceGlobs.currMenuPosition.x = interfaceGlobs.slideStartPosition.x;
            SFX_Random_PlaySoundNormal(SFX_InterfaceSlideOnScreen, FALSE);
            if (interfaceGlobs.nextMenuType == Interface_Menu_Type_Count)
            {
                interfaceGlobs.flags &= ~INTERFACE_GLOB_FLAG_UNK_4;
                interfaceGlobs.flags |= INTERFACE_GLOB_FLAG_UNK_80;
                Panel_ChangeFlags_BasedOnState(Panel_PriorityList);
                Panel_UnsetFlag_6();
                panelGlobs.flags |= PANEL_GLOB_FLAG_UNK_2;
            }
            else if (interfaceGlobs.nextMenuType == Interface_Menu_Encyclopedia)
            {
                interfaceGlobs.flags &= ~INTERFACE_GLOB_FLAG_UNK_4;
                interfaceGlobs.flags |= INTERFACE_GLOB_FLAG_UNK_80;
                Panel_ChangeFlags_BasedOnState(Panel_Encyclopedia);
                legoGlobs.flags3 |= GAME3_UNK_1;
                Encyclopedia_UnsetFlag1();
            }
            else
            {
                interfaceGlobs.flags |= INTERFACE_GLOB_FLAG_UNK_4;
            }

            interfaceGlobs.currMenuType = interfaceGlobs.nextMenuType;
            interfaceGlobs.selBlockPos.x = interfaceGlobs.highlightBlockPos.x;
            interfaceGlobs.selBlockPos.y = interfaceGlobs.highlightBlockPos.y;
        }
    }
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
        interfaceGlobs.flags &= ~INTERFACE_GLOB_FLAG_UNK_80;
        interfaceGlobs.flags |= INTERFACE_GLOB_FLAG_UNK_100;
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

B32 Interface_FUN_0041b5b0(Interface_MenuType menuIcon, Interface_Callback_Unknown callback, void* context)
{
    if (menuIcon == Interface_Menu_BuildBuilding)
    {
        // TODO: Implement Interface_FUN_0041b5b0
    }
    else if (menuIcon == Interface_Menu_BuildSmallVehicle)
    {
        // TODO: Implement Interface_FUN_0041b5b0
    }
    else if (menuIcon == Interface_Menu_BuildLargeVehicle)
    {
        // TODO: Implement Interface_FUN_0041b5b0
    }
    else if (interfaceGlobs.menuList[menuIcon].iconCount != 0)
    {
        S32 i = 0;
        while (TRUE)
        {
            Interface_MenuItemType menuItem = interfaceGlobs.menuList[menuIcon].iconList[i];
            if (menuItem == Interface_MenuItem_PowerOn || menuItem == Interface_MenuItem_PowerOff)
            {
                menuItem = Interface_GetPrimaryUnit_PowerIcon(&menuItem);
            }

            if (callback(menuItem, 0, 0, context))
                break;

            i++;
            if (interfaceGlobs.menuList[menuIcon].iconCount <= i)
                return FALSE;
        }

        return TRUE;
    }

    return FALSE;
}

B32 Interface_Callback_FUN_0041b730(Interface_MenuType menuIcon, LegoObject_Type objType, LegoObject_ID objID, void* context)
{
    Point2F* destPos = context;

    lpImage image;
    Interface_MenuItemFlags flags;

    if (menuIcon == Interface_MenuItem_Build)
    {
        Interface_ChangeObjectIconFlag1_FUN_0041c730(objType, objID);
        image = Interface_GetBuildImageByObjectType(objType, objID);
        if (objType == LegoObject_Vehicle)
        {
            flags = interfaceGlobs.vehicleItemFlags[objID] & INTERFACE_MENUITEM_FLAG_FLASH;
        }
        else
        {
            flags = objType; // TODO: What?
            if (objType == LegoObject_Building)
                flags = interfaceGlobs.buildingItemFlags[objID] & INTERFACE_MENUITEM_FLAG_FLASH;
        }

        Interface_SetObjectIconFlag8(objType, objID, FALSE);
    }
    else
    {
        Interface_ChangeIconFlag1_FUN_0041c6e0(menuIcon);
        image = Interface_FUN_0041c9e0(menuIcon);
        flags = interfaceGlobs.menuItemFlags[menuIcon] & INTERFACE_MENUITEM_FLAG_FLASH;
        Interface_SetIconFlag8(menuIcon, FALSE);
    }
    Interface_FUN_0041c420(menuIcon, objType, objID, destPos);

    F32 yOffset;
    if (image == NULL)
    {
        yOffset = 40;
    }
    else
    {
        yOffset = image->height;
        if (flags == INTERFACE_MENUITEM_FLAG_NONE || interfaceGlobs.flashingState != 0)
            Image_DisplayScaled(image, NULL, destPos, NULL);
    }

    if (menuIcon == Interface_MenuItem_TeleportMan)
    {
        objID = 0;
        objType = LegoObject_MiniFigure;
    }
    else if (menuIcon != Interface_MenuItem_Build)
    {
        destPos->y += yOffset;
        return FALSE;
    }

    Interface_DrawTeleportQueueNumber(objType, objID, destPos);

    destPos->y += yOffset;
    return FALSE;
}

U32 Interface_GetBuildMenuIconCount(Interface_MenuType interfaceMenuType)
{
    U32 iconCount = 0;

    if (interfaceMenuType == Interface_Menu_BuildBuilding)
        return legoGlobs.buildingCount;

    if (interfaceMenuType == Interface_Menu_BuildSmallVehicle)
    {
        // TODO: Implement Interface_GetBuildMenuIconCount
    }
    else
    {
        if (interfaceMenuType != Interface_Menu_BuildLargeVehicle)
            return interfaceGlobs.menuList[interfaceMenuType].iconCount;

        // TODO: Implement Interface_GetBuildMenuIconCount
    }

    return iconCount;
}

B32 Interface_NotMainOrFirstPersonMenu(Interface_MenuType interfaceMenuType)
{
    if (interfaceMenuType != Interface_Menu_Main && interfaceMenuType != Interface_Menu_FP)
        return TRUE;

    return FALSE;
}

void Interface_SetFloat1494To25_AndUnsetFlags800()
{
    interfaceGlobs.float_1494 = 25.0f;
    interfaceGlobs.flags &= ~INTERFACE_GLOB_FLAG_UNK_800;
}

void Interface_FUN_0041ebd0(F32 xScreen, F32 yScreen)
{
    // TODO: Implement Interface_FUN_0041ebd0
}

B32 Interface_GetIconFlag8(Interface_MenuItemType menuIcon)
{
    return interfaceGlobs.menuItemFlags[menuIcon] & INTERFACE_MENUITEM_FLAG_UNK_8;
}

void Interface_SetIconFlag8(Interface_MenuItemType menuIcon, B32 on)
{
    if (on)
    {
        interfaceGlobs.menuItemFlags[menuIcon] |= INTERFACE_MENUITEM_FLAG_UNK_8;
    }
    else
    {
        interfaceGlobs.menuItemFlags[menuIcon] &= ~INTERFACE_MENUITEM_FLAG_UNK_8;
    }
}

void Interface_SetObjectIconFlag8(LegoObject_Type objType, LegoObject_ID objID, B32 on)
{
    // TODO: Implement Interface_SetObjectIconFlag8
}

void Interface_FUN_0041c420(Interface_MenuItemType menuIcon, LegoObject_Type objType, LegoObject_ID objID, Point2F* destPos)
{
    // TODO: Implement Interface_FUN_0041c420
}

void Interface_ChangeObjectIconFlag1_FUN_0041c730(LegoObject_Type objType, LegoObject_ID objID)
{
    // TODO: Implement Interface_ChangeObjectIconFlag1_FUN_0041c730
}

void Interface_ChangeIconFlag1_FUN_0041c6e0(Interface_MenuItemType menuIcon)
{
    Interface_MenuItemFlags flags = interfaceGlobs.menuItemFlags[menuIcon];
    if ((flags & INTERFACE_MENUITEM_FLAG_UNK_2) == INTERFACE_MENUITEM_FLAG_NONE && ((interfaceGlobs.flags & INTERFACE_GLOB_FLAG_UNK_80) != INTERFACE_GLOB_FLAG_NONE))
    {
        flags = interfaceGlobs.menuItemFlags[menuIcon];
        if (Interface_HandleMenuItem(menuIcon))
        {
            interfaceGlobs.menuItemFlags[menuIcon] = flags & ~INTERFACE_MENUITEM_FLAG_UNK_1;
            return;
        }
    }

    interfaceGlobs.menuItemFlags[menuIcon] = flags | INTERFACE_MENUITEM_FLAG_UNK_1;
}

B32 Interface_HandleMenuItem(Interface_MenuItemType menuIcon)
{
    // TODO: Implement Interface_HandleMenuItem

    return TRUE;
}

Interface_MenuItemType Interface_GetPrimaryUnit_PowerIcon(Interface_MenuItemType* menuIcon)
{
    // TODO: Implement Interface_GetPrimaryUnit_PowerIcon

    return *menuIcon;
}

B32 Interface_GetIconUnk_FUN_0041c820(Interface_MenuItemType menuIcon, B32 param2)
{
    // TODO: Implement Interface_GetIconUnk_FUN_0041c820

    return FALSE;
}

lpImage Interface_GetBuildImageByObjectType(LegoObject_Type objType, LegoObject_ID objID)
{
    // TODO: Implement Interface_GetBuildImageByObjectType

    return NULL;
}

lpImage Interface_FUN_0041c9e0(Interface_MenuItemType menuIcon)
{
    if (Interface_GetIconUnk_FUN_0041c820(menuIcon, FALSE))
        return interfaceGlobs.menuItemIcons_no[menuIcon];

    if (Interface_GetIconFlag8(menuIcon))
        return interfaceGlobs.menuItemIcons_pr[menuIcon];

    return interfaceGlobs.menuItemIcons[menuIcon];
}

B32 Interface_DoSomethingWithRenameReplace(U32 mouseX, U32 mouseY, B32 leftButton, B32 leftButtonLast, B32 leftReleased)
{
    if ((interfaceGlobs.flags & INTERFACE_GLOB_FLAG_UNK_4) == INTERFACE_GLOB_FLAG_NONE ||
        (interfaceGlobs.flags & INTERFACE_GLOB_FLAG_UNK_80) == INTERFACE_GLOB_FLAG_NONE)
    {
        interfaceGlobs.flags &= ~(INTERFACE_GLOB_FLAG_UNK_4000 | INTERFACE_GLOB_FLAG_UNK_2000 | INTERFACE_GLOB_FLAG_UNK_1000 | INTERFACE_GLOB_FLAG_UNK_400 | INTERFACE_GLOB_FLAG_UNK_200);
        return 0;
    }

    interfaceGlobs.flags &= ~(INTERFACE_GLOB_FLAG_UNK_4000 | INTERFACE_GLOB_FLAG_UNK_2000 | INTERFACE_GLOB_FLAG_UNK_1000 | INTERFACE_GLOB_FLAG_UNK_400 | INTERFACE_GLOB_FLAG_UNK_200);

    LegoObject_Type objType;
    LegoObject_ID objID;
    U32 someNum;
    if (Interface_FUN_0041edb0(mouseX, mouseY, interfaceGlobs.areaf_fb4.x, interfaceGlobs.areaf_fb4.y, &objType, &objID, &someNum))
    {
        interfaceGlobs.flags |= INTERFACE_GLOB_FLAG_UNK_1000;
        if (!leftButton && !leftReleased && objType != LegoObject_Type_Count)
        {
            char buffer[512];
            const char* name = legoGlobs.langUpgradeLevel_name[someNum];
            if (name == NULL)
            {
                sprintf(buffer, "%s", Object_GetLangName(objType, objID));
            }
            else
            {
                sprintf(buffer, "%s (%s)", Object_GetLangName(objType, objID), name);
            }
            ToolTip_SetText(ToolTip_InterfaceMenu, buffer);
            ToolTip_AddFlag4(ToolTip_InterfaceMenu);
            ToolTip_ResetTimer(ToolTip_InterfaceMenu);
            if ((legoGlobs.flags2 & GAME2_DISABLETOOLTIPSOUND) == GAME2_NONE)
            {
                Interface_SetSFX_004df1f4(objectGlobs.objectTtSFX[objType][objID]);
            }
        }

        Interface_FUN_0041c610(Interface_MenuItem_Back, 0, 0, leftButton, leftButtonLast);
        return TRUE;
    }

    Interface_MenuItemType menuItemType;
    LegoObject_Type objType2;
    LegoObject_ID objID2;
    if (!Interface_FUN_0041c0f0(mouseX, mouseY, &menuItemType, &objType2, &objID2))
    {
        lpImage image;
        U32 xDiff;
        U32 yDiff;
        U32 count = Interface_GetBuildMenuIconCount(interfaceGlobs.currMenuType);
        if (!Interface_NotMainOrFirstPersonMenu(interfaceGlobs.currMenuType))
        {
            image = interfaceGlobs.iconPanelNoBackImages[count - 1];
            xDiff = interfaceGlobs.iconPanelNoBackIconOffsets[count - 1].x + interfaceGlobs.currMenuPosition.x;
            yDiff = interfaceGlobs.iconPanelNoBackIconOffsets[count - 1].y + interfaceGlobs.currMenuPosition.y;
        }
        else
        {
            image = interfaceGlobs.iconPanelImages[count - 1];
            xDiff = interfaceGlobs.iconPanelIconOffsets[count - 1].x + interfaceGlobs.currMenuPosition.x;
            yDiff = interfaceGlobs.iconPanelIconOffsets[count - 1].y + interfaceGlobs.currMenuPosition.y;
        }

        if (image == NULL)
        {
            return FALSE;
        }

        U32 pixel;
        if (!Image_GetPixel(image, mouseX - xDiff, mouseY - yDiff, &pixel))
        {
            return FALSE;
        }

        if (pixel == 0)
        {
            return FALSE;
        }

        if (leftReleased)
        {
            Lego_SetPointerSFX(PointerSFX_NotOkay);
        }

        return TRUE;
    }

    if (menuItemType == Interface_MenuItem_Back)
    {
        interfaceGlobs.flags |= INTERFACE_GLOB_FLAG_UNK_4000;
        ToolTip_AddFlag4(ToolTip_InterfaceMenuBackButton);
        goto endFunc;
    }

    const char* someString = NULL;
    S32 oreCount = 0;

    if (menuItemType == Interface_MenuItem_Build)
    {
        // TODO: Implement Interface_DoSomethingWithRenameReplace
    }
    else
    {
        S32 varFlag;
        if (menuItemType == Interface_MenuItem_LayPath ||
            (varFlag = interfaceGlobs.flags | INTERFACE_GLOB_FLAG_UNK_200, menuItemType == Interface_MenuItem_PlaceFence))
        {
            varFlag = interfaceGlobs.flags | (INTERFACE_GLOB_FLAG_UNK_2000 | INTERFACE_GLOB_FLAG_UNK_200);
        }

        interfaceGlobs.flags = varFlag;

        // TODO: Implement Interface_DoSomethingWithRenameReplace
    }

    if (!leftButton && !leftReleased)
    {
        if (someString == NULL)
        {
            char buffer[512];
            sprintf(buffer, "Missing config for\n\'%s\'", interfaceGlobs.menuItemName[menuItemType]);
            ToolTip_SetText(ToolTip_InterfaceMenu, buffer);
        }
        else
        {
            ToolTip_SetText(ToolTip_InterfaceMenu, someString);
            for (S32 i = oreCount; i != 0; i--)
            {
                ToolTip_AddIcon(ToolTip_InterfaceMenu, objectGlobs.ToolTipIcon_Ore);
            }
        }

        ToolTip_AddFlag4(ToolTip_InterfaceMenu);
        ToolTip_ResetTimer(ToolTip_InterfaceMenu);
    }
    else
    {
        Interface_SetSFX_004df1f4(SFX_NULL);
    }

endFunc:

    if (Interface_FUN_0041c610(menuItemType, objType2, objID2, leftButton, leftButtonLast) && leftReleased)
    {
        if (menuItemType == Interface_MenuItem_Build)
        {
            // TODO: Implement Interface_DoSomethingWithRenameReplace
        }
        else
        {
            interfaceGlobs.menuItemClicks[menuItemType]++;
        }

        B32 var = Interface_DoAction_FUN_0041dbd0(menuItemType);
        Lego_SetPointerSFX(!var);
    }

    return TRUE;
}

B32 Interface_FUN_0041c0f0(U32 mouseX, U32 mouseY, Interface_MenuItemType* outMenuItemType, LegoObject_Type* outObjType, LegoObject_ID* outObjID)
{
    SearchInterfaceUnknown search;

    U32 count = Interface_GetBuildMenuIconCount(interfaceGlobs.currMenuType);
    if (Interface_NotMainOrFirstPersonMenu(interfaceGlobs.currMenuType))
    {
        U32 xDiff = interfaceGlobs.iconPanelBackButtonOffsets[count - 1].x + interfaceGlobs.currMenuPosition.x;
        U32 yDiff = interfaceGlobs.iconPanelBackButtonOffsets[count - 1].y + interfaceGlobs.currMenuPosition.y;

        if (xDiff <= mouseX && mouseX <= interfaceGlobs.backButtonSize.width + xDiff &&
            yDiff <= mouseY && mouseY <= interfaceGlobs.backButtonSize.height + yDiff)
        {
            *outMenuItemType = Interface_MenuItem_Back;
            return TRUE;
        }
    }

    search.currMenuPos.x = interfaceGlobs.currMenuPosition.x;
    search.currMenuPos.y = interfaceGlobs.currMenuPosition.y;
    search.mouseX = mouseX;
    search.mouseY = mouseY;

    if (!Interface_FUN_0041b5b0(interfaceGlobs.currMenuType, Interface_Callback_FUN_0041c240, &search))
    {
        return FALSE;
    }

    *outMenuItemType = search.menuItemType;
    *outObjType = search.objType;
    *outObjID = search.objID;

    return TRUE;
}

void Interface_SetSFX_004df1f4(SFX_ID sfxType)
{
    interfaceGlobs.sfxType_149c = sfxType;
}

B32 Interface_FUN_0041edb0(U32 mouseX, U32 mouseY, F32 x, F32 y, LegoObject_Type *outObjType, LegoObject_ID* outObjID, U32* outObjLevel)
{
    // TODO: Implement Interface_FUN_0041edb0

    return FALSE;
}

B32 Interface_FUN_0041c610(Interface_MenuItemType menuIcon, LegoObject_Type objType, LegoObject_ID objID, B32 leftButton, B32 leftButtonLast)
{
    Interface_MenuItemType type = interfaceGlobs.menuItemType_fa8;
    if (!leftButton)
    {
        interfaceGlobs.menuItemType_fa8 = Interface_MenuItem_Type_Count;
    }
    else if ((interfaceGlobs.flags & INTERFACE_GLOB_FLAG_UNK_1000) == INTERFACE_GLOB_FLAG_NONE)
    {
        if (interfaceGlobs.menuItemType_fa8 == Interface_MenuItem_Type_Count)
        {
            if (leftButtonLast)
                goto nLabel;

            interfaceGlobs.menuItemType_fa8 = menuIcon;
            interfaceGlobs.field_fac = objType;
            interfaceGlobs.field_fb0 = objID;

            if (menuIcon == Interface_MenuItem_Build)
            {
                Interface_SetObjectIconFlag8(objType, objID, TRUE);
                goto nLabel;
            }
        }
        else
        {
            if (interfaceGlobs.menuItemType_fa8 != menuIcon)
                goto nLabel;

            if (menuIcon == Interface_MenuItem_Build)
            {
                if (interfaceGlobs.field_fac == objType && interfaceGlobs.field_fb0 == objID)
                {
                    Interface_SetObjectIconFlag8(objType, objID, TRUE);
                }
                goto nLabel;
            }
        }

        Interface_SetIconFlag8(menuIcon, TRUE);
    }

nLabel:
    if (type == menuIcon)
    {
        if (menuIcon != Interface_MenuItem_Build)
            return TRUE;
        if (interfaceGlobs.field_fac == objType && interfaceGlobs.field_fb0 == objID)
            return TRUE;
    }

    return FALSE;
}

B32 Interface_Callback_FUN_0041c240(Interface_MenuItemType menuIcon, LegoObject_Type objType, LegoObject_ID objID, void* context)
{
    SearchInterfaceUnknown* search = context;

    lpImage image;
    if (menuIcon == Interface_MenuItem_Build)
    {
        image = Interface_GetBuildImageByObjectType(objType, objID);
    }
    else
    {
        image = Interface_FUN_0041c9e0(menuIcon);
    }

    U32 x;
    U32 y;
    if (image == NULL)
    {
        x = 40;
        y = 40;
    }
    else
    {
        x = image->width;
        y = image->height;
    }

    if (search->currMenuPos.x <= search->mouseX && search->mouseX <= x + search->currMenuPos.x &&
        search->currMenuPos.y <= search->mouseY && search->mouseY <= y + search->currMenuPos.y)
    {
        search->menuItemType = menuIcon;
        Interface_SetDat_004decd8_004decdc(menuIcon, objType, objID);
        if (menuIcon == Interface_MenuItem_Build)
        {
            search->objType = objType;
            search->objID = objID;
        }

        Interface_FUN_0041cc10(&search->currMenuPos, x - 1, y - 1);
        return TRUE;
    }

    search->currMenuPos.y += y;
    return FALSE;
}

B32 Interface_DoAction_FUN_0041dbd0(Interface_MenuItemType menuIcon)
{
    lpLegoObject obj = Message_GetPrimarySelectedUnit();
    if (Interface_GetIconUnk_FUN_0041c820(menuIcon, TRUE))
    {
        return FALSE;
    }

    switch (menuIcon)
    {
        // TODO: Implement Interface_DoAction_FUN_0041dbd0
        case Interface_MenuItem_Dig:
        {
            AITask_DoDig_AtBlockPos(&interfaceGlobs.selBlockPos, FALSE, TRUE);

            interfaceGlobs.origWallHighlight = Map3D_GetBlockHighlight(Lego_GetMap(), interfaceGlobs.selBlockPos.x, interfaceGlobs.selBlockPos.y);
            Interface_BackToMain();
            return TRUE;
        }
        // TODO: Implement Interface_DoAction_FUN_0041dbd0
    }

    Interface_BackToMain();
    return TRUE;
}

void Interface_SetDat_004decd8_004decdc(Interface_MenuItemType menuIcon, LegoObject_Type objType, LegoObject_ID objID)
{
    // TODO: Implement Interface_SetDat_004decd8_004decdc
}

void Interface_FUN_0041cc10(Point2F* point, U32 unkWidth, U32 unkHeight)
{
    interfaceGlobs.areaf_fb4.x = point->x;
    interfaceGlobs.areaf_fb4.y = point->y;
    interfaceGlobs.areaf_fb4.width = (F32)unkWidth;
    interfaceGlobs.areaf_fb4.height = (F32)unkHeight;
}

void Interface_FUN_0041b940(F32 elapsedAbs)
{
    // TODO: Implement Interface_FUN_0041b940
}

void Interface_DrawTeleportQueueNumber(LegoObject_Type objType, LegoObject_ID objID, Point2F* screenPt)
{
    // TODO: Implement Interface_DrawTeleportQueueNumber
}

// Draws a green 2-pixel width rectangle around the specified rect (drawing 1 and 2 pixels outside
// of area).
//
// Passed area must subtract 1 from width/height so that there isn't a gap between the edge and the
// start of the border on the bottom/right side.
void Interface_DrawHoverOutline(Area2F* area)
{
    Point2F rectLines[5];
    if ((legoGlobs.flags1 & GAME1_FREEZEINTERFACE) == GAME1_NONE)
    {
        // Draw for rect expanded by 1 pixel.
        // Then draw for rect expanded by 2 pixels, for a 2-pixel-width rectangle
        // outline.
        rectLines[0].x = area->x - 1.0f;
        rectLines[0].y = area->y - 1.0f;
        rectLines[1].x = (area->width + area->x) + 1.0f;
        rectLines[1].y = rectLines[0].y;
        rectLines[2].x = rectLines[1].x;
        rectLines[2].y = (area->height + area->y) + 1.0f;
        rectLines[3].x = rectLines[0].x;
        rectLines[3].y = rectLines[2].y;
        rectLines[4].x = rectLines[0].x;
        rectLines[4].y = rectLines[0].y;
        Draw_LineListEx(rectLines, rectLines + 1, 4, 0.0f, 1.0f, 0.0f, DrawEffect_None);

        rectLines[0].x = area->x - 2.0f;
        rectLines[0].y = area->y - 2.0f;
        rectLines[1].x = (area->width + area->x) + 2.0f;
        rectLines[1].y = rectLines[0].y;
        rectLines[2].x = rectLines[1].x;
        rectLines[2].y = (area->height + area->y) + 2.0f;
        rectLines[3].x = rectLines[0].x;
        rectLines[3].y = rectLines[2].y;
        rectLines[4].x = rectLines[0].x;
        rectLines[4].y = rectLines[0].y;
        Draw_LineListEx(rectLines, rectLines + 1, 4, 0.0f, 1.0f, 0.0f, DrawEffect_None);
    }
}
