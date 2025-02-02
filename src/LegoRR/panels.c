#include "panels.h"
#include "flic.h"

Panel_Globs panelGlobs;

void Panel_Initialize()
{
    panelGlobs.currentPanel = Panel_Type_Count;
    panelGlobs.currentButton = PanelButton_Type_Count;

    Panel_RegisterName(Panel_Radar);
    Panel_RegisterName(Panel_RadarFill);
    Panel_RegisterName(Panel_RadarOverlay);
    Panel_RegisterName(Panel_Messages);
    Panel_RegisterName(Panel_MessagesSide);
    Panel_RegisterName(Panel_CrystalSideBar);
    Panel_RegisterName(Panel_TopPanel);
    Panel_RegisterName(Panel_Information);
    Panel_RegisterName(Panel_PriorityList);
    Panel_RegisterName(Panel_CameraControl);
    Panel_RegisterName(Panel_InfoDock);
    Panel_RegisterName(Panel_Encyclopedia);

    // TODO: Implement Panel_Initialize
}

void Panel_RotationControl_Initialize(lpConfig config, const char* gameName)
{
    // TODO: Implement Panel_RotationControl_Initialize
}

void Panel_Crystals_Initialize(const char* smallCrystal, const char* usedCrystal, const char* noSmallCrystal)
{
    // TODO: Implement Panel_Crystals_Initialize
}

void Panel_AirMeter_Initialize(const char* airJuiceName, U32 juiceX, U32 juiceY, U32 juiceLength, const char* noAirName, U32 noAirX, U32 noAirY)
{
    // TODO: Implement Panel_AirMeter_Initialize
}

void Panel_CryOreSideBar_Initialize(const char* sidebarName, U32 xPos, U32 yPos, U32 meterOffset)
{
    panelGlobs.cryOreSideBarImage = Image_LoadBMP(sidebarName);
    panelGlobs.cryOreSideBarOffset.x = (F32)xPos;
    panelGlobs.cryOreSideBarOffset.y = (F32)yPos;
    panelGlobs.cryOreMeterOffset = meterOffset;
}

void Panel_Button_SetFlags_10(Panel_Type panelType, PanelButton_Type buttonType, B32 state)
{
    // TODO: Implement Panel_Button_SetFlags_10
}

void Panel_LoadInterfaceButtons_ScrollInfo()
{
    // TODO: Implement Panel_LoadInterfaceButtons_ScrollInfo
}

B32 Panel_GetPanelType(const char* panelName, Panel_Type* panelType)
{
    for (S32 i = 0; i < Panel_Type_Count; i++)
    {
        if (_stricmp(panelName, panelGlobs.panelName[i]) != 0)
            continue;

        *panelType = (Panel_Type)i;
        return TRUE;
    }

    return FALSE;
}

void Panel_LoadImage(const char* filename, Panel_Type panelType, PanelDataFlags flags)
{
    panelGlobs.panelTable[panelType].buttonCount = 0;
    panelGlobs.panelTable[panelType].flags = flags;
    panelGlobs.panelTable[panelType].imageOrFlic = Image_LoadBMP(filename);

    if (panelGlobs.panelTable[panelType].imageOrFlic != NULL)
    {
        panelGlobs.panelTable[panelType].isFlic = FALSE;
        Image_SetupTrans(panelGlobs.panelTable[panelType].imageOrFlic, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f);
        panelGlobs.panelTable[panelType].flags |= PANELDATA_FLAG_UNK1;
        return;
    }

    if (Flic_Setup(filename, (lpFlic*)&panelGlobs.panelTable[panelType].imageOrFlic, FLICMEMORY | FLICLOOPINGON))
    {
        panelGlobs.panelTable[panelType].isFlic = TRUE;
        panelGlobs.panelTable[panelType].flags |= PANELDATA_FLAG_UNK1;
    }
}

void Panel_SetArea(Panel_Type panelType, S32 xOut, S32 yOut, S32 xIn, S32 yIn)
{
    panelGlobs.panelTable[panelType].xyOut.x = xOut;
    panelGlobs.panelTable[panelType].xyIn.x = xIn;
    panelGlobs.panelTable[panelType].xyOut.y = yOut;
    panelGlobs.panelTable[panelType].xyIn.y = yIn;
    panelGlobs.panelTable[panelType].field_20 = 0;
    if ((panelGlobs.panelTable[panelType].flags & PANELDATA_FLAG_UNK2) != PANELDATA_FLAG_NONE)
    {
        panelGlobs.panelTable[panelType].xyOutIn.x = xOut;
        panelGlobs.panelTable[panelType].xyOutIn.y = yOut;
        return;
    }

    panelGlobs.panelTable[panelType].xyOutIn.x = xIn;
    panelGlobs.panelTable[panelType].xyOutIn.y = yIn;
}

void Panel_DrawPanel(Panel_Type panelType, F32 elapsedAbs)
{
    // TODO: Implement Panel_FUN_0045a9f0

    B32 isFlic = panelGlobs.panelTable[panelType].isFlic;
    if (isFlic)
    {
        // TODO: Implement Panel_FUN_0045a9f0
    }
    else
    {
        Point2F pos;
        pos.x = floor((F64)panelGlobs.panelTable[panelType].xyOutIn.x);
        pos.y = floor((F64)panelGlobs.panelTable[panelType].xyOutIn.y);
        Image_DisplayScaled(panelGlobs.panelTable[panelType].imageOrFlic, NULL, &pos, NULL);
    }

    if (panelType == Panel_CameraControl)
    {
        // TODO: Implement Panel_FUN_0045a9f0
    }

    // TODO: Implement Panel_FUN_0045a9f0
}

void Panel_Crystals_Draw(U32 crystals, U32 usedCrystals, F32 elapsedGame)
{
    // TODO: Implement Panel_Crystals_Draw
}

void Panel_CryOreSideBar_ChangeOreMeter(B32 increment, U32 amount)
{
    // TODO: Implement Panel_CryOreSideBar_ChangeOreMeter
}

void Panel_CryOreSideBar_Draw()
{
    if (panelGlobs.cryOreSideBarImage != NULL && panelGlobs.cryOreMeterValue > 0.0f)
    {
        Size2F size;
        Point2F pos;

        size.width = (F32)panelGlobs.cryOreSideBarImage->width;
        size.height = (F32)panelGlobs.cryOreMeterOffset * panelGlobs.cryOreMeterValue;
        pos.x = panelGlobs.cryOreSideBarOffset.x;
        pos.y = panelGlobs.cryOreSideBarOffset.y - size.height;

        Image_DisplayScaled(panelGlobs.cryOreSideBarImage, NULL, &pos, &size);
    }
}

void Panel_Crystals_LoadRewardQuota(lpConfig config, const char* gameName, const char* levelName)
{
    // TODO: Implement Panel_Crystals_LoadRewardQuota
}

B32 Panel_IsFlags_2_Not8(Panel_Type panelType)
{
    PanelDataFlags flags = panelGlobs.panelTable[panelType].flags;
    if (((flags & PANELDATA_FLAG_UNK2) != 0) && ((flags & PANELDATA_FLAG_UNK8) == 0))
        return TRUE;

    return FALSE;
}

void Panel_ChangeFlags_BasedOnState(Panel_Type panelType)
{
    // TODO: Implement Panel_ChangeFlags_BasedOnState
}

void Panel_Button_SetFlag_8_OrUnset_c(Panel_Type panelType, PanelButton_Type buttonType, B32 setFlag8)
{
    // TODO: Implement Panel_Button_SetFlag_8_OrUnset_c
}

void Panel_UnsetFlag_6()
{
    // TODO: Implement Panel_UnsetFlag_6
}

U32 Panel_PrintF(Panel_Type panelType, lpFont font, S32 x, S32 y, B32 center, const char* msg, ...)
{
    // TODO: Implement Panel_PrintF

    return 0;
}

B32 Panel_CheckCollision(F32 elapsedAbs, U32 mouseX, U32 mouseY, B32 leftButton, B32 leftButtonLast, B32 *outPanelCollision)
{
    // TODO: Implement Panel_CheckCollision

    *outPanelCollision = FALSE;
    return FALSE;
}
