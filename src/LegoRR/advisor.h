#pragma once

#include "platform.h"
#include "lego_camera.h"
#include "viewport.h"
#include "config.h"

// "Acheived" is a source-accurate, but misspelled name
typedef enum Advisor_Type
{
    Advisor_Objective                        = 0,
    Advisor_ObjectiveAcheived                = 1,
    Advisor_ObjectiveAchieved                = 1,
    Advisor_ObjectiveFailed                  = 2,
    Advisor_IconPoint_Normal                 = 3,
    Advisor_IconPoint_BackButton             = 4,
    Advisor_IconPoint_TopButtons             = 5,
    Advisor_PanelPoint_RadarToggle           = 6,
    Advisor_PanelPoint_RadarTaggedObjectView = 7,
    Advisor_PanelPoint_RadarZoomIn           = 8,
    Advisor_PanelPoint_RadarZoomOut          = 9,
    Advisor_PanelPoint_RadarMapView          = 10,
    Advisor_PanelPoint_InfoDockGoto          = 11,
    Advisor_PanelPoint_InfoDockClose         = 12,
    Advisor_PanelPoint_TopPanelInfo          = 13,
    Advisor_PanelPoint_TopPanelOptions       = 14,
    Advisor_PanelPoint_TopPanelPriorities    = 15,
    Advisor_PanelPoint_TopPanelCallToArms    = 16,
    Advisor_PanelPoint_TopPanelGoBack        = 17,
    Advisor_PanelPoint_ControlZoomIn         = 18,
    Advisor_PanelPoint_ControlZoomOut        = 19,
    Advisor_TalkInGame                       = 20,
    Advisor_Type_Count                       = 21,
} Advisor_Type;

extern void Advisor_Initialize(const char* gameName, lpLegoCamera cameraMain, lpViewport viewMain);
extern void Advisor_InitViewport(F32 param_unused);
extern void Advisor_LoadAnims(lpConfig config, const char* gameName);
extern void Advisor_LoadPositions(lpConfig config, const char* gameName, S32 screenWidth, S32 screenHeight);

extern void Advisor_Update(F32 elapsed);
