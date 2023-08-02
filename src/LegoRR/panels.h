#pragma once

#include "platform.h"
#include "config.h"

typedef enum Panel_Type
{
    Panel_Radar          = 0,
    Panel_RadarFill      = 1,
    Panel_RadarOverlay   = 2,
    Panel_Messages       = 3,
    Panel_MessagesSide   = 4,
    Panel_CrystalSideBar = 5,
    Panel_TopPanel       = 6,
    Panel_Information    = 7,
    Panel_PriorityList   = 8,
    Panel_CameraControl  = 9,
    Panel_InfoDock       = 10,
    Panel_Encyclopedia   = 11,
    Panel_Type_Count     = 12,
} Panel_Type;

typedef enum PanelButton_Type
{
    PanelButton_CameraControl_ZoomIn         = 0,
    PanelButton_CrystalSideBar_Ore           = 0,
    PanelButton_Encyclopedia_Close           = 0,
    PanelButton_InfoDock_Goto                = 0,
    PanelButton_Information_Toggle           = 0,
    PanelButton_PriorityList_Disable1        = 0,
    PanelButton_Radar_Toggle                 = 0,
    PanelButton_TopPanel_Options             = 0,
    PanelButton_CameraControl_ZoomOut        = 1,
    PanelButton_CrystalSideBar_Crystals      = 1,
    PanelButton_Encyclopedia_Count           = 1,
    PanelButton_InfoDock_Close               = 1,
    PanelButton_Information_Function         = 1,
    PanelButton_PriorityList_Disable2        = 1,
    PanelButton_Radar_TaggedObjectView       = 1,
    PanelButton_TopPanel_Priorities          = 1,
    PanelButton_CameraControl_CycleBuildings = 2,
    PanelButton_CrystalSideBar_Count         = 2,
    PanelButton_InfoDock_Count               = 2,
    PanelButton_Information_Count            = 2,
    PanelButton_PriorityList_Disable3        = 2,
    PanelButton_Radar_ZoomIn                 = 2,
    PanelButton_TopPanel_CallToArms          = 2,
    PanelButton_CameraControl_Rotate         = 3,
    PanelButton_PriorityList_Disable4        = 3,
    PanelButton_Radar_ZoomOut                = 3,
    PanelButton_TopPanel_Count               = 3,
    PanelButton_CameraControl_Count          = 4,
    PanelButton_PriorityList_Disable5        = 4,
    PanelButton_Radar_MapView                = 4,
    PanelButton_PriorityList_Disable6        = 5,
    PanelButton_Radar_Count                  = 5,
    PanelButton_PriorityList_Disable7        = 6,
    PanelButton_PriorityList_Disable8        = 7,
    PanelButton_PriorityList_Disable9        = 8,
    PanelButton_PriorityList_UpOne1          = 9,
    PanelButton_PriorityList_UpOne2          = 10,
    PanelButton_PriorityList_UpOne3          = 11,
    PanelButton_PriorityList_UpOne4          = 12,
    PanelButton_PriorityList_UpOne5          = 13,
    PanelButton_PriorityList_UpOne6          = 14,
    PanelButton_PriorityList_UpOne7          = 15,
    PanelButton_PriorityList_UpOne8          = 16,
    PanelButton_PriorityList_Close           = 17,
    PanelButton_PriorityList_Reset           = 18,
    PanelButton_PriorityList_Count           = 19,
    PanelButton_Type_Count                   = 24,
} PanelButton_Type;

extern void Panel_Initialize();

extern void Panel_RotationControl_Initialize(lpConfig config, const char* gameName);
extern void Panel_Crystals_Initialize(const char* smallCrystal, const char* usedCrystal, const char* noSmallCrystal);
extern void Panel_AirMeter_Initialize(const char* airJuiceName, U32 juiceX, U32 juiceY, U32 juiceLength, const char* noAirName, U32 noAirX, U32 noAirY);
extern void Panel_CryOreSideBar_Initialize(const char* sidebarName, U32 xPos, U32 yPos, U32 meterOffset);

extern void Panel_Button_SetFlags_10(Panel_Type panelType, PanelButton_Type buttonType, B32 state);

extern void Panel_LoadInterfaceButtons_ScrollInfo();

extern void Panel_FUN_0045a9f0(Panel_Type panelType, F32 elapsedAbs);

extern void Panel_CryOreSideBar_Draw();
