#include "interface.h"

#include "lego.h"
#include "map3d.h"

Interface_Globs interfaceGlobs = {};

void Interface_Initialize(U32 x, U32 y, lpFont font)
{
    // TODO: Implement Interface_Initialize
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
    // TODO: Implement Interface_LoadPlusMinusImages
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
