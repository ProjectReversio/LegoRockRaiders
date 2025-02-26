#include "lego.h"
#include <string.h>
#include <stdio.h>
#include "platform.h"
#include "main.h"
#include "front_end.h"
#include "error.h"
#include "images.h"
#include "sfx.h"
#include "common.h"
#include "mem.h"
#include "sound.h"
#include "loader.h"
#include "utils.h"
#include "mesh.h"
#include "nerps.h"
#include "advisor.h"
#include "keys.h"
#include "info.h"
#include "interface.h"
#include "bubble.h"
#include "pointer.h"
#include "smoke.h"
#include "message.h"
#include "aitask.h"
#include "radarmap.h"
#include "panels.h"
#include "tooltip.h"
#include "text.h"
#include "priorities.h"
#include "lws.h"
#include "damage_font.h"
#include "detail.h"
#include "rewards.h"
#include "effects.h"
#include "3DSound.h"
#include "help_window.h"
#include "obj_info.h"
#include "weapons.h"
#include "light_effects.h"
#include "stats.h"
#include "dependencies.h"
#include "encyclopedia.h"
#include "objective.h"
#include "teleporter.h"
#include "construction.h"
#include "dummy.h"
#include "ptl.h"
#include "electric_fence.h"
#include "erode.h"
#include "fallin.h"
#include "spider_web.h"
#include "water.h"
#include "game_control.h"
#include "input.h"
#include "level.h"
#include "map_shared.h"
#include "object_recall.h"
#include "roof.h"
#include "search.h"

Lego_Globs legoGlobs;

extern const ColourRGBF c_wallHighlightColours[6] = {
    {1.0f, 1.0f, 1.0f}, // None
    {0.5f, 0.5f, 0.5f}, // Dig
    {1.0f, 0.5f, 0.4f}, // Dynamite
    {0.5f, 1.0f, 0.5f}, // Reinforce
    {0.4f, 0.5f, 1.0f}, // Selected
    {0.6f, 0.6f, 0.0f}, // Tutorial
};

B32 Gods_Go(const char* programName)
{
    memset(&legoGlobs, 0, sizeof(legoGlobs));
    legoGlobs.gameName = programName;
    Main_SetTitle(programName);

    Main_State mainState;
    mainState.Initialize = Lego_Initialize;
    mainState.MainLoop = Lego_MainLoop;
    mainState.Shutdown = Lego_Shutdown_Quick;

    if (Main_ProgrammerMode() == PROGRAMMER_MODE_10)
        mainState.Shutdown = Lego_Shutdown_Full; // Full shutdown with proper cleanup.

    return Main_SetState(&mainState);
}

#define Lego_Surface_RegisterName(n) legoGlobs.surfaceName[n] = #n

B32 Lego_Initialize()
{
    legoGlobs.DragBoxRGB.red = 0.2f;
    legoGlobs.DragBoxRGB.green = 0.7f;
    legoGlobs.DragBoxRGB.blue = 1.0f;

    Lego_Surface_RegisterName(Lego_SurfaceType_Tunnel);
    Lego_Surface_RegisterName(Lego_SurfaceType_Immovable);
    Lego_Surface_RegisterName(Lego_SurfaceType_Hard);
    Lego_Surface_RegisterName(Lego_SurfaceType_Medium);
    Lego_Surface_RegisterName(Lego_SurfaceType_Loose);
    Lego_Surface_RegisterName(Lego_SurfaceType_Soil);
    Lego_Surface_RegisterName(Lego_SurfaceType_Lava);
    Lego_Surface_RegisterName(Lego_SurfaceType_Water);
    Lego_Surface_RegisterName(Lego_SurfaceType_OreSeam);
    Lego_Surface_RegisterName(Lego_SurfaceType_Lake);
    Lego_Surface_RegisterName(Lego_SurfaceType_CrystalSeam);
    Lego_Surface_RegisterName(Lego_SurfaceType_RechargeSeam);
    Lego_Surface_RegisterName(Lego_SurfaceType_Rubble);
    Lego_Surface_RegisterName(Lego_SurfaceType_Reinforcement);
    Lego_Surface_RegisterName(Lego_SurfaceType_Path);
    Lego_Surface_RegisterName(Lego_SurfaceType_SlugHole);
    Lego_Surface_RegisterName(Lego_SurfaceType_Undiscovered);
    Lego_Surface_RegisterName(Lego_SurfaceType_Cavern);

    for (U32 surfaceId = 0; surfaceId < Lego_SurfaceType_Count; surfaceId++)
        Error_Fatal(legoGlobs.surfaceName[surfaceId] == NULL, Error_Format("Lego_Initialize: Surface name not registered for id: %d", surfaceId));

    Lego_SetGameSpeed(1.0f);

    Keys_Initialize();
    Viewport_Initialize();
    LegoObject_Initialize();
    Image_Initialize();
    TextWindow_Initialize();
    RadarMap_Initialize();
    Panel_Initialize();
    AITask_Initialize();
    Message_Initialize();
    Smoke_Initialize();
    Pointer_Initialize();
    Bubble_Initialize();

    legoGlobs.bmpFONT5_HI = Font_Load("Interface\\Fonts\\FONT5_HI.bmp");
    legoGlobs.bmpToolTipFont = Font_Load("Interface\\Fonts\\ToolTipFont.bmp");
    legoGlobs.bmpDesktopFont = Font_Load("Interface\\Fonts\\DeskTopFont.bmp");
    legoGlobs.bmpfont5_HI = Font_Load("Interface\\Fonts\\font5_HI.bmp");
    legoGlobs.bmpMbriefFONT = Font_Load("Interface\\Fonts\\MbriefFONT.bmp");
    legoGlobs.bmpMbriefFONT2 = Font_Load("Interface\\Fonts\\MbriefFONT2.bmp");
    legoGlobs.bmpRSFont = Font_Load("Interface\\Fonts\\RSFont.bmp");

    Info_Initialize(legoGlobs.bmpFONT5_HI);
    Interface_Initialize(565, 18, legoGlobs.bmpToolTipFont);

    legoGlobs.rootCont = Container_Initialize(legoGlobs.gameName);
    if (legoGlobs.rootCont == NULL)
    {
        Error_Warn(TRUE, "Failed to initialize root container");
        return FALSE;
    }

    Container_SetTriggerFrameCallback(LegoObject_TriggerFrameCallback, NULL);

    legoGlobs.config = Config_Load("Lego.cfg");
    if (legoGlobs.config == NULL)
    {
        Error_Warn(TRUE, "Failed to load config file 'Lego.cfg'");
        Container_Shutdown();
        return FALSE;
    }

    F32 tooltipRed;
    F32 tooltipGreen;
    F32 tooltipBlue;
    if (!Config_GetRGBValue(legoGlobs.config, Config_BuildStringID(legoGlobs.gameName, "Main", "ToolTipRGB", 0), &tooltipRed, &tooltipGreen, &tooltipBlue))
    {
        tooltipRed = 0.3254902f;
        tooltipGreen = 0.3254902f;
        tooltipBlue = 0.3254902f;
    }

    ToolTip_Initialize(legoGlobs.bmpToolTipFont, 2, 1, 1.0f, appWidth(), appHeight(), 32, tooltipRed, tooltipGreen, tooltipBlue);

    legoGlobs.CreditsTextFile = Config_GetStringValue(legoGlobs.config, Config_BuildStringID(legoGlobs.gameName, "Main", "CreditsTextFile", 0));
    legoGlobs.CreditsBackAVI = Config_GetStringValue(legoGlobs.config, Config_BuildStringID(legoGlobs.gameName, "Main", "CreditsBackAVI", 0));

    legoGlobs.BuildingUpgradeCostOre = Config_GetIntValue(legoGlobs.config, Config_BuildStringID(legoGlobs.gameName, "Main", "BuildingUpgradeCostOre", 0));
    if (legoGlobs.BuildingUpgradeCostOre == 0)
        legoGlobs.BuildingUpgradeCostOre = 5;

    legoGlobs.BuildingUpgradeCostStuds = Config_GetIntValue(legoGlobs.config, Config_BuildStringID(legoGlobs.gameName, "Main", "BuildingUpgradeCostStuds", 0));
    if (legoGlobs.BuildingUpgradeCostStuds == 0)
        legoGlobs.BuildingUpgradeCostStuds = 1;

    legoGlobs.RenameReplace = Config_GetStringValue(legoGlobs.config, Config_BuildStringID(legoGlobs.gameName, "Main", "RenameReplace", 0));
    char* renameReplace_str = legoGlobs.RenameReplace;
    if (legoGlobs.RenameReplace)
    {
        char* renameReplace_ptr;
        while (renameReplace_ptr = renameReplace_str, *renameReplace_ptr != '\0')
        {
            if (*renameReplace_ptr == '_')
                *renameReplace_ptr = ' ';

            renameReplace_str = renameReplace_ptr + 1;
        }
    }

    U32 videoMemTotal, videoMemFree;
    if (DirectDraw_GetAvailTextureMem(&videoMemTotal, &videoMemFree))
    {
        U32 textureUsage = Config_GetIntValue(legoGlobs.config, Config_BuildStringID(legoGlobs.gameName, "Main", "TextureUsage", 0));
        if (textureUsage != 0 && (DirectDraw_AdjustTextureUsage(&textureUsage), textureUsage <= videoMemFree))
            Main_DisableTextureManagement();
    }

    Area2F fontRender80_rect;
    fontRender80_rect.x = 55.0f;
    fontRender80_rect.y = 435.0f;
    fontRender80_rect.width = 325.0f;
    fontRender80_rect.height = 42.0f;
    legoGlobs.textWnd_80 = TextWindow_Create(legoGlobs.bmpfont5_HI, &fontRender80_rect, 1024);

    Area2F fontRender84_rect;
    fontRender84_rect.x = 55.0f;
    fontRender84_rect.y = 435.0f;
    fontRender84_rect.width = 260.0f;
    fontRender84_rect.height = 42.0f;
    legoGlobs.textWnd_84 = TextWindow_Create(legoGlobs.bmpfont5_HI, &fontRender84_rect, 1024);

    F32 textPauseTime = Config_GetFloatValue(legoGlobs.config, Config_BuildStringID(legoGlobs.gameName, "Main", "TextPauseTime", 0));
    Text_Load(legoGlobs.textWnd_80, legoGlobs.textWnd_84, 316, 425, textPauseTime * 25.0f);

    Area2F MsgPanel_rect1;
    MsgPanel_rect1.x = 55.0f;
    MsgPanel_rect1.y = 435.0f;
    MsgPanel_rect1.width = 325.0f;
    MsgPanel_rect1.height = 42.0f;

    Area2F MsgPanel_rect2;
    MsgPanel_rect2.x = 55.0f;
    MsgPanel_rect2.y = 363.0f;
    MsgPanel_rect2.width = 325.0f;
    MsgPanel_rect2.height = 140.0f;

    Text_Initialize("Interface\\MessagePanel\\MsgPanel.bmp", 42, 409, 338, &MsgPanel_rect1, &MsgPanel_rect2, 20.0f);
#ifndef LEGORR_FORCE_SHOW_VERSION
    if ((mainGlobs.flags & MAIN_FLAG_SHOWVERSION) != MAIN_FLAG_NONE)
#endif
    {
        const char* startMessage = Config_GetStringValue(legoGlobs.config, Config_BuildStringID(legoGlobs.gameName, "Main", "StartMessage", 0));
        const char* version = Config_GetTempStringValue(legoGlobs.config, Config_BuildStringID(legoGlobs.gameName, "Main", "Version", 0));

        if (startMessage)
        {
            startMessage = Front_Util_StringReplaceChar(startMessage, '_', ' ');
            if (version)
                TextWindow_PrintF(legoGlobs.textWnd_80, "\n%s (%s)", startMessage, version);
            else
                TextWindow_PrintF(legoGlobs.textWnd_80, "\n%s", startMessage);

            Mem_Free(startMessage);
        }
    }

    if(Front_IsFrontEndEnabled() && Front_IsIntrosEnabled())
    {
        Front_PlayIntroMovie("LegoAvi", Main_ProgrammerMode() != PROGRAMMER_OFF);
        Front_PlayIntroSplash("DDILogo", TRUE, "DDILogoTime");
        Front_PlayIntroMovie("DDIAvi", TRUE);
        Front_PlayIntroMovie("RRAvi", TRUE);
        Front_PlayIntroSplash("LMILogo", Main_ProgrammerMode() != PROGRAMMER_OFF, "LMILogoTime");
    }
    SFX_Initialize();

    const char* progressBar = Config_GetTempStringValue(legoGlobs.config, Config_BuildStringID(legoGlobs.gameName, "Main", "ProgressBar", 0));
    const char* loadScreen = Config_GetTempStringValue(legoGlobs.config, Config_BuildStringID(legoGlobs.gameName, "Main", "LoadScreen", 0));
    const char* shutdownScreen = Config_GetTempStringValue(legoGlobs.config, Config_BuildStringID(legoGlobs.gameName, "Main", "ShutdownScreen", 0));
    const char* progressWindow = Config_GetStringValue(legoGlobs.config, Config_BuildStringID(legoGlobs.gameName, "Main", "ProgressWindow", 0));
    const char* loadingText = Config_GetTempStringValue(legoGlobs.config, Config_BuildStringID(legoGlobs.gameName, "Main", "LoadingText", 0));

    Direction progressBarDirection;
    char* progressWindow_stringParts[10];
    Area2F progressWindow_rect;

    if (progressWindow == NULL)
    {
        progressBar = NULL;
    } else {
        char d = toupper(progressWindow[0]);
        switch (d)
        {
            case 'D':
                progressBarDirection = DIRECTION_DOWN;
                break;
            case 'L':
                progressBarDirection = DIRECTION_LEFT;
                break;
            case 'R':
                progressBarDirection = DIRECTION_RIGHT;
                break;
            case 'U':
                progressBarDirection = DIRECTION_UP;
                break;
            default:
                progressBarDirection = DIRECTION_COUNT;
                break;
        }

        U32 count = Util_Tokenize(progressWindow, progressWindow_stringParts, ",");
        if ((count == 4 && progressWindow[1] == ':') && progressBarDirection != DIRECTION_COUNT)
        {
            progressWindow_rect.x = atoi(progressWindow_stringParts[0] + 2);
            progressWindow_rect.y = atoi(progressWindow_stringParts[1]);
            progressWindow_rect.width = atoi(progressWindow_stringParts[2]);
            progressWindow_rect.height = atoi(progressWindow_stringParts[3]);
        }
    }

    Mem_Free(progressWindow);

    const char* loaderProfile_filename;
    if (Sound_IsInitialized())
        loaderProfile_filename = "LoaderProfile.txt";
    else
        loaderProfile_filename = "LoaderProfileNoSound.txt";

    Loader_Initialize(loadScreen, shutdownScreen, legoGlobs.bmpFONT5_HI, loaderProfile_filename, progressBarDirection, progressBar, &progressWindow_rect, loadingText);
    Loader_Display_Loading_Bar("Game Data");

    if (!(legoGlobs.cameraMain = Camera_Create(legoGlobs.rootCont, LegoCamera_Top)) ||
        !(legoGlobs.cameraFP = Camera_Create(legoGlobs.rootCont, LegoCamera_FP)) ||
        !(legoGlobs.cameraTrack = Camera_Create(legoGlobs.rootCont, LegoCamera_Radar)) ||
        !(legoGlobs.viewMain = Viewport_Create(0.0f, 0.0f, 1.0f, 1.0f, legoGlobs.cameraMain->contCam)) ||
        !(legoGlobs.viewTrack = Viewport_CreatePixel(16, 13, 151, 151, legoGlobs.cameraTrack->contCam)))
    {
        Error_Warn(TRUE, "Unable to create cameras and viewports");
        Container_Shutdown();
        return FALSE;
    }

    if (Config_GetBoolValue(legoGlobs.config, Config_BuildStringID(legoGlobs.gameName, "Main", "StreamNERPSSpeach", 0)) == BOOL3_TRUE)
        legoGlobs.flags1 |= GAME1_STREAMNERPSSPEACH;

    if (Config_GetBoolValue(legoGlobs.config, Config_BuildStringID(legoGlobs.gameName, "Main", "ShowDebugToolTips", 0)) == BOOL3_TRUE)
        legoGlobs.flags2 |= GAME2_SHOWDEBUGTOOLTIPS;

    if (Config_GetBoolValue(legoGlobs.config, Config_BuildStringID(legoGlobs.gameName, "Main", "AllowDebugKeys", 0)) == BOOL3_TRUE)
        legoGlobs.flags2 |= GAME2_ALLOWDEBUGKEYS;

    if (Config_GetBoolValue(legoGlobs.config, Config_BuildStringID(legoGlobs.gameName, "Main", "AllowEditMode", 0)) == BOOL3_TRUE)
        legoGlobs.flags2 |= GAME2_ALLOWEDITMODE;

    Config_GetRGBValue(legoGlobs.config, Config_BuildStringID(legoGlobs.gameName, "Main", "PowerCrystalRGB", 0), &legoGlobs.PowerCrystalRGB.red, &legoGlobs.PowerCrystalRGB.green, &legoGlobs.PowerCrystalRGB.blue);
    Config_GetRGBValue(legoGlobs.config, Config_BuildStringID(legoGlobs.gameName, "Main", "UnpoweredCrystalRGB", 0), &legoGlobs.UnpoweredCrystalRGB.red, &legoGlobs.UnpoweredCrystalRGB.green, &legoGlobs.UnpoweredCrystalRGB.blue);

    Lego_LoadSamples(legoGlobs.config, (mainGlobs.flags & MAIN_FLAG_REDUCESAMPLES) == MAIN_FLAG_NONE);
    Lego_LoadSurfaceTypeDescriptions_sound(legoGlobs.config, legoGlobs.gameName);

    Priorities_LoadPositions(legoGlobs.config, legoGlobs.gameName);
    Priorities_LoadImages(legoGlobs.config, legoGlobs.gameName);

    Lego_LoadToolTipInfos(legoGlobs.config, legoGlobs.gameName);

    Object_LoadToolNames(legoGlobs.config, legoGlobs.gameName);

    Panel_RotationControl_Initialize(legoGlobs.config, legoGlobs.gameName);

    const char* sharedObjects = Config_GetTempStringValue(legoGlobs.config, Config_BuildStringID(legoGlobs.gameName, "Main", "SharedObjects", 0));
    Lws_Initialize(sharedObjects, SFX_GetType, SFX_Random_PlaySound3DOnFrame, SFX_IsSoundOn);

    const char* sharedTextureDir = Config_GetTempStringValue(legoGlobs.config, Config_BuildStringID(legoGlobs.gameName, "Main", "SharedTextures", 0));
    if (sharedTextureDir)
        Container_SetSharedTextureDirectory(sharedTextureDir);
    Mesh_Initialize(sharedTextureDir);

    char buttonBuffer[128];
    sprintf(buttonBuffer, "NextButton%ix%i", appWidth(), appHeight());
    const char* nextButton = Config_GetTempStringValue(legoGlobs.config, Config_BuildStringID(legoGlobs.gameName, "Main", buttonBuffer, 0));
    if (nextButton && (legoGlobs.NextButtonImage = Image_LoadBMP(nextButton)))
    {
        Image_SetupTrans(legoGlobs.NextButtonImage, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0);
        sprintf(buttonBuffer, "NextButtonPos%ix%i", appWidth(), appHeight());
        const char* nextButtonPos = Config_GetStringValue(legoGlobs.config, Config_BuildStringID(legoGlobs.gameName, "Main", buttonBuffer, 0));
        if (nextButtonPos)
        {
            char* nextButtonPos_stringParts[10];
            U32 count = Util_Tokenize(nextButtonPos, nextButtonPos_stringParts, ",");
            if (count == 2)
            {
                legoGlobs.NextButtonPos.x = atof(nextButtonPos_stringParts[0]);
                legoGlobs.NextButtonPos.y = atof(nextButtonPos_stringParts[1]);
            } else {
                Error_Fatal(TRUE, "Unable to parse NextButtonPos");
            }
            Mem_Free(nextButtonPos);
        }
    }

    sprintf(buttonBuffer, "BackButton%ix%i", appWidth(), appHeight());
    const char* backButton = Config_GetTempStringValue(legoGlobs.config, Config_BuildStringID(legoGlobs.gameName, "Main", buttonBuffer, 0));
    if (backButton && (legoGlobs.BackButtonImage = Image_LoadBMP(nextButton)))
    {
        Image_SetupTrans(legoGlobs.BackButtonImage, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0);
        sprintf(buttonBuffer, "BackButtonPos%ix%i", appWidth(), appHeight());
        const char* backButtonPos = Config_GetStringValue(legoGlobs.config, Config_BuildStringID(legoGlobs.gameName, "Main", buttonBuffer, 0));
        if (backButtonPos)
        {
            char* backButtonPos_stringParts[10];
            U32 count = Util_Tokenize(backButtonPos, backButtonPos_stringParts, ",");
            if (count == 2)
            {
                legoGlobs.BackButtonPos.x = atof(backButtonPos_stringParts[0]);
                legoGlobs.BackButtonPos.y = atof(backButtonPos_stringParts[1]);
            } else {
                Error_Fatal(TRUE, "Unable to parse BackButtonPos");
            }
            Mem_Free(backButtonPos);
        }
    }

    const char* backArrow = Config_GetTempStringValue(legoGlobs.config, Config_BuildStringID(legoGlobs.gameName, "Main", "BackArrow", 0));
    if (backArrow && (legoGlobs.BackArrowImage = Image_LoadBMP(backArrow)))
    {
        Image_SetupTrans(legoGlobs.BackArrowImage, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0);
    }
    NERPs_SetHasNextButton(legoGlobs.NextButtonImage != NULL);

    if (!Lego_LoadLighting() || !Lego_LoadGraphicsSettings() || !Lego_LoadUpgradeTypes() ||
        !Lego_LoadVehicleTypes() || !Lego_LoadMiniFigureTypes() || !Lego_LoadRockMonsterTypes() ||
        !Lego_LoadBuildingTypes())
    {
        Error_Fatal(TRUE, "Unable to load game data");
        Config_Free(legoGlobs.config);
        Container_Shutdown();
        return FALSE;
    }

    F32 cameraSpeed = Config_GetFloatValue(legoGlobs.config, Config_BuildStringID(legoGlobs.gameName, "Main", "CameraSpeed", 0));
    F32 cameraDropOff = Config_GetFloatValue(legoGlobs.config, Config_BuildStringID(legoGlobs.gameName, "Main", "CameraDropOff", 0));
    F32 cameraAcceleration = Config_GetFloatValue(legoGlobs.config, Config_BuildStringID(legoGlobs.gameName, "Main", "CameraAcceleration", 0));
    U32 mouseScrollIndent = Config_GetIntValue(legoGlobs.config, Config_BuildStringID(legoGlobs.gameName, "Main", "MouseScrollIndent", 0));

    Camera_InitCameraMovements(cameraSpeed, cameraDropOff, cameraAcceleration, mouseScrollIndent);

    legoGlobs.MinDistFor3DSoundsOnTopView = Config_GetFloatValue(legoGlobs.config, Config_BuildStringID(legoGlobs.gameName, "Main", "MinDistFor3DSoundsOnTopView", 0));
    if (legoGlobs.MinDistFor3DSoundsOnTopView == 0.0f)
        legoGlobs.MinDistFor3DSoundsOnTopView = 150.0f;

    F32 maxDistFor3DSounds = Config_GetFloatValue(legoGlobs.config, Config_BuildStringID(legoGlobs.gameName, "Main", "MaxDistFor3DSounds", 0));
    if (maxDistFor3DSounds != 0.0f)
        Sound3D_SetMaxDist(maxDistFor3DSounds);

    Sound3D_SetRollOffFactor(1.0f);

    F32 rollOffFor3DSounds = Config_GetFloatValue(legoGlobs.config, Config_BuildStringID(legoGlobs.gameName, "Main", "RollOffFor3DSounds", 0));
    if (rollOffFor3DSounds != 0.0f)
        Sound3D_SetRollOffFactor(rollOffFor3DSounds);

    legoGlobs.selectPlace = SelectPlace_Create(legoGlobs.rootCont, 5.0f);
    SelectPlace_Hide(legoGlobs.selectPlace, TRUE);

    legoGlobs.MaxReturnedCrystals = Config_GetIntValue(legoGlobs.config, Config_BuildStringID(legoGlobs.gameName, "Main", "MaxReturnedCrystals", 0));
    if (legoGlobs.MaxReturnedCrystals == 0)
        legoGlobs.MaxReturnedCrystals = 10;

    legoGlobs.MouseScrollBorder = Config_GetIntValue(legoGlobs.config, Config_BuildStringID(legoGlobs.gameName, "Main", "MouseScrollBorder", 0));
    if (legoGlobs.MouseScrollBorder == 0)
        legoGlobs.MouseScrollBorder = 5;

    Lego_LoadObjectNames(legoGlobs.config);
    Lego_LoadObjectTheNames(legoGlobs.config);
    if (Config_GetBoolValue(legoGlobs.config, Config_BuildStringID(legoGlobs.gameName, "Main", "HelpWindowOn", 0)) == BOOL3_TRUE)
    {
        HelpWindow_SetFont(legoGlobs.bmpMbriefFONT2);
        HelpWindow_Initialize(legoGlobs.config, legoGlobs.gameName);
    }

    ObjInfo_Initialize(legoGlobs.config, legoGlobs.gameName);
    LightEffects_Load(legoGlobs.config, legoGlobs.gameName);
    Stats_Initialize(legoGlobs.config, legoGlobs.gameName);
    Dependencies_Initialize(legoGlobs.config, legoGlobs.gameName);
    Bubble_LoadBubbles(legoGlobs.config);
    Encyclopedia_Initialize(legoGlobs.config, legoGlobs.gameName);
    LegoObject_LoadObjTtsSFX(legoGlobs.config, legoGlobs.gameName);
    Weapon_Initialize(legoGlobs.config, legoGlobs.gameName);
    legoGlobs.viewMode = ViewMode_Top;
    Sound3D_MakeListener(legoGlobs.cameraMain->contListener->masterFrame);
    Sound3D_SetMinDistForAtten(legoGlobs.MinDistFor3DSoundsOnTopView);

    F32 minDist = Config_GetFloatValue(legoGlobs.config, Config_BuildStringID(legoGlobs.gameName, "Main", "mindist", 0));
    F32 maxDist = Config_GetFloatValue(legoGlobs.config, Config_BuildStringID(legoGlobs.gameName, "Main", "maxdist", 0));

    F32 minTilt = Config_GetAngle(legoGlobs.config, Config_BuildStringID(legoGlobs.gameName, "Main", "mintilt", 0));
    F32 maxTilt = Config_GetAngle(legoGlobs.config, Config_BuildStringID(legoGlobs.gameName, "Main", "maxtilt", 0));

    Camera_SetZoomRange(legoGlobs.cameraMain, minDist, maxDist);
    Camera_SetTiltRange(legoGlobs.cameraMain, minTilt, maxTilt);

    legoGlobs.TVClipDist = Config_GetFloatValue(legoGlobs.config, Config_BuildStringID(legoGlobs.gameName, "Main", "tvclipdist", 0));
    Viewport_SetBackClip(legoGlobs.viewMain, legoGlobs.TVClipDist);
    Viewport_SetBackClip(legoGlobs.viewTrack, legoGlobs.TVClipDist);

    legoGlobs.DynamiteDamageRadius = Config_GetFloatValue(legoGlobs.config, Config_BuildStringID(legoGlobs.gameName, "Main", "DynamiteDamageRadius", 0));
    legoGlobs.DynamiteMaxDamage = Config_GetFloatValue(legoGlobs.config, Config_BuildStringID(legoGlobs.gameName, "Main", "DynamiteMaxDamage", 0));
    legoGlobs.DynamiteWakeRadius = Config_GetFloatValue(legoGlobs.config, Config_BuildStringID(legoGlobs.gameName, "Main", "DynamiteWakeRadius", 0));
    legoGlobs.BirdScarerRadius = Config_GetFloatValue(legoGlobs.config, Config_BuildStringID(legoGlobs.gameName, "Main", "BirdScarerRadius", 0));

    if (Config_GetBoolValue(legoGlobs.config, Config_BuildStringID(legoGlobs.gameName, "Main", "MusicOn", 0)) == BOOL3_TRUE)
        legoGlobs.flags2 |= GAME2_MUSICON;

    legoGlobs.CDStartTrack = Config_GetIntValue(legoGlobs.config, Config_BuildStringID(legoGlobs.gameName, "Main", "CDStartTrack", 0));
    if (legoGlobs.CDStartTrack == 0)
        legoGlobs.CDStartTrack = 2;

    legoGlobs.CDTracks = Config_GetIntValue(legoGlobs.config, Config_BuildStringID(legoGlobs.gameName, "Main", "CDTracks", 0));
    if (legoGlobs.CDTracks == 0)
    {
        legoGlobs.flags2 &= ~GAME2_MUSICON;
        legoGlobs.CDTracks = 1;
    }

    if (Config_GetBoolValue(legoGlobs.config, Config_BuildStringID(legoGlobs.gameName, "Main", "SoundOn", 0)) == BOOL3_TRUE)
        legoGlobs.flags1 |= GAME1_USESFX;

    legoGlobs.flags1 |= (GAME1_USEDETAIL|GAME1_DDRAWCLEAR|GAME1_RENDERPANELS|GAME1_DYNAMICPM);

    if (Config_GetBoolValue(legoGlobs.config, Config_BuildStringID(legoGlobs.gameName, "Main", "Clear", 0)) == BOOL3_FALSE)
        legoGlobs.flags1 &= ~GAME1_DDRAWCLEAR;

    if (Config_GetBoolValue(legoGlobs.config, Config_BuildStringID(legoGlobs.gameName, "Main", "Panels", 0)) == BOOL3_FALSE)
        legoGlobs.flags1 &= ~GAME1_RENDERPANELS;

    if (Config_GetBoolValue(legoGlobs.config, Config_BuildStringID(legoGlobs.gameName, "Main", "WallProMeshes", 0)) == BOOL3_FALSE)
        legoGlobs.flags1 &= ~GAME1_USEDETAIL;

    if (Config_GetBoolValue(legoGlobs.config, Config_BuildStringID(legoGlobs.gameName, "Main", "DynamicPM", 0)) == BOOL3_FALSE)
        legoGlobs.flags1 &= ~GAME1_DYNAMICPM;

    legoGlobs.HPBlocks = Config_GetIntValue(legoGlobs.config, Config_BuildStringID(legoGlobs.gameName, "Main", "HPBlocks", 0));

    if (Config_GetBoolValue(legoGlobs.config, Config_BuildStringID(legoGlobs.gameName, "Main", "OnlyBuildOnPaths", 0)) == BOOL3_TRUE)
        legoGlobs.flags1 |= GAME1_ONLYBUILDONPATHS;

    if (Config_GetBoolValue(legoGlobs.config, Config_BuildStringID(legoGlobs.gameName, "Main", "AlwaysRockFall", 0)) == BOOL3_TRUE)
        legoGlobs.flags1 |= GAME1_ALWAYSROCKFALL;

    legoGlobs.MiniFigureRunAway = Config_GetFloatValue(legoGlobs.config, Config_BuildStringID(legoGlobs.gameName, "Main", "MiniFigureRunAway", 0));
    if (legoGlobs.MiniFigureRunAway == 0.0f)
        legoGlobs.MiniFigureRunAway = 10.0f;

    legoGlobs.DrainTime = Config_GetFloatValue(legoGlobs.config, Config_BuildStringID(legoGlobs.gameName, "Main", "DrainTime", 0));
    if (legoGlobs.DrainTime == 0.0f)
        legoGlobs.DrainTime = 10.0f;
    legoGlobs.DrainTime *= 25.0f;

    legoGlobs.ReinforceHits = Config_GetIntValue(legoGlobs.config, Config_BuildStringID(legoGlobs.gameName, "Main", "ReinforceHits", 0));
    if (legoGlobs.ReinforceHits == 0)
        legoGlobs.ReinforceHits = 4;

    legoGlobs.MedPolyRange = Config_GetFloatValue(legoGlobs.config, Config_BuildStringID(legoGlobs.gameName, "Main", "MedPolyRange", 0));
    legoGlobs.HighPolyRange = Config_GetFloatValue(legoGlobs.config, Config_BuildStringID(legoGlobs.gameName, "Main", "HighPolyRange", 0));

    Error_Fatal(legoGlobs.MedPolyRange == 0.0f, "Lego_Init: MedPolyRange must not be 0");
    Error_Fatal(legoGlobs.HighPolyRange == 0.0f, "Lego_Init: HighPolyRange must not be 0");

    legoGlobs.MinEnergyForEat = Config_GetFloatValue(legoGlobs.config, Config_BuildStringID(legoGlobs.gameName, "Main", "MinEnergyForEat", 0));

    Lego_LoadMiscObjects(legoGlobs.config);

    lpConfig pointersArray = Config_FindArray(legoGlobs.config, Config_BuildStringID(legoGlobs.gameName, "Pointers", 0));
    Error_Fatal(pointersArray == NULL, "Lego_Init: Pointers array not found");
    Pointer_LoadPointers(pointersArray);
    Pointer_SetCurrent_IfTimerFinished(Pointer_Standard);

    if ((legoGlobs.flags2 & GAME2_ALLOWDEBUGKEYS) != GAME2_NONE)
    {
        Message_Debug_RegisterSelectedUnitHotkey(KEY_ONE, Message_FirstPerson, NULL, FALSE, NULL);
        Message_Debug_RegisterSelectedUnitHotkey(KEY_TWO, Message_FirstPerson, NULL, TRUE, NULL);
        Message_Debug_RegisterSelectedUnitHotkey(KEY_FOUR, Message_TrackObject, NULL, FALSE, NULL);
        Message_Debug_RegisterSelectedUnitHotkey(KEY_THREE, Message_TopView, NULL, FALSE, NULL);
    }

    Smoke_LoadTextures("MiscAnims\\Smoke", "Smoke", 3);
    DamageFont_LoadFrames("Interface\\FONTS\\HealthFont", "a000_");

    Point2F Dialog_center;
    Dialog_center.x = 0.0f;
    Dialog_center.y = 0.0f;

    const char* dialogImage = Config_GetStringValue(legoGlobs.config, Config_BuildStringID(legoGlobs.gameName, "Dialog", "Image", 0));
    if (dialogImage && (legoGlobs.DialogImage = Image_LoadBMP(dialogImage)))
    {
        Image_SetupTrans(legoGlobs.DialogImage, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f);
        Dialog_center.x = ((F32)appWidth() / 2.0f) - (F32)(legoGlobs.DialogImage->width >> 1);
        Dialog_center.y = ((F32)appHeight() / 2.0f) - (F32)(legoGlobs.DialogImage->height >> 1);
    }

    char* Window_stringParts[4];
    const char* dialogTitleWindow = Config_GetStringValue(legoGlobs.config, Config_BuildStringID(legoGlobs.gameName, "Dialog", "TitleWindow", 0));
    if (dialogTitleWindow && Util_Tokenize(dialogTitleWindow, Window_stringParts, "|") == 4)
    {
        Area2F Window_rect;
        Window_rect.x = (F32)atoi(Window_stringParts[0]) + Dialog_center.x;
        Window_rect.y = (F32)atoi(Window_stringParts[1]) + Dialog_center.y;
        Window_rect.width = (F32)atoi(Window_stringParts[2]);
        Window_rect.height = (F32)atoi(Window_stringParts[3]);
        legoGlobs.DialogTextWndTitle = TextWindow_Create(legoGlobs.bmpMbriefFONT2, &Window_rect, 1024);
    }

    const char* dialogTextWindow = Config_GetStringValue(legoGlobs.config, Config_BuildStringID(legoGlobs.gameName, "Dialog", "TextWindow", 0));
    if (dialogTextWindow && Util_Tokenize(dialogTextWindow, Window_stringParts, "|") == 4)
    {
        Area2F Window_rect;
        Window_rect.x = (F32)atoi(Window_stringParts[0]) + Dialog_center.x;
        Window_rect.y = (F32)atoi(Window_stringParts[1]) + Dialog_center.y;
        Window_rect.width = (F32)atoi(Window_stringParts[2]);
        Window_rect.height = (F32)atoi(Window_stringParts[3]);
        legoGlobs.DialogTextWndMessage = TextWindow_Create(legoGlobs.bmpMbriefFONT, &Window_rect, 1024);
    }

    const char* dialogOkWindow = Config_GetStringValue(legoGlobs.config, Config_BuildStringID(legoGlobs.gameName, "Dialog", "OkWindow", 0));
    if (dialogOkWindow && Util_Tokenize(dialogOkWindow, Window_stringParts, "|") == 4)
    {
        Area2F Window_rect;
        Window_rect.x = (F32)atoi(Window_stringParts[0]) + Dialog_center.x;
        Window_rect.y = (F32)atoi(Window_stringParts[1]) + Dialog_center.y;
        Window_rect.width = (F32)atoi(Window_stringParts[2]);
        Window_rect.height = (F32)atoi(Window_stringParts[3]);
        legoGlobs.DialogTextWndOK = TextWindow_Create(legoGlobs.bmpMbriefFONT, &Window_rect, 1024);
    }

    const char* dialogCancelWindow = Config_GetStringValue(legoGlobs.config, Config_BuildStringID(legoGlobs.gameName, "Dialog", "CancelWindow", 0));
    if (dialogCancelWindow && Util_Tokenize(dialogCancelWindow, Window_stringParts, "|") == 4)
    {
        Area2F Window_rect;
        Window_rect.x = (F32)atoi(Window_stringParts[0]) + Dialog_center.x;
        Window_rect.y = (F32)atoi(Window_stringParts[1]) + Dialog_center.y;
        Window_rect.width = (F32)atoi(Window_stringParts[2]);
        Window_rect.height = (F32)atoi(Window_stringParts[3]);
        legoGlobs.DialogTextWndCancel = TextWindow_Create(legoGlobs.bmpMbriefFONT, &Window_rect, 1024);
    }

    const char* contrastOverlay = Config_GetStringValue(legoGlobs.config, Config_BuildStringID(legoGlobs.gameName, "Main", "ContrastOverlay", 0));
    if (contrastOverlay && (legoGlobs.DialogContrastOverlay = Image_LoadBMP(contrastOverlay)))
        Image_SetupTrans(legoGlobs.DialogContrastOverlay, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f);

    legoGlobs.flags1 |= (GAME1_RADARON|GAME1_RADAR_TRACKOBJECTVIEW);

    Dust_Setup(legoGlobs.rootCont, "animtex\\dirt");

    legoGlobs.radarZoom = 15.0f;
    legoGlobs.radarScreenRect.x = 16.0f;
    legoGlobs.radarScreenRect.y = 13.0f;
    legoGlobs.radarScreenRect.width = 151.0f;
    legoGlobs.radarScreenRect.height = 151.0f;

    Effect_Load_RockFallStylesAll(legoGlobs.config, legoGlobs.gameName, legoGlobs.rootCont);

    Lego_LoadToolTips(legoGlobs.config);
    Object_LoadToolTipIcons(legoGlobs.config);

    Lego_LoadUpgradeNames(legoGlobs.config);
    Lego_LoadInfoMessages(legoGlobs.config);

    Info_SetOverFlowImageFile("Interface\\MessageTabs\\infoOverFlow.bmp");

    Lego_LoadTextMessages(legoGlobs.config);

    Lego_LoadPanels(legoGlobs.config, appWidth(), appHeight());

    Panel_Crystals_Initialize("Interface\\RightPanel\\SmallCrystal.bmp", "Interface\\RightPanel\\UsedCrystal.bmp", "Interface\\RightPanel\\NoSmallCrystal.bmp");
    Panel_AirMeter_Initialize("Interface\\AirMeter\\msgpanel_air_juice.bmp", 85, 6, 236, "Interface\\AirMeter\\msgpanel_noair.bmp", 32, 0);
    Panel_CryOreSideBar_Initialize("Interface\\RightPanel\\crystalsidebar_ore.bmp", 615, 434, 423);

    Lego_LoadPanelButtons(legoGlobs.config, appWidth(), appHeight());
    Lego_LoadTutorialIcon(legoGlobs.config);

    Interface_InitBuildItems();
    Interface_LoadMenuItems(legoGlobs.config, legoGlobs.gameName);
    Interface_LoadItemPanels(legoGlobs.config, legoGlobs.gameName);
    Interface_LoadPlusMinusImages("Interface\\Dependencies\\+.bmp", "Interface\\Dependencies\\=.bmp");

    Reward_LoadGraphics(legoGlobs.config, legoGlobs.gameName);
    Reward_Initialize();

    Panel_LoadInterfaceButtons_ScrollInfo();

    Panel_Button_SetFlags_10(Panel_Radar, PanelButton_Radar_ZoomIn, TRUE);
    Panel_Button_SetFlags_10(Panel_Radar, PanelButton_Radar_ZoomOut, TRUE);

    Front_ResetSaveNumber();

    if (Front_IsFrontEndEnabled())
        Front_Initialize(legoGlobs.config);

    Advisor_Initialize(legoGlobs.gameName, legoGlobs.cameraMain, legoGlobs.viewMain);
    Advisor_InitViewport(200.0f);
    Advisor_LoadAnims(legoGlobs.config, legoGlobs.gameName);

    if (Config_GetBoolValue(legoGlobs.config, Config_BuildStringID(legoGlobs.gameName, "Main", "DisplayAdvisor", 0)) == BOOL3_TRUE)
        Advisor_LoadPositions(legoGlobs.config, legoGlobs.gameName, appWidth(), appHeight());

    if ((legoGlobs.flags1 & GAME1_USESFX) != GAME1_NONE)
    {
        Lego_SetSoundOn(TRUE);
    }

    const char* startLevelName = NULL;
    if (Main_ProgrammerMode() < PROGRAMMER_MODE_3 &&
        Config_GetBoolValue(legoGlobs.config, Config_BuildStringID(legoGlobs.gameName, "Main", "FrontEnd", 0)) == BOOL3_TRUE)
    {
        Loader_Display_Loading_Bar(NULL);

        Front_RunScreenMenuType(Menu_Screen_Title);
        if (Front_IsTriggerAppQuit())
            return FALSE;

        startLevelName = Front_GetSelectedLevel();
    } else {
        startLevelName = Main_GetStartLevel();
        if (!startLevelName)
            startLevelName = Config_GetTempStringValue(legoGlobs.config, Config_BuildStringID(legoGlobs.gameName, "Main", "startlevel", 0));
    }

    Front_LoadOptionParameters(TRUE, FALSE);
    if (Lego_LoadLevel(startLevelName))
    {
        SFX_Random_PlaySoundNormal(SFX_AmbientLoop, TRUE);
        return TRUE;
    }

    Config_Free(legoGlobs.config);
    Container_Shutdown();
    return FALSE;
}

B32 Lego_Shutdown_Full()
{
    // TODO: Implement Lego_Shutdown_Full

    Image_Shutdown();

    // TODO: Implement Lego_Shutdown_Full

    return TRUE;
}

B32 Lego_Shutdown_Quick()
{
    // TODO: Implement Lego_Shutdown_Quick
    return TRUE;
}

B32 Lego_MainLoop(F32 elapsed)
{
    F32 elapsedGame;

    F32 elapsedAbs = elapsed;

    // TODO: Implement Lego_MainLoop

    if ((legoGlobs.flags1 & GAME1_FREEZEINTERFACE) != GAME1_NONE)
        elapsed = 0.0f;

    // TODO: Implement Lego_MainLoop

    legoGlobs.elapsedAbs = elapsed;
    if (legoGlobs.viewMode == ViewMode_FP && mainGlobs.programmerLevel < PROGRAMMER_MODE_2)
        elapsedGame = elapsed;
    else
        elapsedGame = (F32)Lego_GetGameSpeed() * elapsed;

    // TODO: Implement Lego_MainLoop

    if ((legoGlobs.flags1 & GAME1_LEVELSTART) != GAME1_NONE)
    {
        elapsedGame = 1.0f;
        legoGlobs.flags1 &= ~GAME1_LEVELSTART;
    }

    B32 keyDownT;
    B32 keyDownR;
    B32 keyDownAnyShift;
    B32 dontExit = Lego_HandleKeys(elapsedGame, elapsed, &keyDownT, &keyDownR, &keyDownAnyShift);
    if (!dontExit)
        return FALSE;

    if ((legoGlobs.flags1 & GAME1_FREEZEINTERFACE) == GAME1_NONE)
        Lego_HandleRadarInput();

    AITask_UpdateAll(elapsedGame);
    Level_UpdateEffects(legoGlobs.currLevel, elapsedGame);
    DamageFont_UpdateAll(elapsed);
    LegoObject_UpdateAll(elapsedGame);
    Weapon_Update(elapsedGame);
    Erode_Update(elapsedGame);
    Level_BlockActivity_UpdateAll(legoGlobs.currLevel, elapsedGame);
    Message_PTL_Update();

    Camera_Update(legoGlobs.cameraMain, legoGlobs.currLevel, elapsed, elapsedGame);
    Camera_Update(legoGlobs.cameraTrack, legoGlobs.currLevel, elapsed, elapsedGame);
    Camera_Update(legoGlobs.cameraFP, legoGlobs.currLevel, elapsed, elapsedGame);

    Construction_UpdateAll(elapsedGame);

    Interface_FUN_0041b940(elapsedGame);

    Lego_UnkUpdateMapsWorldUnk_FUN_004290d0(elapsed, FALSE);

    LegoObject_HideAllCertainObjects();
    ElectricFence_UpdateAll(elapsedGame);

    Lego_HandleWorld(elapsedAbs, elapsed, keyDownT, keyDownR, keyDownAnyShift);
    Map3D_Update(legoGlobs.currLevel->map, elapsedGame);

    Water_Update(legoGlobs.currLevel, elapsedGame);
    Sound3D_UpdateListener();
    Smoke_Update(elapsedGame);
    SpiderWeb_Update(elapsedGame);
    LightEffects_Update(elapsedGame);
    Info_FUN_0041a1f0(elapsedGame);

    if (legoGlobs.IsFallinsEnabled)
        Fallin_Update(elapsedGame);

    // "Toggles fallin mode."
    if ((legoGlobs.flags2 & GAME2_ALLOWDEBUGKEYS) != GAME2_NONE &&
        Input_IsKeyPressed(KEY_F6))
    {
        legoGlobs.IsFallinsEnabled = !legoGlobs.IsFallinsEnabled;
        const char* txt = "On";
        if (!legoGlobs.IsFallinsEnabled)
            txt = "Off";
        TextWindow_PrintF(legoGlobs.textWnd_80, "\nFallin Mode %s", txt);
    }

    Container_Hide(legoGlobs.ambientLight, FALSE);
    if (legoGlobs.viewMode == ViewMode_Top)
    {
        if ((legoGlobs.flags1 & GAME1_FREEZEINTERFACE) == GAME1_NONE)
            Container_Hide(legoGlobs.rootSpotlight, FALSE);

        Container_Hide(legoGlobs.spotlightTop, FALSE);

        Viewport_Clear(legoGlobs.viewMain, TRUE);
        Viewport_Render(legoGlobs.viewMain, legoGlobs.rootCont, elapsed);

        Container_Hide(legoGlobs.spotlightTop, TRUE);
        Container_Hide(legoGlobs.rootSpotlight, TRUE);
    }
    else if (legoGlobs.viewMode == ViewMode_FP)
    {
        // TODO: Implement Lego_MainLoop
    }
    Container_Hide(legoGlobs.ambientLight, TRUE);

    if (Panel_IsFlags_2_Not8(Panel_Radar))
        legoGlobs.flags1 |= GAME1_RADARON;
    else
        legoGlobs.flags1 &= ~GAME1_RADARON;

    if (legoGlobs.viewMode == ViewMode_Top)
    {
        Lego_DrawAllSelectedUnitBoxes(legoGlobs.viewMain);
        Lego_DrawAllLaserTrackerBoxes(legoGlobs.viewMain);
        Encyclopedia_DrawSelectBox(legoGlobs.viewMain);
    }

    Lego_DrawDragSelectionBox(legoGlobs.currLevel);

    if ((legoGlobs.flags1 & GAME1_FREEZEINTERFACE) == GAME1_NONE)
    {
        Bubble_Unk_DrawObjectUIs_FUN_004074d0(elapsed);
    }

    // TODO: Implement Lego_MainLoop

    Main_Finalize3D();
    Smoke_HideAll(TRUE);
    SFX_Update(elapsedGame);

    // TODO: Implement Lego_MainLoop

    Map3D_HideVisibleBlocksList(legoGlobs.currLevel->map);

    // TODO: Implement Lego_MainLoop

    if ((legoGlobs.flags1 & GAME1_RENDERPANELS) != GAME1_NONE)
    {
        if ((legoGlobs.flags1 & GAME1_RADARON) == GAME1_NONE)
        {
            Panel_DrawPanel(Panel_RadarFill, elapsed);
        }
        Panel_DrawPanel(Panel_Radar, elapsed);

        if ((((legoGlobs.flags1 & GAME1_RADARON) != GAME1_NONE) && ((legoGlobs.flags1 & GAME1_RADAR_TRACKOBJECTVIEW) != GAME1_NONE)) &&
            ((legoGlobs.flags1 & GAME1_RADAR_NOTRACKOBJECT) == GAME1_NONE))
        {
            Panel_DrawPanel(Panel_RadarOverlay, elapsed);
        }
        Panel_DrawPanel(Panel_MessagesSide, elapsed);
        Panel_DrawPanel(Panel_CameraControl, elapsed);
        Panel_DrawPanel(Panel_TopPanel, elapsed);
        Panel_DrawPanel(Panel_PriorityList, elapsed);
        Priorities_Draw();
        Panel_DrawPanel(Panel_Encyclopedia, elapsed);
        Encyclopedia_Update(elapsed);
        if ((legoGlobs.flags2 & GAME2_INMENU) == GAME2_NONE)
        {
            Interface_FUN_0041b3c0();
        }
        Interface_FUN_0041b860(elapsed);
        Panel_DrawPanel(Panel_CrystalSideBar, elapsed);

        F32 crystals = elapsed;
        F32 chargedCrystals = elapsed;
        if (legoGlobs.currLevel != NULL)
        {
            crystals = legoGlobs.currLevel->crystals;
            chargedCrystals = legoGlobs.currLevel->crystals - legoGlobs.currLevel->crystalsDrained;
        }
        Panel_Crystals_Draw((U32)crystals, (U32)chargedCrystals, elapsedGame);
        Panel_CryOreSideBar_Draw();

        Panel_PrintF(Panel_CrystalSideBar, legoGlobs.bmpToolTipFont, 16, 469, TRUE, "%i", legoGlobs.currLevel->ore + legoGlobs.currLevel->studs * 5);

        Panel_DrawPanel(Panel_Information, elapsed);
        ScrollInfo_Update(FALSE);
        Info_DrawPanel(elapsed);
        Panel_DrawPanel(Panel_InfoDock, elapsed);
        Info_Draw(elapsed);
    }

    HelpWindow_FUN_00418930();

    F32 el = elapsed;
    if (Objective_IsEnded())
        el = elapsedGame;

    Advisor_Update(el);
    Sound_Update(legoGlobs.flags1 & GAME1_USEMUSIC);

    if ((legoGlobs.flags1 & GAME1_DEBUG_NONERPS) == GAME1_NONE)
        NERPsRuntime_Execute(elapsed);

    Objective_Update(legoGlobs.textWnd_80, legoGlobs.currLevel, elapsedGame, elapsedAbs);

    if (mainGlobs.programmerLevel < PROGRAMMER_MODE_2)
    {
        // TODO: Implement Lego_MainLoop
    }

    Text_Update(elapsed);

    // TODO: Implement Lego_MainLoop

    Lego_DrawRenameInput(elapsedGame);
    if ((legoGlobs.flags1 & GAME1_PAUSED) == GAME1_NONE)
    {
        Pointer_Update(elapsedAbs);
        if (legoGlobs.renameInput == NULL)
            Pointer_DrawPointer(inputGlobs.msx, inputGlobs.msy);
    }
    else
    {
        if (!Front_IsFrontEndEnabled() || !Front_Options_Update(elapsed, Menu_Modal_Paused))
        {
            Pointer_Update(elapsedAbs);
            Pointer_DrawPointer(inputGlobs.msx, inputGlobs.msy);
        }
        else
        {
            // TODO: Implement Lego_MainLoop
        }
    }

    // TODO: Implement Lego_MainLoop

#ifdef LEGORR_DEBUG_SHOW_INFO
        Lego_Debug_ShowInfo();
#endif

    // TODO: Implement Lego_MainLoop

#ifdef LEGORR_FORCE_DEV_MODE
    if ((legoGlobs.flags1 & GAME1_DEBUG_SHOWVERTEXMODE) == GAME1_NONE)
        legoGlobs.flags1 |= GAME1_DEBUG_SHOWVERTEXMODE;
#endif
    if ((legoGlobs.flags1 & GAME1_DEBUG_SHOWVERTEXMODE) != GAME1_NONE)
    {
        const char* txt = "Vertex";
        if ((legoGlobs.flags1 & GAME1_VERTEXMODE) == GAME1_NONE)
            txt = "Block";
        Font_PrintF(legoGlobs.bmpDesktopFont, 10, 10, "%s mode", txt);
    }

    if ((legoGlobs.flags1 & GAME1_VERTEXMODE) != GAME1_NONE && legoGlobs.bool_c8 != 0)
    {
        // TODO: Implement Lego_MainLoop
        //Point3F vec;
        //Map3D_GetBlockFirstVertexPosition(legoGlobs.currLevel->map, legoGlobs.pointi_c0.x, legoGlobs.pointi_c0.y, &vec);
        //Viewport_Transform(legoGlobs.viewMain, ???, &vec);
        // TODO: Implement Lego_MainLoop
    }

    ToolTip_Update(inputGlobs.msx, inputGlobs.msy, elapsedGame);

    if ((legoGlobs.flags2 & GAME2_ALLOWDEBUGKEYS) != GAME2_NONE)
    {
        // TODO: Implement Lego_MainLoop
    }

    // TODO: Implement Lego_MainLoop

    if ((legoGlobs.flags2 & GAME2_ALLOWDEBUGKEYS) != GAME2_NONE)
    {
        // TODO: Implement Lego_MainLoop
    }

    return TRUE;
}

// Returning FALSE will naturally exit the program (as handled by Lego_MainLoop).
B32 Lego_HandleKeys(F32 elapsedGame, F32 elapsed, B32 *outKeyDownT, B32 *outKeyDownR, B32 *outKeyDownAnyShift)
{
    // TODO: Implement Lego_HandleKeys

    if ((legoGlobs.flags1 & (GAME1_FREEZEINTERFACE | GAME1_LEVELENDING)) == GAME1_NONE)
    {
        // TODO: Implement Lego_HandleKeys

        if ((legoGlobs.flags2 & GAME2_ALLOWDEBUGKEYS) != GAME2_NONE)
        {
            // Toggle sound On/Off.
            if (Input_IsKeyPressed(KEY_S))
            {
                if ((legoGlobs.flags1 & GAME1_USESFX) == GAME1_NONE)
                {
                    Lego_SetSoundOn(TRUE);
                    SFX_Random_PlaySoundNormal(SFX_AmbientLoop, TRUE);
                }
                else
                {
                    Lego_SetSoundOn(FALSE);
                }

                const char* txt = "ON";
                if ((legoGlobs.flags1 & GAME1_USESFX) == GAME1_NONE)
                    txt = "OFF";
                TextWindow_PrintF(legoGlobs.textWnd_80, "\nSound Effects: %s", txt);
            }

            // Toggle music On/Off.
            if (Input_IsKeyPressed(KEY_M))
            {
                Lego_SetMusicOn((legoGlobs.flags1 & GAME1_USEMUSIC) == GAME1_NONE);
                const char* txt = "ON";
                if ((legoGlobs.flags1 & GAME1_USEMUSIC) == GAME1_NONE)
                    txt = "OFF";
                TextWindow_PrintF(legoGlobs.textWnd_80, "\nMusic: %s", txt);
            }
        }

        // TODO: Implement Lego_HandleKeys
    }

    // TODO: Implement Lego_HandleKeys

    B32 gotoNextLevel = FALSE;

    B32 lastLeftMouseButtonDown = FALSE;
    if (gamectrlGlobs.handleKeysLastLeftMouseButtonDown != 0 && inputGlobs.mslb == FALSE)
        lastLeftMouseButtonDown = TRUE;

    B32 spaceKeyHeld;
    if (Input_IsKeyUp(KEY_SPACE) && !Input_IsKeyReleased(KEY_SPACE))
        spaceKeyHeld = FALSE;
    else
        spaceKeyHeld = TRUE;

    B32 objHandleKeys = Objective_HandleKeys(spaceKeyHeld, lastLeftMouseButtonDown, &gotoNextLevel);

    // TODO: Implement Lego_HandleKeys

    gamectrlGlobs.handleKeysLastLeftMouseButtonDown = inputGlobs.mslb;
    if (gotoNextLevel)
        return FALSE;

    if ((legoGlobs.flags2 & GAME2_ALLOWDEBUGKEYS) != GAME2_NONE)
    {
        // TODO: Implement Lego_HandleKeys
    }

    if ((mainGlobs.flags & MAIN_FLAG_DEBUGCOMPLETE) == MAIN_FLAG_NONE)
        return TRUE;

    // TODO: Implement Lego_HandleKeys

    return TRUE;
}

// bx,by : mouse-over block position.
// liveObj : mouse-over object.
void Lego_HandleDebugKeys(S32 bx, S32 by, lpLegoObject liveObj, F32 gameCtrlElapsed)
{
    // TODO: Implement Lego_HandleDebugKeys
}

void Lego_HandleWorld(F32 elapsedGame, F32 elapsedAbs, B32 keyDownT, B32 keyDownR, B32 keyDownAnyShift)
{
    B32 leftReleased = 0;
    B32 rightReleased = 0;
    B32 thing = FALSE;

    legoGlobs.flags2 &= ~GAME2_MOUSE_INSIDEGAMEVIEW;

    SelectPlace_Hide(legoGlobs.selectPlace, TRUE);
    if (inputGlobs.mslb == FALSE && inputGlobs.msrb == FALSE)
    {
        gamectrlGlobs.handleWorldNoMouseButtonsElapsed += elapsedGame;
    }
    else
    {
        gamectrlGlobs.handleWorldNoMouseButtonsElapsed = 0.0f;
    }

    if (gamectrlGlobs.mslb_Last_0 != 0)
    {
        if (inputGlobs.mslb == FALSE)
        {
            if ((legoGlobs.flags1 & GAME1_MULTISELECT) == GAME1_NONE)
            {
                thing = TRUE;
                leftReleased = TRUE;
            }
        }
        else
        {
            gamectrlGlobs.mslb_Last_6 = TRUE;
        }
    }

    if (inputGlobs.mslb == FALSE)
    {
        gamectrlGlobs.mslb_Last_6 = FALSE;
    }

    if (gamectrlGlobs.mslr_Last_0 != 0 && inputGlobs.msrb == FALSE)
    {
        rightReleased = TRUE;
    }

    if (gamectrlGlobs.mslb_Last_6 == FALSE)
    {
        gamectrlGlobs.pointf_8.x = 0.0f;
        gamectrlGlobs.pointf_8.y = 0.0f;
    }
    else
    {
        gamectrlGlobs.pointf_8.x += (F32)((inputGlobs.diffx ^ inputGlobs.diffx >> 0x1f) - (inputGlobs.diffx >> 0x1f));
        gamectrlGlobs.pointf_8.y += (F32)((inputGlobs.diffy ^ inputGlobs.diffy >> 0x1f) - (inputGlobs.diffy >> 0x1f));
    }

    HelpWindow_FUN_00418cd0(inputGlobs.msx, inputGlobs.msy, inputGlobs.mslb, gamectrlGlobs.mslb_Last_0, thing, elapsedGame);

    if ((legoGlobs.flags1 & GAME1_FREEZEINTERFACE) != GAME1_NONE)
    {
        Pointer_SetCurrent_IfTimerFinished(Pointer_Standard);
        legoGlobs.flags1 &= ~(GAME1_MULTISELECT|GAME1_UNK_200);

        gamectrlGlobs.mslb_Last_0 = inputGlobs.mslb;
        gamectrlGlobs.mslr_Last_0 = inputGlobs.msrb;
        gamectrlGlobs.bool_160 = 1;
        return;
    }

    if (gamectrlGlobs.bool_160 != 0)
    {
        gamectrlGlobs.mslb_Last_0 = 0;
        gamectrlGlobs.mslr_Last_0 = 0;
        gamectrlGlobs.mslb_Last_6 = 0;
        leftReleased = FALSE;
        rightReleased = FALSE;
        gamectrlGlobs.bool_160 = 0;
    }

    Lego_Block* theBlock = NULL;
    U32 mouseBlockX, mouseBlockY;
    Point3F mouseWorldPos;
    B32 intersections = Map3D_GetIntersections(legoGlobs.currLevel->map, legoGlobs.viewMain, inputGlobs.msx, inputGlobs.msy, &mouseBlockX, &mouseBlockY, &mouseWorldPos);
    B32 somethingIntersections = intersections == FALSE;
    if (intersections)
    {
        theBlock = &legoGlobs.currLevel->blocks[legoGlobs.currLevel->width * mouseBlockY + mouseBlockX];
        Container_SetPosition(legoGlobs.rootLight, NULL, mouseWorldPos.x, mouseWorldPos.y, mouseWorldPos.z - 250.0f);
    }
    LightEffects_SetDimmerMode(!intersections);

    // TODO: Implement Lego_HandleWorld

    Pointer_Type currentPointer = Pointer_GetCurrentType();
    if ((legoGlobs.flags1 & GAME1_MULTISELECT) == GAME1_NONE && Message_AnyUnitSelected())
        Pointer_SetCurrent_IfTimerFinished(Pointer_Selected);
    else
        Pointer_SetCurrent_IfTimerFinished(Pointer_Standard);

    B32 collFlag = FALSE;

    if ((legoGlobs.flags1 & GAME1_MULTISELECT) == GAME1_NONE &&
        Panel_CheckCollision(elapsedAbs, inputGlobs.msx, inputGlobs.msy, inputGlobs.mslb, gamectrlGlobs.mslb_Last_0, &collFlag))
    {
        Lego_UpdateTopdownCamera(elapsedAbs);
        legoGlobs.flags1 |= GAME1_UNK_200;
        goto nwlabel;
    }

    Lego_UpdateTopdownCamera(elapsedAbs);

    PointerSFX_Type ptrType;
    if ((legoGlobs.flags1 & GAME1_MULTISELECT) == GAME1_NONE)
    {
        if (Priorities_HandleInput(inputGlobs.msx, inputGlobs.msy, inputGlobs.mslb, gamectrlGlobs.mslb_Last_0, leftReleased))
        {
            legoGlobs.flags1 |= GAME1_UNK_200;
            goto nwlabel;
        }

        if ((legoGlobs.flags1 & GAME1_MULTISELECT) == GAME1_NONE &&
            (legoGlobs.flags2 & GAME2_INMENU) == GAME2_NONE &&
            Interface_DoSomethingWithRenameReplace(inputGlobs.msx, inputGlobs.msy, inputGlobs.mslb, gamectrlGlobs.mslb_Last_0, leftReleased))
        {
            legoGlobs.flags1 |= GAME1_UNK_200;
            goto nwlabel;
        }

        if ((legoGlobs.flags1 & GAME1_MULTISELECT) != GAME1_NONE ||
            !Info_Update_FUN_0041a0d0(inputGlobs.msx, inputGlobs.msy, leftReleased))
        {
            goto nextLbl;
        }

        legoGlobs.flags1 |= GAME1_UNK_200;

        if (leftReleased == 0)
            goto nwlabel;

        ptrType = PointerSFX_Okay;

theLblNew:
        Lego_SetPointerSFX(ptrType);
        goto nwlabel;
    }

nextLbl:
    if (legoGlobs.viewMode == ViewMode_FP)
    {
        legoGlobs.flags1 |= GAME1_UNK_200;
        goto nwlabel;
    }

    if ((legoGlobs.flags1 & GAME1_MULTISELECT) == GAME1_NONE &&
        (legoGlobs.flags1 & GAME1_RADARON) != GAME1_NONE &&
        RadarMap_InsideRadarScreen(legoGlobs.currLevel->radarMap, inputGlobs.msx, inputGlobs.msy))
    {
        if (leftReleased != 0)
        {
            Lego_SetPointerSFX(PointerSFX_Okay);
        }

        if ((legoGlobs.flags1 & GAME1_RADAR_TRACKOBJECTVIEW) == GAME1_NONE)
        {
            Pointer_SetCurrent_IfTimerFinished(currentPointer);
        }

        goto nwlabel;
    }

    if (collFlag)
    {
        // TODO: Implement Lego_HandleWorld
    }

    if (somethingIntersections)
        goto nwlabel;

    legoGlobs.mouseWorldPos.x = mouseWorldPos.x;
    legoGlobs.mouseWorldPos.y = mouseWorldPos.y;
    legoGlobs.mouseWorldPos.z = mouseWorldPos.z;
    legoGlobs.flags2 |= GAME2_MOUSE_INSIDEGAMEVIEW;
    legoGlobs.mouseBlockPos.x = mouseBlockX;
    legoGlobs.mouseBlockPos.y = mouseBlockY;
    LegoObject* mouseObj = NULL;
    // TODO: Cleanup this flags check
    if ((legoGlobs.flags1 >> 8 & 1) == 0 || (legoGlobs.flags3 & (GAME3_UNK_1 | GAME3_PLACEBUILDING)) != GAME3_NONE)
    {
        if ((legoGlobs.flags3 & GAME3_PLACEBUILDING) == GAME3_NONE)
        {
            Pointer_Type pointer;
            if (LegoObject_DoPickSphereSelection(inputGlobs.msx, inputGlobs.msy, &mouseObj))
            {
                // --- Hovering on Object ---

                if (mouseObj->type == LegoObject_MiniFigure && mouseObj->driveObject != NULL)
                {
                    mouseObj = mouseObj->driveObject;
                }
                if (mouseObj->carryingThisObject != NULL)
                {
                    mouseObj = mouseObj->carryingThisObject;
                }
                LegoObject* someObj = NULL;
                if (mouseObj->type == LegoObject_Vehicle && Message_AnyUnitSelected() &&
                    (someObj = Message_GetPrimarySelectedUnit(), someObj->type == LegoObject_MiniFigure))
                {
                    // TODO: Implement Lego_HandleWorld
                }
                else if ((legoGlobs.flags3 & GAME3_UNK_20) == GAME3_NONE && (legoGlobs.flags3 & GAME3_UNK_40) == GAME3_NONE)
                {
                    // TODO: Implement Lego_HandleWorld
                }
                else
                {
                    // TODO: Implement Lego_HandleWorld
                }
            }
            else
            {
                // --- Hovering on Block ---

                if ((legoGlobs.flags1 & GAME1_LASERTRACKER) != GAME1_NONE)
                {
                    B32 newBool;
                    BlockFlags1 bf1;
                    if (mouseBlockX < legoGlobs.currLevel->width - 1 && mouseBlockY < legoGlobs.currLevel->height - 1 &&
                        (bf1 = legoGlobs.currLevel->blocks[mouseBlockY * legoGlobs.currLevel->width + mouseBlockX].flags1, (bf1 & BLOCK1_WALL) != BLOCK1_NONE &&
                        (bf1 & BLOCK1_FLOOR) == BLOCK1_NONE))
                    {
                        newBool = TRUE;
                    }
                    else
                    {
                        newBool = FALSE;
                    }
                    if (newBool)
                    {
                        Level_SetPointer_FromSurfaceType(theBlock->terrain);
                    }

                    goto wowlabel;
                }

                if (leftReleased != 0)
                {
                    theBlock->clickCount++;
                }

                if ((legoGlobs.flags3 & GAME3_PICKUPOBJECT) != GAME3_NONE || (legoGlobs.flags3 & GAME3_LOADVEHICLE) != GAME3_NONE)
                {
                    if ((legoGlobs.flags3 & GAME3_PICKUPOBJECT) == GAME3_NONE)
                    {
                        pointer = Pointer_VehicleCantPickUp;
                    }
                    else
                    {
                        pointer = Pointer_LegoManCantPickUp;
                    }

                    // TODO: Implement Lego_HandleWorld
                    // goto
                }

                if ((legoGlobs.flags3 & GAME3_UNK_20) == GAME3_NONE && ((legoGlobs.flags3 & GAME3_UNK_40) == GAME3_NONE))
                {
                    if ((legoGlobs.flags3 & GAME3_UNK_2) == GAME3_NONE && (legoGlobs.flags3 & GAME3_UNK_4) == GAME3_NONE)
                    {
                        if ((legoGlobs.flags3 & GAME3_UNK_1) == GAME3_NONE)
                        {
                            BlockFlags1 blockFlags1;
                            if (mouseBlockX < legoGlobs.currLevel->width - 1 && mouseBlockY < legoGlobs.currLevel->height - 1)
                            {
                                blockFlags1 = legoGlobs.currLevel->blocks[mouseBlockY * legoGlobs.currLevel->width + mouseBlockX].flags1 & BLOCK1_SURVEYED;
                            }
                            else
                            {
                                blockFlags1 = BLOCK1_NONE;
                            }

                            if (blockFlags1 != BLOCK1_NONE)
                            {
                                B32 isFloor;
                                if (mouseBlockX < legoGlobs.currLevel->width - 1 && mouseBlockY < legoGlobs.currLevel->height - 1 &&
                                    (blockFlags1 = legoGlobs.currLevel->blocks[mouseBlockY * legoGlobs.currLevel->width + mouseBlockX].flags1, (blockFlags1 & BLOCK1_WALL) != BLOCK1_NONE) &&
                                    (blockFlags1 & BLOCK1_FLOOR) == BLOCK1_NONE)
                                {
                                    isFloor = TRUE;
                                }
                                else
                                {
                                    isFloor = FALSE;
                                }

                                if (isFloor && (legoGlobs.currLevel->blocks[legoGlobs.currLevel->width * legoGlobs.mouseBlockPos.y + legoGlobs.mouseBlockPos.x].terrain == Lego_SurfaceType8_Immovable || legoGlobs.currLevel->blocks[legoGlobs.currLevel->width * legoGlobs.mouseBlockPos.y + legoGlobs.mouseBlockPos.x].terrain == Lego_SurfaceType8_RechargeSeam))
                                {
                                    if ((theBlock->flags1 & BLOCK1_FLOOR) == BLOCK1_NONE)
                                    {
                                        Level_SetPointer_FromSurfaceType(theBlock->terrain);
                                    }

                                    if (gamectrlGlobs.mslb_Last_6 == 0)
                                    {
                                        Lego_ShowBlockToolTip(&legoGlobs.mouseBlockPos, TRUE, TRUE, FALSE);
                                    }

                                    if (leftReleased != 0)
                                    {
                                        Lego_SetPointerSFX(PointerSFX_NotOkay_ImmovableRock);
                                    }
                                }
                                else
                                {
                                    B32 someBool = FALSE;
                                    if ((theBlock->flags1 & BLOCK1_FLOOR) == BLOCK1_NONE)
                                    {
                                        Level_SetPointer_FromSurfaceType(theBlock->terrain);
                                    }

                                    if (gamectrlGlobs.mslb_Last_6 == 0)
                                    {
                                        Lego_ShowBlockToolTip(&legoGlobs.mouseBlockPos, TRUE, TRUE, FALSE);
                                    }

                                    if (Message_AnyUnitSelected())
                                    {
                                        if ((legoGlobs.flags1 & GAME1_VERTEXMODE) == GAME1_NONE)
                                        {
                                            // TODO: Implement Lego_HandleWorld
                                        }
                                        else if (theBlock->predug == Lego_PredugType_Cavern_Exposed || !Level_FindSelectedUnit_BlockCheck_FUN_00431960(mouseBlockX, mouseBlockY, TRUE))
                                        {
                                            legoGlobs.bool_c8 = FALSE;
                                        }
                                        else
                                        {
                                            // TODO: Implement Lego_HandleWorld
                                        }

                                        if (!someBool && (theBlock->flags1 & BLOCK1_FLOOR) != BLOCK1_NONE)
                                        {
                                            // TODO: Implement Lego_HandleWorld
                                        }
                                    }

                                    if (!someBool)
                                    {
                                        B32 someBool2;
                                        U32 idx = legoGlobs.currLevel->width * mouseBlockY + mouseBlockX;
                                        if ((legoGlobs.currLevel->blocks[idx].flags1 & BLOCK1_BUILDINGSOLID) == BLOCK1_NONE && ((legoGlobs.currLevel->blocks[idx].flags2 & BLOCK2_TOOLSTORE) == BLOCK2_NONE))
                                        {
                                            someBool2 = FALSE;
                                        }
                                        else
                                        {
                                            someBool2 = TRUE;
                                        }

                                        B32 someBool3 = FALSE;
                                        Lego_SurfaceType8 surfType;
                                        if (!someBool2 && (surfType = theBlock->terrain, surfType != Lego_SurfaceType8_Lava && surfType != Lego_SurfaceType8_Lake) && (theBlock->flags2 & BLOCK2_SLUGHOLE_EXPOSED) == BLOCK2_NONE)
                                        {
                                            Interface_MenuType menuType;
                                            if ((theBlock->flags1 & BLOCK1_FLOOR) != BLOCK1_NONE)
                                            {
                                                B32 someBool4 = FALSE;
                                                B32 someBool5 = FALSE;
                                                if ((legoGlobs.currLevel->blocks[legoGlobs.currLevel->width * legoGlobs.mouseBlockPos.y + legoGlobs.mouseBlockPos.x].flags1 & BLOCK1_ERODEACTIVE) != BLOCK1_NONE)
                                                {
                                                    someBool4 = TRUE;
                                                    someBool5 = TRUE;
                                                    if (leftReleased != 0 && !Level_BlockPointerCheck(&legoGlobs.mouseBlockPos))
                                                    {
                                                        Message_AddMessageAction(Message_ClearSelection, NULL, 0, NULL);
                                                        Lego_SetPointerSFX(PointerSFX_Okay_Floor);
                                                        Interface_OpenMenu_FUN_0041b200(Interface_Menu_Erode, &legoGlobs.mouseBlockPos);
                                                    }
                                                }
                                                if (!someBool4 && (theBlock->flags1 & BLOCK1_BUILDINGPATH) != BLOCK1_NONE && (someBool5 = TRUE, leftReleased != 0))
                                                {
                                                    Lego_SetPointerSFX(PointerSFX_NotOkay);
                                                }

                                                // TODO: Verify if this if statement is correct
                                                if (((theBlock->flags1 & BLOCK1_FOUNDATION) != BLOCK1_NONE ||
                                                    ((legoGlobs.currLevel->blocks[legoGlobs.currLevel->width * legoGlobs.mouseBlockPos.y + legoGlobs.mouseBlockPos.x].flags1 & BLOCK1_LAYEDPATH) != BLOCK1_NONE)) &&
                                                    ((legoGlobs.currLevel->blocks[legoGlobs.currLevel->width * legoGlobs.mouseBlockPos.y + legoGlobs.mouseBlockPos.x].flags1 & BLOCK1_BUILDINGPATH) == BLOCK1_NONE))
                                                {
                                                    BlockFlags1 newBlockFlags1;
                                                    if ((legoGlobs.mouseBlockPos.x < 0 || legoGlobs.mouseBlockPos.y < 0) ||
                                                        (legoGlobs.currLevel->width <= legoGlobs.mouseBlockPos.x || legoGlobs.currLevel->height <= legoGlobs.mouseBlockPos.y))
                                                    {
                                                        newBlockFlags1 = BLOCK1_NONE;
                                                    }
                                                    else
                                                    {
                                                        newBlockFlags1 = legoGlobs.currLevel->blocks[legoGlobs.mouseBlockPos.y * legoGlobs.currLevel->width + legoGlobs.mouseBlockPos.x].flags1 & BLOCK1_PATH;
                                                    }

                                                    if (newBlockFlags1 == BLOCK1_NONE && (someBool5 = TRUE, leftReleased != 0) && !Level_BlockPointerCheck(&legoGlobs.mouseBlockPos))
                                                    {
                                                        Lego_SetPointerSFX(PointerSFX_Okay_Floor);
                                                        Interface_OpenMenu_FUN_0041b200(Interface_Menu_Construction, &legoGlobs.mouseBlockPos);
                                                    }
                                                }

                                                if (someBool5)
                                                {
                                                    goto wowlabel;
                                                }

                                                B32 someBool6;
                                                if ((legoGlobs.currLevel->blocks[legoGlobs.currLevel->width * legoGlobs.mouseBlockPos.y + legoGlobs.mouseBlockPos.x].flags1 & BLOCK1_LAYEDPATH) == BLOCK1_NONE)
                                                {
                                                    BlockFlags1 newBlockFlags2;
                                                    if (legoGlobs.mouseBlockPos.x < 0 || legoGlobs.mouseBlockPos.y < 0 || legoGlobs.currLevel->width <= legoGlobs.mouseBlockPos.x || legoGlobs.currLevel->height <= legoGlobs.mouseBlockPos.y)
                                                    {
                                                        newBlockFlags2 = BLOCK1_NONE;
                                                    }
                                                    else
                                                    {
                                                        newBlockFlags2 = legoGlobs.currLevel->blocks[legoGlobs.mouseBlockPos.y * legoGlobs.currLevel->width + legoGlobs.mouseBlockPos.x].flags1 & BLOCK1_PATH;
                                                    }

                                                    if (newBlockFlags2 != BLOCK1_NONE)
                                                        goto whatlabel;
                                                    someBool6 = FALSE;
                                                }
                                                else
                                                {
whatlabel:
                                                    someBool6 = TRUE;
                                                    if (leftReleased != 0 && !Level_BlockPointerCheck(&legoGlobs.mouseBlockPos))
                                                    {
                                                        Message_AddMessageAction(Message_ClearSelection, NULL, 0, NULL);
                                                        Lego_SetPointerSFX(PointerSFX_Okay_Floor);
                                                        Interface_OpenMenu_FUN_0041b200(Interface_Menu_Ground, &legoGlobs.mouseBlockPos);
                                                    }
                                                }

                                                if (!someBool6)
                                                {
                                                    B32 someBool7 = FALSE;
                                                    if ((theBlock->flags1 & BLOCK1_CLEARED) != BLOCK1_NONE && (someBool7 = TRUE, leftReleased != 0) &&
                                                        !Level_BlockPointerCheck(&legoGlobs.mouseBlockPos))
                                                    {
                                                        Message_AddMessageAction(Message_ClearSelection, NULL, 0, NULL);
                                                        Lego_SetPointerSFX(PointerSFX_Okay_Floor);
                                                        Interface_OpenMenu_FUN_0041b200(Interface_Menu_Ground, &legoGlobs.mouseBlockPos);
                                                    }
                                                    if (!someBool7)
                                                    {
                                                        someBool3 = FALSE;
                                                        if ((theBlock->flags1 & BLOCK1_CLEARED) != BLOCK1_NONE || (someBool3 = TRUE, leftReleased == 0) ||
                                                            Level_BlockPointerCheck(&legoGlobs.mouseBlockPos))
                                                        {
                                                            goto thinglabel;
                                                        }
                                                        Message_AddMessageAction(Message_ClearSelection, NULL, 0, NULL);
                                                        Lego_SetPointerSFX(PointerSFX_Okay_Floor);
                                                        menuType = Interface_Menu_Rubble;
                                                        goto unklabel;
                                                    }
                                                }

                                                goto wowlabel;
                                            }

                                            if (surfType == Lego_SurfaceType8_Immovable || (theBlock->flags1 & BLOCK1_INCORNER) != BLOCK1_NONE ||
                                                (someBool3 = TRUE, leftReleased == 0 || Level_BlockPointerCheck(&legoGlobs.mouseBlockPos)))
                                            {
                                                goto thinglabel;
                                            }
                                            Message_AddMessageAction(Message_ClearSelection, NULL, 0, NULL);
                                            Lego_SetPointerSFX(PointerSFX_Okay_Wall);
                                            menuType = Interface_Menu_Wall;
unklabel:
                                            someBool3 = TRUE;
                                            Interface_OpenMenu_FUN_0041b200(menuType, &legoGlobs.mouseBlockPos);
                                        }

thinglabel:
                                        if (!someBool3)
                                        {
                                            goto herething;
                                        }
                                    }
                                }

                                goto wowlabel;
                            }
                        }
                        else
                        {
                            legoGlobs.flags1 |= GAME1_UNK_200;
                            Pointer_SetCurrent_IfTimerFinished(Pointer_CantHelp);
                            Text_DisplayMessage(Text_Encyclopedia, FALSE, FALSE);
                        }

herething:
                        if (leftReleased != 0)
                        {
                            Lego_SetPointerSFX(PointerSFX_NotOkay);
                        }
                    }
                    else
                    {
                        // TODO: Implement Lego_HandleWorld
                    }
                }
                else
                {
                    // TODO: Implement Lego_HandleWorld
                }
            }
        }
        else
        {
            // TODO: Implement Lego_HandleWorld
        }
    }

wowlabel:
    // TODO: Implement Lego_HandleWorld

    if ((legoGlobs.flags2 & GAME2_ALLOWDEBUGKEYS) != GAME2_NONE)
    {
        Lego_HandleDebugKeys(mouseBlockX, mouseBlockY, mouseObj, gamectrlGlobs.handleWorldNoMouseButtonsElapsed);
    }

nwlabel:

    // TODO: Implement Lego_HandleWorld

    if (gamectrlGlobs.mslb_Last_6 == 0)
    {
        legoGlobs.flags1 &= ~(GAME1_MULTISELECT | GAME1_UNK_200);
    }

    gamectrlGlobs.mslb_Last_0 = inputGlobs.mslb;
    gamectrlGlobs.mslr_Last_0 = inputGlobs.msrb;
}

void Lego_UpdateTopdownCamera(F32 elapsedAbs)
{
    if (legoGlobs.viewMode != ViewMode_Top)
        return;

    // TODO: Implement Lego_UpdateTopdownCamera
    // TODO: handle tutorial
    //if (tutorial)
    //    return;

    if (((legoGlobs.MouseScrollBorder <= inputGlobs.msx && legoGlobs.MouseScrollBorder <= inputGlobs.msy) &&
        inputGlobs.msx <= (S32)(mainGlobs.appWidth - legoGlobs.MouseScrollBorder)) && inputGlobs.msy <= (S32)(mainGlobs.appHeight - legoGlobs.MouseScrollBorder))
    {
        // Stop camera moving if mouse isn't in the scroll indent region around the window border.
        Camera_StopMovement(legoGlobs.cameraMain);
        return;
    }

    Point3F camWorldPos;
    Camera_GetTopdownWorldPos(legoGlobs.cameraMain, legoGlobs.currLevel->map, &camWorldPos);

    Point3F mouseWorldPos;
    Lego_MainView_MouseTransform(inputGlobs.msx, inputGlobs.msy, &mouseWorldPos.x, &mouseWorldPos.y);


    Point3F mouseCamWorldPos;
    mouseCamWorldPos.x = mouseWorldPos.x - camWorldPos.x;
    mouseCamWorldPos.y = mouseWorldPos.y - camWorldPos.y;
    camWorldPos.z = 0.0f;
    mouseWorldPos.z = 0.0f;
    mouseCamWorldPos.z = 0.0f;

    Camera_Move(legoGlobs.cameraMain, &mouseCamWorldPos, elapsedAbs);
}

void Lego_MainView_MouseTransform(U32 mouseX, U32 mouseY, F32 *outXPos, F32* outYPos)
{
    // TODO: Cleanup Decompiled code

    Point3F local_4c;
    Point3F local_40;
    Point3F local_34;
    Point3F local_28;
    Point3F local_1c;
    Point4F local_10;

    local_28.x = 0.0f;
    local_28.y = 0.0f;
    local_28.z = 0.0f;

    local_4c.x = (F32)mouseY;
    local_4c.y = 0.0f;

    local_34.x = 0.0f;
    local_34.y = 0.0f;
    local_34.z = -1.0f;

    local_10.x = (F32)(U64)mouseX;
    local_10.y = (F32)(U64)mouseY;
    local_10.z = 0.0f;
    local_10.w = 1.0f;

    Viewport_InverseTransform(legoGlobs.viewMain,&local_40,&local_10);
    local_10.z = 1.0f;
    local_10.w = 1.0f;
    local_10.x = (F32)(U64)mouseX;
    local_10.y = (F32)(U64)mouseY;

    Viewport_InverseTransform(legoGlobs.viewMain,&local_4c,&local_10);
    local_4c.x = local_4c.x - local_40.x;
    local_4c.y = local_4c.y - local_40.y;
    local_4c.z = local_4c.z - local_40.z;

    if (Maths_RayPlaneIntersection(&local_1c,&local_40,&local_4c,&local_28,&local_34)) {
        *outXPos = local_1c.x;
        *outYPos = local_1c.y;
    }
}

void Lego_XYCallback_AddVisibleSmoke(S32 bx, S32 by)
{
    // TODO: Implement Lego_XYCallback_AddVisibleSmoke
}

void Lego_UnkUpdateMapsWorldUnk_FUN_004290d0(F32 elapsedAbs, B32 pass2)
{
    if (!pass2)
    {
        if (legoGlobs.viewMode == ViewMode_Top)
        {
            Point3F pos;
            Container_GetPosition(legoGlobs.cameraMain->cont2, NULL, &pos);

            Point2I blockPos;
            Map3D_WorldToBlockPos_NoZ(legoGlobs.currLevel->map, pos.x, pos.y, &blockPos.x, &blockPos.y);

            Point3F pos2;
            Container_GetOrientation(legoGlobs.cameraMain->contCam, NULL, &pos2, NULL);
            F32 blockSize = Map3D_BlockSize(Lego_GetMap());
            blockSize *= 3.0f;

            pos2.x *= blockSize;
            pos2.y *= blockSize;
            pos2.z *= blockSize;

            pos.x += pos2.x;
            pos.y += pos2.y;
            pos.z += pos2.z;

            Map3D_WorldToBlockPos_NoZ(legoGlobs.currLevel->map, pos.x, pos.y, &blockPos.x, &blockPos.y);
            Map3D_AddVisibleBlocksInRadius_AndDoCallbacks(legoGlobs.currLevel->map, blockPos.x, blockPos.y, 8, Lego_XYCallback_AddVisibleSmoke);
        }
        else if (legoGlobs.viewMode == ViewMode_FP)
        {
            // TODO: Implement Lego_UnkUpdateMapsWorldUnk_FUN_004290d0
        }
    } else if ((legoGlobs.flags1 & GAME1_RADARON) != GAME1_NONE && (legoGlobs.flags1 & GAME1_RADAR_TRACKOBJECTVIEW) != GAME1_NONE)
    {
        Point3F pos;
        Container_GetPosition(legoGlobs.cameraTrack->cont2, NULL, &pos);
        Point2I blockPos;
        Map3D_WorldToBlockPos_NoZ(legoGlobs.currLevel->map, pos.x, pos.y, &blockPos.x, &blockPos.y);
        Map3D_AddVisibleBlocksInRadius_AndDoCallbacks(legoGlobs.currLevel->map, blockPos.x, blockPos.y, 2, NULL);
    }
}

B32 Lego_LoadLevel(const char* levelName)
{
    Front_PlayLevelMovie(levelName, TRUE);
    Teleporter_Restart();
    Front_UpdateOptionsSliders();
    HelpWindow_IfFlag4_AndParam_Clear1_Set2_Else_Clear3(FALSE);
    Construction_DisableCryOreDrop(FALSE);
    LegoObject_SetLevelEnding(FALSE);
    lpSaveData save = Front_Save_GetCurrentSaveData();
    lpSaveStruct_18 saveStruct18;
    if (save == NULL)
        saveStruct18 = NULL;
    else
        saveStruct18 = &save->saveStruct18_1c;

    Object_Save_OverwriteStruct18(saveStruct18);
    S32 saveNumber;
    if (!ObjectRecall_IsLoaded() && (saveNumber = Front_Save_GetSaveNumber(), saveNumber != -1))
    {
        char buff[260];
        sprintf(buff, "%s\\%i.osf", "Saves", saveNumber);
        ObjectRecall_LoadRROSFile(buff);
    }

    lpLego_Level level = Mem_Alloc(sizeof(Lego_Level));
    if (level == NULL)
    {
        Loader_Display_Loading_Bar(NULL); // this line probably isn't needed, in the og source it's just a goto
        legoGlobs.currLevel = NULL;
        return FALSE;
    }

    memset(level, 0, sizeof(Lego_Level));

    legoGlobs.flags1 |= GAME1_LEVELSTART;
    legoGlobs.currLevel = level;

    Loader_Display_Loading_Bar(levelName);

    Priorities_LoadLevel(legoGlobs.config, legoGlobs.gameName, levelName);

    U32 seed = Config_GetIntValue(legoGlobs.config, Config_BuildStringID(legoGlobs.gameName, levelName, "Seed", 0));
    if (seed != 0)
        srand(seed);

    const char* boulderAnimation = Config_GetTempStringValue(legoGlobs.config, Config_BuildStringID(legoGlobs.gameName, levelName, "BoulderAnimation", 0));
    if (boulderAnimation == NULL || _stricmp(boulderAnimation, "Rock") == 0)
        level->BoulderAnimation = TEXTURES_ROCK;
    else if (_stricmp(boulderAnimation, "Lava") == 0)
        level->BoulderAnimation = TEXTURES_LAVA;
    else if (_stricmp(boulderAnimation, "Ice") == 0)
        level->BoulderAnimation = TEXTURES_ICE;
    else
        level->BoulderAnimation = TEXTURES_ROCK;

    legoGlobs.FallinMultiplier = Config_GetIntValue(legoGlobs.config, Config_BuildStringID(legoGlobs.gameName, levelName, "FallinMultiplier", 0));
    if (legoGlobs.FallinMultiplier == 0)
        legoGlobs.FallinMultiplier = 1;

    level->BlockSize = Config_GetFloatValue(legoGlobs.config, Config_BuildStringID(legoGlobs.gameName, levelName, "blocksize", 0));
    level->DigDepth = Config_GetFloatValue(legoGlobs.config, Config_BuildStringID(legoGlobs.gameName, levelName, "digdepth", 0));
    level->UseRoof = Config_GetBoolValue(legoGlobs.config, Config_BuildStringID(legoGlobs.gameName, levelName, "useroof", 0));
    level->RoofHeight = Config_GetFloatValue(legoGlobs.config, Config_BuildStringID(legoGlobs.gameName, levelName, "roofheight", 0));
    level->RoughLevel = Config_GetFloatValue(legoGlobs.config, Config_BuildStringID(legoGlobs.gameName, levelName, "roughlevel", 0));

    const char* surfaceMap = Config_GetStringValue(legoGlobs.config, Config_BuildStringID(legoGlobs.gameName, levelName, "surfacemap", 0));
    const char* preDugMap = Config_GetStringValue(legoGlobs.config, Config_BuildStringID(legoGlobs.gameName, levelName, "predugmap", 0));
    const char* terrainMap = Config_GetStringValue(legoGlobs.config, Config_BuildStringID(legoGlobs.gameName, levelName, "terrainmap", 0));
    const char* blockPointersMap = Config_GetStringValue(legoGlobs.config, Config_BuildStringID(legoGlobs.gameName, levelName, "blockPointersMap", 0));
    const char* cryOreMap = Config_GetStringValue(legoGlobs.config, Config_BuildStringID(legoGlobs.gameName, levelName, "CryOreMap", 0));
    const char* erodeMap = Config_GetStringValue(legoGlobs.config, Config_BuildStringID(legoGlobs.gameName, levelName, "ErodeMap", 0));
    const char* emergeMap = Config_GetStringValue(legoGlobs.config, Config_BuildStringID(legoGlobs.gameName, levelName, "EmergeMap", 0));
    const char* aiMap = Config_GetStringValue(legoGlobs.config, Config_BuildStringID(legoGlobs.gameName, levelName, "AIMap", 0));
    const char* pathMap = Config_GetStringValue(legoGlobs.config, Config_BuildStringID(legoGlobs.gameName, levelName, "PathMap", 0));
    const char* fallinMap = Config_GetStringValue(legoGlobs.config, Config_BuildStringID(legoGlobs.gameName, levelName, "FallinMap", 0));
    const char* textureSet = Config_GetStringValue(legoGlobs.config, Config_BuildStringID(legoGlobs.gameName, levelName, "textureset", 0));

    level->EmergeTimeOut = Config_GetFloatValue(legoGlobs.config, Config_BuildStringID(legoGlobs.gameName, levelName, "EmergeTimeOut", 0));
    if (level->EmergeTimeOut == 0.0f)
        level->EmergeTimeOut = 1500.0f;

    const char* filenameObjectList = Config_GetTempStringValue(legoGlobs.config, Config_BuildStringID(legoGlobs.gameName, levelName, "olistfile", 0));
    const char* filenameEntityListFile = Config_GetTempStringValue(legoGlobs.config, Config_BuildStringID(legoGlobs.gameName, levelName, "elistfile", 0));

    level->SelBoxHeight = Config_GetFloatValue(legoGlobs.config, Config_BuildStringID(legoGlobs.gameName, levelName, "selboxheight", 0));

    const char* filenamePTL = Config_GetTempStringValue(legoGlobs.config, Config_BuildStringID(legoGlobs.gameName, levelName, "PTLFile", 0));
    const char* filenameNERP = Config_GetTempStringValue(legoGlobs.config, Config_BuildStringID(legoGlobs.gameName, levelName, "NERPFile", 0));
    const char* filenameNERPMessage = Config_GetTempStringValue(legoGlobs.config, Config_BuildStringID(legoGlobs.gameName, levelName, "NERPMessageFile", 0));
    const char* nameEmergeCreature = Config_GetTempStringValue(legoGlobs.config, Config_BuildStringID(legoGlobs.gameName, levelName, "EmergeCreature", 0));

    level->SafeCaverns = Config_GetBoolValue(legoGlobs.config, Config_BuildStringID(legoGlobs.gameName, levelName, "SafeCaverns", 0));

    const char* filenameIntroAVI = Config_GetTempStringValue(legoGlobs.config, Config_BuildStringID(legoGlobs.gameName, levelName, "IntroAVI", 0));

    level->StartFP = Config_GetBoolValue(legoGlobs.config, Config_BuildStringID(legoGlobs.gameName, levelName, "StartFP", 0)) == BOOL3_TRUE;
    level->NoDrain = Config_GetBoolValue(legoGlobs.config, Config_BuildStringID(legoGlobs.gameName, levelName, "NoDrain", 0)) == BOOL3_TRUE;

    level->OxygenRate = Config_GetFloatValue(legoGlobs.config, Config_BuildStringID(legoGlobs.gameName, levelName, "OxygenRate", 0));
    level->OxygenRate *= 0.001f;

    F32 upgradeTime = Config_GetFloatValue(legoGlobs.config, Config_BuildStringID(legoGlobs.gameName, levelName, "UpgradeTime", 0));
    upgradeTime *= 25.0f;
    for (S32 i = 0; i < 20; i++) // TODO: should use a define for the count
        level->UpgradeTimes[i] = upgradeTime;

    level->UpgradeTimes[2] = Config_GetFloatValue(legoGlobs.config, Config_BuildStringID(legoGlobs.gameName, levelName, "MinifigureUpgradeTime", 0));
    if (level->UpgradeTimes[2] == 0.0f)
        level->UpgradeTimes[2] = upgradeTime;
    else
        level->UpgradeTimes[2] *= 25.0f;

    level->UpgradeTimes[1] = Config_GetFloatValue(legoGlobs.config, Config_BuildStringID(legoGlobs.gameName, levelName, "VehicleUpgradeTime", 0));
    if (level->UpgradeTimes[1] == 0.0f)
        level->UpgradeTimes[1] = upgradeTime;
    else
        level->UpgradeTimes[1] *= 25.0f;

    level->UpgradeTimes[4] = Config_GetFloatValue(legoGlobs.config, Config_BuildStringID(legoGlobs.gameName, levelName, "BuildingUpgradeTime", 0));
    if (level->UpgradeTimes[4] == 0.0f)
        level->UpgradeTimes[4] = upgradeTime;
    else
        level->UpgradeTimes[4] *= 25.0f;

    level->TrainTime = Config_GetFloatValue(legoGlobs.config, Config_BuildStringID(legoGlobs.gameName, levelName, "TrainTime", 0));
    level->TrainTime *= 25.0f;

    B32 noMultiSelect = Config_GetBoolValue(legoGlobs.config, Config_BuildStringID(legoGlobs.gameName, levelName, "NoMultiSelect", 0)) == BOOL3_TRUE;
    if (noMultiSelect)
        legoGlobs.flags2 |= GAME2_NOMULTISELECT;

    B32 noAutoEat = Config_GetBoolValue(legoGlobs.config, Config_BuildStringID(legoGlobs.gameName, levelName, "NoAutoEat", 0)) == BOOL3_TRUE;
    if (noAutoEat)
        legoGlobs.flags2 |= GAME2_NOAUTOEAT;
    else
        legoGlobs.flags2 &= ~GAME2_NOAUTOEAT;

    legoGlobs.EndGameAVI1 = Config_GetStringValue(legoGlobs.config, Config_BuildStringID(legoGlobs.gameName, levelName, "EndGameAVI1", 0));
    legoGlobs.EndGameAVI2 = Config_GetStringValue(legoGlobs.config, Config_BuildStringID(legoGlobs.gameName, levelName, "EndGameAVI2", 0));

    if (legoGlobs.RenameReplace != NULL)
    {
        B32 allowRename = Config_GetBoolValue(legoGlobs.config, Config_BuildStringID(legoGlobs.gameName, levelName, "AllowRename", 0)) == BOOL3_TRUE;
        if (allowRename)
            legoGlobs.flags2 |= GAME2_ALLOWRENAME;
        else
            legoGlobs.flags2 &= ~GAME2_ALLOWRENAME;
    }

    B32 recallOLObjects = Config_GetBoolValue(legoGlobs.config, Config_BuildStringID(legoGlobs.gameName, levelName, "RecallOLObjects", 0)) == BOOL3_TRUE;
    if (recallOLObjects)
        legoGlobs.flags2 |= GAME2_RECALLOLOBJECTS;
    else
        legoGlobs.flags2 &= ~GAME2_RECALLOLOBJECTS;

    B32 generateSpiders = Config_GetBoolValue(legoGlobs.config, Config_BuildStringID(legoGlobs.gameName, levelName, "GenerateSpiders", 0)) == BOOL3_TRUE;
    if (generateSpiders)
        legoGlobs.flags2 |= GAME2_GENERATESPIDERS;
    else
        legoGlobs.flags2 &= ~GAME2_GENERATESPIDERS;

    B32 disableToolTipSound = Config_GetBoolValue(legoGlobs.config, Config_BuildStringID(legoGlobs.gameName, levelName, "DisableToolTipSound", 0)) == BOOL3_TRUE;
    if (disableToolTipSound)
        legoGlobs.flags2 |= GAME2_DISABLETOOLTIPSOUND;
    else
        legoGlobs.flags2 &= ~GAME2_DISABLETOOLTIPSOUND;

    B32 disableEndTeleport = Config_GetBoolValue(legoGlobs.config, Config_BuildStringID(legoGlobs.gameName, levelName, "DisableEndTeleport", 0)) == BOOL3_TRUE;
    Objective_SetEndTeleportEnabled(!disableEndTeleport);

    const char* dragBoxRGB = Config_GetStringValue(legoGlobs.config, Config_BuildStringID(legoGlobs.gameName, levelName, "DragBoxRGB", 0));
    if (dragBoxRGB != NULL)
    {
        const char* parts[10];
        S32 partCount = Util_Tokenize(dragBoxRGB, parts, ":");

        if (partCount == 3)
        {
            legoGlobs.DragBoxRGB.red = atoi(parts[0]) * 0.003921569f;
            legoGlobs.DragBoxRGB.green = atoi(parts[1]) * 0.003921569f;
            legoGlobs.DragBoxRGB.blue = atoi(parts[2]) * 0.003921569f;
        }

        Mem_Free(dragBoxRGB);
    }

    level->BuildingTolerance = Config_GetFloatValue(legoGlobs.config, Config_BuildStringID(legoGlobs.gameName, levelName, "BuildingTolerance", 0));
    level->BuildingMaxVariation = Config_GetFloatValue(legoGlobs.config, Config_BuildStringID(legoGlobs.gameName, levelName, "BuildingMaxVariation", 0));
    if (level->BuildingTolerance == 0.0f)
        level->BuildingTolerance = 4.0f;
    if (level->BuildingMaxVariation == 0.0f)
        level->BuildingMaxVariation = 6.0f;

    level->MaxStolen = Config_GetIntValue(legoGlobs.config, Config_BuildStringID(legoGlobs.gameName, levelName, "MaxStolen", 0));

    const char* slug = Config_GetTempStringValue(legoGlobs.config, Config_BuildStringID(legoGlobs.gameName, levelName, "Slug", 0));
    if (slug == NULL)
    {
        level->Slug = 20;
    }
    else
    {
        LegoObject_Type slugType;
        if (Lego_GetObjectByName(slug, &slugType, (LegoObject_ID*)&level->Slug, NULL) != 0)
        {
            level->SlugTime = Config_GetFloatValue(legoGlobs.config, Config_BuildStringID(legoGlobs.gameName, levelName, "SlugTime", 0));
            if (level->SlugTime == 0.0f)
                level->SlugTime = 60.0f;
            level->SlugTime *= 25.0f;

            F32 initialSlugTime = Config_GetFloatValue(legoGlobs.config, Config_BuildStringID(legoGlobs.gameName, levelName, "InitialSlugTime", 0));
            if (initialSlugTime == 0.0f)
                initialSlugTime = 60.0f;

            legoGlobs.InitialSlugTime = initialSlugTime * 25.0f;
        }
    }

    const char* rockFallStyle = Config_GetTempStringValue(legoGlobs.config, Config_BuildStringID(legoGlobs.gameName, levelName, "RockFallStyle", 0));
    U32 rockFallStyleNum;
    Effect_GetRockFallStyle(rockFallStyle, &rockFallStyleNum);
    Effect_SetRockFallStyle(rockFallStyleNum);

    legoGlobs.IsFallinsEnabled = (Config_GetBoolValue(legoGlobs.config, Config_BuildStringID(legoGlobs.gameName, levelName, "noFallins", 0)) != BOOL3_TRUE);

    Panel_Crystals_LoadRewardQuota(legoGlobs.config, legoGlobs.gameName, levelName);

    LegoObject_Type emergeCreatureType;
    Lego_GetObjectByName(nameEmergeCreature, &emergeCreatureType, (LegoObject_ID*)&level->EmergeCreature, NULL);

    if (!Config_GetRGBValue(legoGlobs.config, Config_BuildStringID(legoGlobs.gameName, levelName, "FogColourRGB", 0), &legoGlobs.FogColourRGB.red, &legoGlobs.FogColourRGB.green, &legoGlobs.FogColourRGB.blue))
    {
        legoGlobs.flags1 &= ~GAME1_FOGCOLOURRGB;
    }
    else
    {
        if (!Config_GetRGBValue(legoGlobs.config, Config_BuildStringID(legoGlobs.gameName, levelName, "HighFogColourRGB", 0), &legoGlobs.HighFogColourRGB.red, &legoGlobs.HighFogColourRGB.green, &legoGlobs.HighFogColourRGB.blue))
        {
            legoGlobs.flags1 &= ~GAME1_HIGHFOGCOLOURRGB;
        }
        else
        {
            legoGlobs.flags1 |= GAME1_HIGHFOGCOLOURRGB;
            legoGlobs.float_364 = 1.0f;

            F32 fogRate = Config_GetFloatValue(legoGlobs.config, Config_BuildStringID(legoGlobs.gameName, levelName, "FogRate", 0));
            if (fogRate == 0.0f)
                legoGlobs.FogRate = 1500.0f;
            else
                legoGlobs.FogRate = fogRate * 25.0f;
        }

        Container_SetFogColour(legoGlobs.FogColourRGB.red, legoGlobs.FogColourRGB.green, legoGlobs.FogColourRGB.blue);
        Container_SetFogMode(D3DRMFOGMETHOD_TABLE);
        Container_SetFogParams(0.0f, legoGlobs.currLevel->BlockSize * legoGlobs.FPClipBlocks, 0.0032f);

        legoGlobs.flags1 |= GAME1_FOGCOLOURRGB;
    }

    level->nextLevelID = Config_GetStringValue(legoGlobs.config, Config_BuildStringID(legoGlobs.gameName, levelName, "nextlevel", 0));
    level->FullName = Config_GetStringValue(legoGlobs.config, Config_BuildStringID(legoGlobs.gameName, levelName, "FullName", 0));
    if (level->FullName == NULL)
    {
        level->FullName = NULL; // This was like this in the decompile
    } else
    {
        U32 i = 0;
        if (strlen(level->FullName) > 0)
        {
            do
            {
                if (level->FullName[i] == '_')
                    level->FullName[i] = ' ';
                i++;
            } while (level->FullName[i] != '\0');
        }
    }

    Objective_LoadLevel(legoGlobs.config, legoGlobs.gameName, levelName, level, mainGlobs.appWidth, mainGlobs.appHeight);

    F32 erodeTriggerTime = Config_GetFloatValue(legoGlobs.config, Config_BuildStringID(legoGlobs.gameName, levelName, "ErodeTriggerTime", 0));
    if (erodeTriggerTime == 0.0f)
        erodeTriggerTime = 10.0f;

    F32 erodeErodeTime = Config_GetFloatValue(legoGlobs.config, Config_BuildStringID(legoGlobs.gameName, levelName, "ErodeErodeTime", 0));
    if (erodeErodeTime == 0.0f)
        erodeErodeTime = 5.0f;

    F32 erodeLockTime = Config_GetFloatValue(legoGlobs.config, Config_BuildStringID(legoGlobs.gameName, levelName, "ErodeLockTime", 0));
    if (erodeLockTime == 0.0f)
        erodeLockTime = 60.0f;

    Erode_Initialize(erodeTriggerTime, erodeErodeTime, erodeLockTime);

    S32 numberOfLandSlidesTillCaveIn = Config_GetIntValue(legoGlobs.config, Config_BuildStringID(legoGlobs.gameName, levelName, "NumberOfLandSlidesTillCaveIn", 0));
    if (numberOfLandSlidesTillCaveIn == 0)
        numberOfLandSlidesTillCaveIn = 3;

    Fallin_Initialize(numberOfLandSlidesTillCaveIn);

    level->name = levelName;
    level->IsStartTeleportEnabled = (Config_GetBoolValue(legoGlobs.config, Config_BuildStringID(legoGlobs.gameName, levelName, "DisableStartTeleport", 0)) != BOOL3_TRUE);
    level->oxygenLevelMessage = 100.0f;
    level->oxygenLevel = 100.0f;
    level->crystals = 0;
    level->ore = 0;
    level->unused_crystals_90 = 0;
    level->unused_ore_a4 = 0;
    level->crystalsPickedUp = 0;
    level->orePickedUp = 0;
    level->status = LEVELSTATUS_INCOMPLETE;

    legoGlobs.placeDestSmallTeleporter = NULL;
    legoGlobs.placeDestBigTeleporter = NULL;

    NERPsFile_LoadScriptFile(filenameNERP);
    NERPsFile_LoadMessageFile(filenameNERPMessage);

    if (PTL_Initialize(filenamePTL, legoGlobs.gameName))
    {
        char* stringParts[2];
        //memset(stringParts, 0, sizeof(stringParts));
        LegoObject_Type legoObjectType = LegoObject_None;
        S32 terrainParam = 0;
        S32 blockPointersParam = 0;
        S32 cryOreParam = 0;
        S32 pathParam = 0;

        if (surfaceMap != NULL && Util_Tokenize(surfaceMap, stringParts, ":") == 2)
            atoi(stringParts[1]);

        if (preDugMap != NULL && Util_Tokenize(preDugMap, stringParts, ":") == 2)
            legoObjectType = atoi(stringParts[1]);

        if (terrainMap != NULL && Util_Tokenize(terrainMap, stringParts, ":") == 2)
            terrainParam = atoi(stringParts[1]);

        if (blockPointersMap != NULL && Util_Tokenize(blockPointersMap, stringParts, ":") == 2)
            blockPointersParam = atoi(stringParts[1]);

        if (cryOreMap != NULL && Util_Tokenize(cryOreMap, stringParts, ":") == 2)
            cryOreParam = atoi(stringParts[1]);

        if (pathMap != NULL && Util_Tokenize(pathMap, stringParts, ":") == 2)
            pathParam = atoi(stringParts[1]);

        if (fallinMap != NULL && Util_Tokenize(fallinMap, stringParts, ":") == 2)
            atoi(stringParts[1]);

        if (emergeMap != NULL && Util_Tokenize(emergeMap, stringParts, ":") == 2)
            pathParam = atoi(stringParts[1]);

        B32 success = Lego_LoadMapSet(level,
                                      surfaceMap,
                                      preDugMap, legoObjectType,
                                      terrainMap, terrainParam,
                                      blockPointersMap, blockPointersParam,
                                      cryOreMap, cryOreParam,
                                      erodeMap,
                                      pathMap, pathParam,
                                      textureSet,
                                      emergeMap, aiMap, fallinMap);

        if (success != 0)
        {
            Mem_Free(surfaceMap);
            Mem_Free(preDugMap);
            Mem_Free(terrainMap);
            Mem_Free(blockPointersMap);
            Mem_Free(cryOreMap);
            Mem_Free(erodeMap);
            Mem_Free(emergeMap);
            Mem_Free(aiMap);
            Mem_Free(fallinMap);

            if (pathMap != NULL)
                Mem_Free(pathMap);

            ElectricFence_Restart(level);
            SpiderWeb_Restart(level);
            Objective_SetStatus(LEVELSTATUS_INCOMPLETE);
            Objective_Update(legoGlobs.textWnd_80, legoGlobs.currLevel, 1.0f, 1.0f);

            if (Lego_LoadOLObjectList(level, filenameObjectList) != 0)
            {
                SearchAddCryOre_c searchAddCryOre;
                searchAddCryOre.crystalCount = Config_GetIntValue(legoGlobs.config, Config_BuildStringID(legoGlobs.gameName, levelName, "InitialCrystals", 0));
                searchAddCryOre.oreCount = Config_GetIntValue(legoGlobs.config, Config_BuildStringID(legoGlobs.gameName, levelName, "InitialOre", 0));
                LegoObject_RunThroughListsSkipUpgradeParts(Level_AddCryOreToToolStore, &searchAddCryOre);

                Loader_Display_Loading_Bar(NULL);

                Point2F coords;
                coords.x = 350.0f;
                coords.y = 70.0f;
                Lego_PlayMovie_old(filenameIntroAVI, &coords);

                AITask_Game_SetNoGather(Config_GetBoolValue(legoGlobs.config, Config_BuildStringID(legoGlobs.gameName, levelName, "NoGather", 0)) == BOOL3_TRUE);

                Info_SetFlag4(TRUE);

                if (level->StartFP == 0 || objectGlobs.minifigureObj_9cb8 == NULL)
                {
                    Lego_SetViewMode(ViewMode_Top, NULL, 0);
                }
                else
                {
                    Lego_SetViewMode(ViewMode_FP, objectGlobs.minifigureObj_9cb8, 1);
                    Message_AddMessageAction(Message_Select, objectGlobs.minifigureObj_9cb8, 0, NULL);
                    Interface_OpenMenu_FUN_0041b200(Interface_Menu_FP, NULL);
                }

                NERPs_InitBlockPointersTable(level);

                if (Config_GetBoolValue(legoGlobs.config, Config_BuildStringID(legoGlobs.gameName, levelName, "Precreate", 0)) == BOOL3_TRUE)
                {
                    // TODO: What is the point of this?

                    LegoObject* objects[10];
                    for (S32 i = 0; i < 10; i++)
                    {
                        objects[i] = LegoObject_Create(legoGlobs.miniFigureData, LegoObject_MiniFigure, 0);
                    }

                    for (S32 i = 0; i < 10; i++)
                    {
                        LegoObject_Remove(objects[i]);
                    }
                }

                return TRUE;
            }
        }
    }

    Mem_Free(level);

    Mem_Free(surfaceMap);
    Mem_Free(preDugMap);
    Mem_Free(terrainMap);
    Mem_Free(blockPointersMap);
    Mem_Free(cryOreMap);
    Mem_Free(erodeMap);
    Mem_Free(emergeMap);

    Loader_Display_Loading_Bar(NULL);
    legoGlobs.currLevel = NULL;

    Error_Fatal(TRUE, Error_Format("Failed to load level '%s'", levelName));
    
    return FALSE;
}

B32 Lego_LoadMapSet(lpLego_Level level, const char* surfaceMap, const char* predugMap, S32 predugParam, const char* terrainMap, S32 terrainParam, const char* blockPointersMap, S32 blockPointersParam, const char* cryOreMap, S32 cryOreParam, const char* erodeMap, const char* pathMap, S32 pathParam, const char* textureSet, const char* emergeMap, const char* aiMap, const char* fallinMap)
{
    level->map = Map3D_Create(legoGlobs.rootCont, surfaceMap, level->BlockSize, level->RoughLevel);

    if (level->map == NULL)
        return FALSE;

    level->radarMap = RadarMap_Create(level->map, &legoGlobs.radarScreenRect, legoGlobs.radarZoom);

    Map3D_SetPerspectiveCorrectionAll(level->map, TRUE);

    // It seems this entire chain of calls is redundant, as they all cause the exact same effect.
    //
    // Original code likely was assigning roof textures for each surface type.
    //  (But all surface types use the same texture, so we get this.)
    Map3D_SetTextureNoFade(level->map, TEXTURE_ROOF_STD);
    Map3D_SetTextureNoFade(level->map, TEXTURE_ROOF_STD);
    Map3D_SetTextureNoFade(level->map, TEXTURE_ROOF_STD);
    Map3D_SetTextureNoFade(level->map, TEXTURE_ROOF_STD);
    Map3D_SetTextureNoFade(level->map, TEXTURE_ROOF_STD);
    Map3D_SetTextureNoFade(level->map, TEXTURE_ROOF_STD);

    level->width = level->map->gridWidth;
    level->height = level->map->gridHeight;
    ElectricFence_Restart(level);
    SpiderWeb_Restart(level);

    if (level->UseRoof != BOOL3_FALSE)
    {
        Roof_Initialize(legoGlobs.rootCont, level->width, level->height);
        // Roof is only visible in FirstPerson view, make sure we start with it hidden
        Roof_Hide(TRUE);
    }

    if (Lego_LoadTextureSet(level, textureSet) != 0)
    {
        level->blocks = Mem_Alloc(level->height * level->width * sizeof(Lego_Block));
        if (level->blocks != NULL)
        {
            memset(level->blocks, 0, level->height * level->width * sizeof(Lego_Block));

            // TODO: Clean this up
            if (level->height != 0)
            {
                U32 y = 0;
                do
                {
                    U32 x = 0;
                    if (level->width != 0)
                    {
                        do
                        {
                            level->blocks[y * level->width + x].predug = Lego_PredugType_Wall;
                            level->blocks[y * level->width + x].texture = TEXTURE_FLOOR_STD;
                            level->blocks[y * level->width + x].terrain = Lego_SurfaceType8_Tunnel;
                            level->blocks[y * level->width + x].flags1 = BLOCK1_NONE;
                            level->blocks[y * level->width + x].direction = 0;
                            level->blocks[y * level->width + x].activity = NULL;
                            level->blocks[y * level->width + x].damage = 0.0f;
                            level->blocks[y * level->width + x].randomness = Maths_Rand();

                            x++;
                        } while (x < level->width);
                    }
                    y++;
                } while (y < level->height);
            }

            if (Lego_LoadPreDugMap(level, predugMap, predugParam) != 0)
            {
                if (Lego_LoadTerrainMap(level, terrainMap, terrainParam) != 0)
                {
                    Lego_LoadCryOreMap(level, cryOreMap, cryOreParam);
                    Lego_LoadErodeMap(level, erodeMap);
                    Lego_LoadPathMap(level, pathMap, pathParam);
                    Lego_LoadEmergeMap(level, emergeMap);
                    Lego_LoadAIMap(level, aiMap);
                    Lego_LoadFallinMap(level, fallinMap);

                    if (Lego_LoadBlockPointersMap(level, blockPointersMap, blockPointersParam) != 0)
                        level->hasBlockPointers = TRUE;

                    for (U32 y = 0; y < level->height; y++)
                    {
                        for (U32 x = 0; x < level->width; x++)
                        {
                            Level_BlockUpdateSurface(level, x, y, TRUE);
                        }
                    }

                    Map3D_UpdateAllBlockNormals(level->map);

                    Water_Initialize(legoGlobs.rootCont, level);

                    return TRUE;
                }
            }
        }
    }

    return FALSE;
}

B32 Lego_LoadOLObjectList(lpLego_Level level, const char* filename)
{
    B32 firstTime = TRUE;

    F32 objHealth;
    Point2F worldPos;
    F32 heading;
    LegoObject_Type objType;
    S32 objIndex;
    lpContainer objSrcData;

    B32 success = FALSE;
    const char* drivingArray[200];
    const char* drivenByName[200];
    U32 i = 0;
    lpLegoObject* objIter;
    lpLegoObject objects[200];

    lpConfig prop = Config_Load(filename);
    if (prop != NULL)
    {
        objIter = objects;
        memset(legoGlobs.recordObjs, 0, sizeof(legoGlobs.recordObjs));
        legoGlobs.recordObjsCount = 0;

        lpConfig conf = Config_FindArray(prop, Config_BuildStringID(legoGlobs.gameName, 0));
        if (conf != NULL)
        {
            do
            {
                const char* objTypeName = Config_GetStringValue(prop, Config_BuildStringID(legoGlobs.gameName, conf->itemName, "type", 0));
                if (objTypeName == NULL)
                    break;

                lpConfig xposItem = Config_FindItem(prop, Config_BuildStringID(legoGlobs.gameName, conf->itemName, "xpos", 0));
                if (xposItem == NULL)
                    break;

                lpConfig yposItem = Config_FindItem(prop, Config_BuildStringID(legoGlobs.gameName, conf->itemName, "ypos", 0));
                if (yposItem == NULL)
                    break;

                lpConfig headingItem = Config_FindItem(prop, Config_BuildStringID(legoGlobs.gameName, conf->itemName, "heading", 0));
                if (headingItem == NULL)
                    break;

                F32 xpos = Config_GetFloatValue(prop, Config_BuildStringID(legoGlobs.gameName, conf->itemName, "xpos", 0));
                F32 ypos = Config_GetFloatValue(prop, Config_BuildStringID(legoGlobs.gameName, conf->itemName, "ypos", 0));
                F32 headingVal = Config_GetFloatValue(prop, Config_BuildStringID(legoGlobs.gameName, conf->itemName, "heading", 0));

                F32 health = Config_GetFloatValue(prop, Config_BuildStringID(legoGlobs.gameName, conf->itemName, "health", 0));

                objHealth = health;
                if (objHealth == 0.0f)
                    objHealth = 100.0f;

                drivingArray[i] = Config_GetStringValue(prop, Config_BuildStringID(legoGlobs.gameName, conf->itemName, "driving", 0));
                drivenByName[i] = Util_StrCpy(conf->itemName);

                U32 xInt = (U32)(U64)xpos - 1.0f;
                U32 yInt = (U32)(U64)ypos - 1.0f;

                Map3D_BlockToWorldPos(level->map, xInt, yInt, &worldPos.x, &worldPos.y);
                worldPos.x += (((F32)xpos - 1.0f) - ((F32)xInt + 0.5f)) * level->BlockSize;
                worldPos.y -= (((F32)ypos - 1.0f) - ((F32)yInt + 0.5f)) * level->BlockSize;
                heading = (F32)headingVal * 0.005555556f * M_PI;
                if (!Lego_GetObjectByName(objTypeName, &objType, &objIndex, &objSrcData))
                    break;

                lpLegoObject someObj = NULL;
                if (objType == LegoObject_TVCamera)
                {
                    Camera_SetTopdownPosition(legoGlobs.cameraMain, worldPos.x, worldPos.y);
                    Camera_SetRotation(legoGlobs.cameraMain, heading);
                    Camera_SetTilt(legoGlobs.cameraMain, 0.8f);

                    legoGlobs.radarCenter.x = worldPos.x;
                    legoGlobs.radarCenter.y = worldPos.y;
                    legoGlobs.radarZoom = 15.0f;
                }
                else
                {
                    BlockFlags1 flags1;
                    if (xInt < legoGlobs.currLevel->width - 1 && yInt < legoGlobs.currLevel->height - 1)
                        flags1 = legoGlobs.currLevel->blocks[yInt * legoGlobs.currLevel->width + xInt].flags1 & BLOCK1_FLOOR;
                    else
                        flags1 = BLOCK1_NONE;

                    if (flags1 == BLOCK1_NONE)
                    {
                        Point2F hiddenWorldPos;
                        hiddenWorldPos.x = worldPos.x;
                        hiddenWorldPos.y = worldPos.y;
                        HiddenObject_Add(objSrcData, objType, objIndex, &hiddenWorldPos, heading, objHealth, drivenByName[i], drivingArray[i]);
                    }
                    else if (objType == LegoObject_Building)
                    {
                        Point2I shapeTranslation;
                        shapeTranslation.x = xInt;
                        shapeTranslation.y = yInt;

                        U32 shapeCount;
                        Point2I* shapePoints = Building_GetShapePoints(&legoGlobs.buildingData[objIndex], &shapeCount);

                        // TODO: Verify and cleanup this code
                        S64 num = (S64)(heading * 0.15915494f * 8.0f);
                        U32 rotation = ((U32)num & 1) + (U32)num >> 1;;

                        Point2I* shapeBlocks = SelectPlace_TransformShapePoints(&shapeTranslation, shapePoints, shapeCount, rotation);

                        lpLegoObject building = Construction_SpawnBuilding(objIndex, &shapeTranslation, rotation, shapeBlocks, shapeCount, level->IsStartTeleportEnabled);
                        if (building != NULL)
                        {
                            LegoObject_FUN_00438720(building);
                        }
                    }
                    else
                    {
                        someObj = LegoObject_Create((void**)objSrcData, objType, objIndex);
                    }
                }

                *objIter = someObj;
                if (someObj != NULL)
                {
                    if (firstTime)
                    {
                        F32 trackRotationSpeed = 0.01f;
                        F32 trackTilt = 0.7f;
                        F32 trackDist = StatsObject_GetTrackDist(someObj);
                        Camera_TrackObject(legoGlobs.cameraTrack, someObj, 2.0f, trackDist, trackTilt, trackRotationSpeed);
                    }
                    firstTime = FALSE;

                    if (objType != LegoObject_Building)
                    {
                        LegoObject_SetPositionAndHeading(someObj, worldPos.x, worldPos.y, heading, TRUE);
                    }

                    if (objType == LegoObject_PowerCrystal || objType == LegoObject_Ore || objType == LegoObject_Dynamite || objType == LegoObject_Barrier)
                    {
                        AITask_DoCollect(someObj, 0.0f);
                    }

                    if (objectGlobs.minifigureObj_9cb8 == NULL && someObj->type == LegoObject_MiniFigure)
                    {
                        objectGlobs.minifigureObj_9cb8 = someObj;
                    }

                    if (legoGlobs.recordObjsCount < 10)
                    {
                        legoGlobs.recordObjs[legoGlobs.recordObjsCount] = someObj;
                        legoGlobs.recordObjsCount++;
                    }

                    someObj->flags4 |= LIVEOBJ4_DONTSHOWHELPWINDOW;
                    someObj->health = objHealth;
                    HelpWindow_RecallDependencies(someObj->type, someObj->id, 0, TRUE);
                    if ((legoGlobs.flags2 & GAME2_RECALLOLOBJECTS) != GAME2_NONE)
                        ObjectRecall_RecallMiniFigure(someObj);

                    LegoObject_RegisterVehicle__callsForWater(someObj);

                    if (objType == LegoObject_SpiderWeb)
                    {
                        someObj->flags3 |= LIVEOBJ3_UNK_10000;
                        Container_SetActivity(someObj->other, "FlapInWind");
                        Container_SetAnimationTime(someObj->other, 0.0f);
                        SpiderWeb_Add(xInt, yInt, someObj);
                    }
                    else if (objType == LegoObject_ElectricFence)
                    {
                        ElectricFence_CreateFence(someObj);
                    }
                }

                Mem_Free(objTypeName);

                i++;
                objIter++;

                conf = Config_GetNextItem(conf);
            } while (conf != NULL);

            // fail if break out of loop
            success = conf == NULL;
        }

        Config_Free(prop);
    }

    if (!success)
    {
        return FALSE;
    }

    // TODO: Implement Lego_LoadOLObjectList

    return TRUE;
}

B32 Lego_LoadTextureSet(lpLego_Level level, const char* keyTexturePath)
{

    S32 surfTextureWidth = Config_GetIntValue(legoGlobs.config, Config_BuildStringID(legoGlobs.gameName, keyTexturePath, "surftextwidth", 0));
    if (surfTextureWidth == 0)
        return FALSE;
    S32 surfTextureHeight = Config_GetIntValue(legoGlobs.config, Config_BuildStringID(legoGlobs.gameName, keyTexturePath, "surftextheight", 0));
    if (surfTextureHeight == 0)
        return FALSE;

    level->textureSetSize.width = surfTextureWidth;
    level->textureSetSize.height = surfTextureHeight;

    Lego_InitTextureMappings(level->map);
    Map3D_UpdateAllBlockNormals(level->map);

    const char* meshbasename = Config_GetTempStringValue(legoGlobs.config, Config_BuildStringID(legoGlobs.gameName, keyTexturePath, "meshbasename", 0));
    if (meshbasename == NULL)
        return FALSE;

    const char* texturebasename = Config_GetTempStringValue(legoGlobs.config, Config_BuildStringID(legoGlobs.gameName, keyTexturePath, "texturebasename", 0));
    if (texturebasename == NULL)
        return FALSE;

    level->textureSet = Detail_LoadTextureSet(texturebasename, level->textureSetSize.width, level->textureSetSize.height);
    if (level->textureSet == NULL)
        return FALSE;

    Map3D_SetTextureSet(level->map, level->textureSet);

    if (Lego_LoadDetailMeshes(level, meshbasename) == 0)
        return FALSE;

    const char* rooftexture = Config_GetTempStringValue(legoGlobs.config, Config_BuildStringID(legoGlobs.gameName, keyTexturePath, "rooftexture", 0));
    if (rooftexture == NULL)
        return FALSE;

    if (Roof_SetTexture(rooftexture) == 0)
        return FALSE;

    return TRUE;
}

B32 Lego_LoadPreDugMap(lpLego_Level level, const char* filename, S32 modifier)
{
    if (filename == NULL)
        return FALSE;

    U32 handle = File_LoadBinaryHandle(filename, NULL);
    if (handle == -1)
        return FALSE;

    U32 width, height;
    MapShared_GetDimensions(handle, &width, &height);

    if (width == level->width && height == level->height)
    {
        if (height != 1 && height != 2)
        {
            U32 by = 1;
            U32 w = width;
            U32 h = height;
            do
            {
                U32 bx = 1;
                if (w != 1 && w != 2)
                {
                    do
                    {
                        U32 block = MapShared_GetBlock(handle, bx, by);
                        Lego_SurfaceType8 terrain = block - modifier;
                        if (terrain == Lego_SurfaceType8_Immovable || terrain == Lego_SurfaceType8_Medium)
                        {
                            Level_DestroyWall(level, bx, by, TRUE);

                            // TODO: Implement Lego_LoadPreDugMap
                        }
                        else if (terrain == Lego_SurfaceType8_Hard || terrain == Lego_SurfaceType8_Loose)
                        {
                            level->blocks[(S32)(bx + by * level->width)].flags1 |= BLOCK1_HIDDEN;
                        }

                        if (terrain == Lego_SurfaceType8_Medium)
                        {
                            level->blocks[(S32)(bx + by * level->width)].flags2 |= BLOCK2_SLUGHOLE_EXPOSED;

                            Point2I blockPos;
                            blockPos.x = bx;
                            blockPos.y = by;
                            LegoObject_RegisterSlimySlugHole(&blockPos);
                        }

                        if (terrain == Lego_SurfaceType8_Loose)
                        {
                            level->blocks[(S32)(bx + by * level->width)].flags2 |= BLOCK2_SLUGHOLE_HIDDEN;
                        }

                        bx++;
                    } while (bx < w - 1);
                }

                by++;
            } while (by < h - 1);
        }
    }

    Mem_FreeHandle(handle);

    return TRUE;
}

B32 Lego_LoadTerrainMap(lpLego_Level level, const char* filename, S32 modifier)
{
    U32 memHandle = File_LoadBinaryHandle(filename, NULL);
    if (memHandle == -1)
        return FALSE;

    U32 width, height;
    MapShared_GetDimensions(memHandle, &width, &height);

    if (width != level->width || height != level->height)
    {
        Mem_FreeHandle(memHandle);
        return FALSE;
    }

    for (U32 y = 0; y < height; y++)
    {
        for (U32 x = 0; x < width; x++)
        {
            U32 block = MapShared_GetBlock(memHandle, x, y);
            Lego_SurfaceType8 terrain = block - modifier;

            // Soil SurfaceType was removed, change to Dirt
            if (terrain == Lego_SurfaceType8_Soil)
                terrain = Lego_SurfaceType8_Loose;

            level->blocks[x + y * level->width].terrain = terrain;

            if (terrain == Lego_SurfaceType8_Lava)
            {
                level->blocks[x + y * level->width].erodeLevel = 4;
            }
            else if (terrain == Lego_SurfaceType8_RechargeSeam)
            {
                // TODO: Implement Lego_LoadTerrainMap
            }
        }
    }

    Mem_FreeHandle(memHandle);

    return TRUE;
}

B32 Lego_LoadCryOreMap(lpLego_Level level, const char* filename, S32 modifier)
{
    // TODO: Implement Lego_LoadCryOreMap
    return TRUE;
}

B32 Lego_LoadErodeMap(lpLego_Level level, const char* filename)
{
    // TODO: Implement Lego_LoadErodeMap
    return TRUE;
}

B32 Lego_LoadPathMap(lpLego_Level level, const char* filename, S32 modifier)
{
    U32 fileHandle;
    if (filename != NULL && (fileHandle = File_LoadBinaryHandle(filename, NULL), fileHandle != -1))
    {
        U32 width, height;
        MapShared_GetDimensions(fileHandle, &width, &height);
        if (width != level->width || height != level->height)
        {
            Mem_FreeHandle(fileHandle);
            return FALSE;
        }

        for (U32 y = 0; y < height; y++)
        {
            for (U32 x = 0; x < width; x++)
            {
                Point2I coords;
                coords.x = x;
                coords.y = y;
                U32 block = MapShared_GetBlock(fileHandle, x, y);

                if ((level->blocks[x + y * level->width].flags1 & BLOCK1_FLOOR) != BLOCK1_NONE)
                {
                    if (block - modifier == 2)
                    {
                        level->blocks[x + y * level->width].flags1 |= BLOCK1_PATH;
                    }
                    else if (block - modifier == 1 &&
                        (legoGlobs.currLevel->blocks[x + legoGlobs.currLevel->width * y].flags1 & BLOCK1_BUILDINGSOLID) == BLOCK1_NONE &&
                        ((legoGlobs.currLevel->blocks[x + legoGlobs.currLevel->width * y].flags2 & 0xFF) & BLOCK2_TOOLSTORE) == BLOCK2_NONE &&
                        (legoGlobs.currLevel->blocks[x + legoGlobs.currLevel->width * y].flags1 & BLOCK1_BUILDINGPATH) == BLOCK1_NONE &&
                        (!Construction_Zone_ExistsAtBlock(&coords)))
                    {
                        level->blocks[x + y * level->width].flags1 &= ~BLOCK1_CLEARED;
                        level->blocks[x + y * level->width].flags1 &= ~BLOCK1_PATH;
                        level->blocks[x + y * level->width].flags1 &= ~BLOCK1_LAYEDPATH;
                        LegoObject_RequestPowerGridUpdate();
                        AITask_RemoveAttackPathReferences(&coords);
                        //unknownemptyfunc(&coords); // TODO: Figure out what this was

                        for (U32 i = 0; i < 4; i++)
                        {
                            AITask_DoClear_AtPosition(&coords, Message_ClearInitialComplete);
                        }

                        legoGlobs.currLevel->blocks[x + legoGlobs.currLevel->width * y].flags1 |= BLOCK1_RUBBLE_FULL;
                    }
                }
            }
        }

        Mem_FreeHandle(fileHandle);
        return TRUE;
    }
    return FALSE;
}

B32 Lego_LoadEmergeMap(lpLego_Level level, const char* filename)
{
    // TODO: Implement Lego_LoadEmergeMap
    return TRUE;
}

B32 Lego_LoadAIMap(lpLego_Level level, const char* filename)
{
    // TODO: Implement Lego_LoadAIMap
    return TRUE;
}

B32 Lego_LoadFallinMap(lpLego_Level level, const char* filename)
{
    // TODO: Implement Lego_LoadFallinMap
    return TRUE;
}

B32 Lego_LoadBlockPointersMap(lpLego_Level level, const char* filename, S32 modifier)
{
    // TODO: Implement Lego_LoadBlockPointersMap
    return FALSE;
}

B32 Lego_EndLevel()
{
    // TODO: Implement Lego_EndLevel
    return TRUE;
}

lpLego_Level Lego_GetLevel()
{
    return legoGlobs.currLevel;
}

lpMap3D Lego_GetMap()
{
    return Lego_GetLevel()->map;
}

void Lego_SetMusicOn(B32 isMusicOn)
{
    if (isMusicOn)
    {
        S16 rng = Maths_Rand();
        legoGlobs.flags1 |= GAME1_USEMUSIC;
        Sound_PlayCDTrack(((U32)(S32)rng % legoGlobs.CDTracks - 1) + legoGlobs.CDStartTrack, SoundMode_Once, Lego_CDTrackPlayNextCallback);
        return;
    }

    legoGlobs.flags1 &= ~GAME1_USEMUSIC;
    Sound_StopCD();
    Sound_Update(FALSE);
}

void Lego_CDTrackPlayNextCallback()
{
    Sound_StopCD();
    Lego_SetMusicOn(TRUE);
}

void Lego_SetSoundOn(B32 isSoundOn)
{
    if (isSoundOn)
    {
        legoGlobs.flags1 |= GAME1_USESFX;
        SFX_SetSoundOn_AndStopAll(TRUE);
        return;
    }

    legoGlobs.flags1 &= ~GAME1_USESFX;
    SFX_SetSoundOn_AndStopAll(FALSE);
}

void Lego_SetGameSpeed(F32 newGameSpeed)
{
    if (legoGlobs.gameSpeed <= newGameSpeed || !gamectrlGlobs.isGameSpeedLocked)
    {
        Front_UpdateSliderGameSpeed();
        legoGlobs.gameSpeed = newGameSpeed;
        if (newGameSpeed < 0.0f)
            legoGlobs.gameSpeed = 0.0f;

        // Debug mode allows game speeds up to 300%
        if (legoGlobs.gameSpeed > 3.0f)
            legoGlobs.gameSpeed = 3.0f;
    }
}

F32 Lego_GetGameSpeed()
{
    return legoGlobs.gameSpeed;
}

void Lego_SetViewMode(ViewMode viewMode, lpLegoObject liveObj, U32 cameraFrame)
{
    if (viewMode == ViewMode_FP)
    {
        // TODO: Implement Lego_SetViewMode
    }
    else if (viewMode == ViewMode_Top)
    {
        if (legoGlobs.objectFP != NULL)
            Lego_Goto(legoGlobs.objectFP, NULL, FALSE);

        legoGlobs.cameraFP->trackObject = NULL;
        legoGlobs.objectFP = NULL;

        Water_Debug_LogContainerMesh(FALSE);

        Map3D_SetEmissive(legoGlobs.currLevel->map, FALSE);
        Viewport_SetCamera(legoGlobs.viewMain, legoGlobs.cameraMain->contCam);
        Sound3D_MakeListener(legoGlobs.cameraMain->contListener->masterFrame);
        Sound3D_SetMinDistForAtten(legoGlobs.MinDistFor3DSoundsOnTopView);
        Viewport_SetField(legoGlobs.viewMain, 0.5f);
        Viewport_SetBackClip(legoGlobs.viewMain, legoGlobs.TVClipDist);
    }

    legoGlobs.viewMode = viewMode;
}

void Lego_Goto(lpLegoObject liveObj, Point2I* blockPos, B32 smooth)
{
    // TODO: Implement Lego_Goto
}

B32 Lego_GetObjectByName(const char* objName, LegoObject_Type* outObjType, LegoObject_ID* outObjID, lpContainer* outModel)
{
    *outObjID = 0;

    for (U32 i = 0; i < legoGlobs.rockMonsterCount; i++)
    {
        if (_stricmp(legoGlobs.rockMonsterName[i], objName) == 0)
        {
            *outObjType = LegoObject_RockMonster;
            *outObjID = i;
            if (outModel == NULL)
                return TRUE;

            *outModel = (lpContainer)&legoGlobs.rockMonsterData[i];
            return TRUE;
        }
    }

    if (_stricmp("tvcamera", objName) == 0)
    {
        *outObjType = LegoObject_TVCamera;
        if (outModel != NULL)
            *outModel = NULL;
        return TRUE;
    }

    if (_stricmp("PowerCrystal", objName) == 0)
    {
        *outObjType = LegoObject_PowerCrystal;
        if (outModel != NULL)
            *outModel = legoGlobs.contCrystal;
        return TRUE;
    }

    if (_stricmp("Ore", objName) == 0)
    {
        *outObjType = LegoObject_Ore;
        *outObjID = 0;
        if (outModel != NULL)
        {
            *outModel = legoGlobs.contOresTable[0];
            return TRUE;
        }
    }

    if (_stricmp("ProcessedOre", objName) == 0)
    {
        *outObjType = LegoObject_Ore;
        *outObjID = 1;
        if (outModel != NULL)
        {
            *outModel = legoGlobs.contOresTable[1];
            return TRUE;
        }
    }

    if (_stricmp("Boulder", objName) == 0)
    {
        *outObjType = LegoObject_Boulder;
        if (outModel != NULL)
        {
            *outModel = legoGlobs.contBoulder;
            return TRUE;
        }
    }

    if (_stricmp("Pusher", objName) == 0)
    {
        *outObjType = LegoObject_Pusher;
        if (outModel != NULL)
        {
            *outModel = legoGlobs.contPusher;
            return TRUE;
        }
    }

    if (_stricmp("LaserShot", objName) == 0)
    {
        *outObjType = LegoObject_LaserShot;
        if (outModel != NULL)
        {
            *outModel = legoGlobs.contLaserShot;
            return TRUE;
        }
    }

    if (_stricmp("Freezer", objName) == 0)
    {
        *outObjType = LegoObject_Freezer;
        if (outModel != NULL)
        {
            *outModel = legoGlobs.contFreezer;
            return TRUE;
        }
    }

    if (_stricmp("Dynamite", objName) == 0)
    {
        *outObjType = LegoObject_Dynamite;
        if (outModel != NULL)
        {
            *outModel = legoGlobs.contDynamite;
            return TRUE;
        }
    }

    if (_stricmp("ElectricFence", objName) == 0)
    {
        *outObjType = LegoObject_ElectricFence;
        if (outModel != NULL)
        {
            *outModel = legoGlobs.contElectricFence;
            return TRUE;
        }
    }

    if (_stricmp("SpiderWeb", objName) == 0)
    {
        *outObjType = LegoObject_SpiderWeb;
        if (outModel != NULL)
        {
            *outModel = legoGlobs.contSpiderWeb;
            return TRUE;
        }
    }

    if (_stricmp("Barrier", objName) == 0)
    {
        *outObjType = LegoObject_Barrier;
        if (outModel != NULL)
        {
            *outModel = legoGlobs.contBarrier;
            return TRUE;
        }
    }

    if (_stricmp("OohScary", objName) == 0)
    {
        *outObjType = LegoObject_OohScary;
        if (outModel != NULL)
        {
            *outModel = legoGlobs.contOohScary;
            return TRUE;
        }
    }

    if (_stricmp("Path", objName) == 0)
    {
        *outObjType = LegoObject_Path;
        if (outModel != NULL)
        {
            *outModel = NULL;
            return TRUE;
        }
    }

    for (U32 i = 0; i < legoGlobs.vehicleCount; i++)
    {
        if (_stricmp(legoGlobs.vehicleName[i], objName) == 0)
        {
            *outObjType = LegoObject_Vehicle;
            *outObjID = i;
            if (outModel == NULL)
                return TRUE;

            *outModel = (lpContainer)&legoGlobs.vehicleData[i];
            return TRUE;
        }
    }

    for (U32 i = 0; i < legoGlobs.miniFigureCount; i++)
    {
        if (_stricmp(legoGlobs.miniFigureName[i], objName) == 0)
        {
            *outObjType = LegoObject_MiniFigure;
            *outObjID = i;
            if (outModel == NULL)
                return TRUE;

            *outModel = (lpContainer)&legoGlobs.miniFigureData[i];
            return TRUE;
        }
    }

    U32 index = 0;
    if (legoGlobs.buildingCount == 0)
        return FALSE;

    while (_stricmp(legoGlobs.buildingName[index], objName) != 0)
    {
        index++;
        if (index >= legoGlobs.buildingCount)
            return FALSE;
    }

    *outObjType = LegoObject_Building;
    *outObjID = index;
    if (outModel != NULL)
        *outModel = (lpContainer)&legoGlobs.buildingData[index];

    return TRUE;
}

void Lego_LoadSamples(lpConfig config, B32 noReduceSamples)
{
    SFX_ID sfxType;

    lpConfig prop = Config_FindArray(config, Config_BuildStringID(legoGlobs.gameName, "Samples", 0));
    if (!prop)
        return;

    SFX_SetSamplePopulateMode(TRUE);
    do
    {
        const char* name = prop->itemName;
        B32 reduced = FALSE;
        if ((*name == '!') && (name++, noReduceSamples == 0))
            reduced = TRUE;
        if (!reduced)
        {
            // SFX_SetSamplePopulateMode(TRUE); was called, meaning this function will register new SFX if they don't already exist
            if (SFX_GetType(name, &sfxType))
                SFX_LoadSampleProperty(prop->dataString, sfxType);
        }
        prop = Config_GetNextItem(prop);
    } while (prop != NULL);
    SFX_SetSamplePopulateMode(FALSE);
}

void Lego_LoadSurfaceTypeDescriptions_sound(lpConfig config, const char* gameName)
{
    // TODO: Implement Lego_LoadSurfaceTypeDescriptions_sound
}

void Lego_LoadToolTipInfos(lpConfig config, const char* gameName)
{
    // TODO: Implement Lego_LoadToolTipInfos
}

B32 Lego_LoadLighting()
{
    F32 r, g, b;

    // === SpotlightTop ===
    if (!Config_GetRGBValue(legoGlobs.config, Config_BuildStringID(legoGlobs.gameName, "Main", "topspotrgb", 0), &r, &g, &b))
    {
        return FALSE;
    }

    legoGlobs.spotlightTop = Container_MakeLight(legoGlobs.cameraMain->cont3, D3DRMLIGHT_SPOT, r, g, b);
    if (legoGlobs.spotlightTop == NULL)
    {
        return FALSE;
    }

    Container_Light_SetSpotPenumbra(legoGlobs.spotlightTop, 0.85f);
    Container_Light_SetSpotUmbra(legoGlobs.spotlightTop, 0.4f);
    Container_SetPosition(legoGlobs.spotlightTop, legoGlobs.cameraMain->cont3, 200.0f, 140.0f, -130.0f);
    Container_SetOrientation(legoGlobs.spotlightTop, legoGlobs.cameraMain->cont3, -1.0f, -0.8f, 0.75f, 0.0f, 1.0f, 0.0f);

    // === SpotlightTrack ===

    if (!Config_GetRGBValue(legoGlobs.config, Config_BuildStringID(legoGlobs.gameName, "Main", "trackspotrgb", 0), &r, &g, &b))
    {
        return FALSE;
    }

    legoGlobs.spotlightTrack = Container_MakeLight(legoGlobs.cameraTrack->cont2, D3DRMLIGHT_SPOT, r, g, b);
    if (legoGlobs.spotlightTrack == NULL)
    {
        return FALSE;
    }

    Container_SetPosition(legoGlobs.spotlightTrack, NULL, 0.0f, 0.0f, -150.0f);
    Container_SetOrientation(legoGlobs.spotlightTrack, NULL, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f);

    // === AmbientLight ===

    if (!Config_GetRGBValue(legoGlobs.config, Config_BuildStringID(legoGlobs.gameName, "Main", "ambientrgb", 0), &r, &g, &b))
    {
        return FALSE;
    }

    legoGlobs.ambientLight = Container_MakeLight(legoGlobs.rootCont, D3DRMLIGHT_AMBIENT, r, g, b);
    if (legoGlobs.ambientLight == NULL)
    {
        return FALSE;
    }

    // === PointLightFP ===

    if (!Config_GetRGBValue(legoGlobs.config, Config_BuildStringID(legoGlobs.gameName, "Main", "fplightrgb", 0), &r, &g, &b))
    {
        return FALSE;
    }

    legoGlobs.pointLightFP = Container_MakeLight(legoGlobs.cameraFP->contCam, D3DRMLIGHT_POINT, r, g, b);
    if (legoGlobs.pointLightFP == NULL)
    {
        return FALSE;
    }

    // === DirLightFP ===

    if (Config_GetRGBValue(legoGlobs.config, Config_BuildStringID(legoGlobs.gameName, "Main", "fprotlightrgb", 0), &r, &g, &b))
        legoGlobs.dirLightFP = Container_MakeLight(legoGlobs.cameraFP->contCam, D3DRMLIGHT_DIRECTIONAL, r, g, b);
    else
        legoGlobs.dirLightFP = NULL;

    // === DirLightCallToArms ===

    legoGlobs.dirLightCallToArms = Container_MakeLight(legoGlobs.cameraFP->contCam, D3DRMLIGHT_DIRECTIONAL, 1.0f, 0.0f, 0.0f);
    if (legoGlobs.dirLightCallToArms == NULL)
    {
        return FALSE;
    }

    // === RootLight/RootSpotlight ===

    legoGlobs.rootLight = Container_Create(Container_GetRoot());
    legoGlobs.rootSpotlight = Container_MakeLight(legoGlobs.rootLight, D3DRMLIGHT_SPOT, 1.0f, 1.0f, 1.0f);
    if (legoGlobs.rootSpotlight == NULL)
    {
        return FALSE;
    }

    // === Properties ===

    LightEffects_Initialize(legoGlobs.rootSpotlight, legoGlobs.rootLight, 0.8f, 0.8f, 0.8f);

    Container_SetOrientation(legoGlobs.rootSpotlight, NULL, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f);
    Container_Light_SetSpotPenumbra(legoGlobs.rootSpotlight, 0.85f);
    Container_Light_SetSpotUmbra(legoGlobs.rootSpotlight, 0.0f);

    Container_Hide(legoGlobs.pointLightFP, TRUE);
    if (legoGlobs.dirLightFP != NULL)
        Container_Hide(legoGlobs.dirLightFP, TRUE);

    Container_Hide(legoGlobs.spotlightTop, TRUE);
    Container_Hide(legoGlobs.spotlightTrack, TRUE);
    Container_Hide(legoGlobs.ambientLight, TRUE);
    Container_Hide(legoGlobs.rootSpotlight, TRUE);
    Container_Hide(legoGlobs.dirLightCallToArms, TRUE);

    Container_Light_SetSpotRange(legoGlobs.spotlightTop, 2000.0f);
    Container_Light_SetSpotRange(legoGlobs.spotlightTrack, 2000.0f);
    Container_Light_SetSpotRange(legoGlobs.rootSpotlight, 2000.0f);

    return TRUE;
}

B32 Lego_LoadGraphicsSettings()
{
    legoGlobs.FPClipBlocks = Config_GetFloatValue(legoGlobs.config, Config_BuildStringID(legoGlobs.gameName, "Main", "fpclipblocks", 0));
    if (legoGlobs.FPClipBlocks != 0.0f)
    {
        const char* qualityStr = Config_GetTempStringValue(legoGlobs.config, Config_BuildStringID(legoGlobs.gameName, "Main", "quality", 0));
        if (qualityStr == NULL)
            return FALSE;

        if (stricmp("gouraud", qualityStr) == 0)
            legoGlobs.quality = Gouraud;
        else if (stricmp("flat", qualityStr) == 0)
            legoGlobs.quality = Flat;
        else if (stricmp("wireframe", qualityStr) == 0)
            legoGlobs.quality = Wireframe;
        else
            return FALSE;

        Bool3 dither = Config_GetBoolValue(legoGlobs.config, Config_BuildStringID(legoGlobs.gameName, "Main", "dither", 0));
        Bool3 filter = Config_GetBoolValue(legoGlobs.config, Config_BuildStringID(legoGlobs.gameName, "Main", "filter", 0));
        Bool3 blend = Config_GetBoolValue(legoGlobs.config, Config_BuildStringID(legoGlobs.gameName, "Main", "blend", 0));
        Bool3 sort = Config_GetBoolValue(legoGlobs.config, Config_BuildStringID(legoGlobs.gameName, "Main", "sort", 0));
        Bool3 mipmap = Config_GetBoolValue(legoGlobs.config, Config_BuildStringID(legoGlobs.gameName, "Main", "mipmap", 0));
        Bool3 linearMipmap = Config_GetBoolValue(legoGlobs.config, Config_BuildStringID(legoGlobs.gameName, "Main", "linearmipmap", 0));

        Main_Setup3D(legoGlobs.quality, dither == BOOL3_TRUE, filter == BOOL3_TRUE, mipmap == BOOL3_TRUE, linearMipmap == BOOL3_TRUE, blend == BOOL3_TRUE, sort == BOOL3_TRUE);

        return TRUE;
    }
    return FALSE;
}

B32 Lego_LoadUpgradeTypes()
{
    // TODO: Implement Lego_LoadUpgradeTypes
    return TRUE;
}

B32 Lego_LoadVehicleTypes()
{
    legoGlobs.vehicleCount = 0;
    for (lpConfig c = Config_FindArray(legoGlobs.config, Config_BuildStringID(legoGlobs.gameName, "VehicleTypes", 0)); c != NULL; c = Config_GetNextItem(c))
    {
        legoGlobs.vehicleCount++;
    }

    if (legoGlobs.vehicleCount != 0 && (legoGlobs.vehicleData = Mem_Alloc(legoGlobs.vehicleCount * sizeof(VehicleModel)), legoGlobs.vehicleData != NULL))
    {
        legoGlobs.vehicleName = Mem_Alloc(legoGlobs.vehicleCount * 4);
        if (legoGlobs.vehicleName != NULL)
        {
            lpConfig conf = Config_FindArray(legoGlobs.config, Config_BuildStringID(legoGlobs.gameName, "VehicleTypes", 0));
            B32 unkBool;
            U32 i = 0;
            while (conf != NULL && (unkBool = Vehicle_Load(&legoGlobs.vehicleData[i], i, legoGlobs.rootCont, conf->dataString, legoGlobs.gameName), unkBool != FALSE))
            {
                Vehicle_Hide(&legoGlobs.vehicleData[i], TRUE);

                legoGlobs.vehicleName[i] = Mem_Alloc(strlen(conf->itemName) + 1);
                strcpy(legoGlobs.vehicleName[i], conf->itemName);

                conf = Config_GetNextItem(conf);

                i++;
            }

            if (conf == NULL)
            {
                return TRUE;
            }

            Mem_Free(legoGlobs.vehicleName);
        }
        Mem_Free(legoGlobs.vehicleData);
    }

    return FALSE;
}

B32 Lego_LoadMiniFigureTypes()
{
    legoGlobs.miniFigureCount = 0;
    for (lpConfig c = Config_FindArray(legoGlobs.config, Config_BuildStringID(legoGlobs.gameName, "MiniFigureTypes", 0)); c != NULL; c = Config_GetNextItem(c))
    {
        legoGlobs.miniFigureCount++;
    }

    if (legoGlobs.miniFigureCount != 0 && (legoGlobs.miniFigureData = Mem_Alloc(legoGlobs.miniFigureCount * sizeof(CreatureModel)), legoGlobs.miniFigureData != NULL))
    {
        legoGlobs.miniFigureName = Mem_Alloc(legoGlobs.miniFigureCount * 4);
        if (legoGlobs.miniFigureName != NULL)
        {
            lpConfig conf = Config_FindArray(legoGlobs.config, Config_BuildStringID(legoGlobs.gameName, "MiniFigureTypes", 0));
            B32 unkBool;
            U32 i = 0;
            while (conf != NULL && (unkBool = Creature_Load(&legoGlobs.miniFigureData[i], i, legoGlobs.rootCont, conf->dataString, legoGlobs.gameName), unkBool != FALSE))
            {
                Object_Hide(&legoGlobs.miniFigureData[i], TRUE);

                legoGlobs.miniFigureName[i] = Mem_Alloc(strlen(conf->itemName) + 1);
                strcpy(legoGlobs.miniFigureName[i], conf->itemName);

                conf = Config_GetNextItem(conf);

                i++;
            }

            if (conf == NULL)
            {
                return TRUE;
            }

            Mem_Free(legoGlobs.miniFigureName);
        }
        Mem_Free(legoGlobs.miniFigureData);
    }

    return FALSE;
}

B32 Lego_LoadRockMonsterTypes()
{
    legoGlobs.rockMonsterCount = 0;
    ;
    for (lpConfig c = Config_FindArray(legoGlobs.config, Config_BuildStringID(legoGlobs.gameName, "RockMonsterTypes", 0)); c != NULL; c = Config_GetNextItem(c))
    {
        legoGlobs.rockMonsterCount++;
    }

    if (legoGlobs.rockMonsterCount != 0 && (legoGlobs.rockMonsterData = Mem_Alloc(legoGlobs.rockMonsterCount * sizeof(CreatureModel)), legoGlobs.rockMonsterData != NULL))
    {
        legoGlobs.rockMonsterName = Mem_Alloc(legoGlobs.rockMonsterCount * 4);
        if (legoGlobs.rockMonsterName != NULL)
        {
            lpConfig conf = Config_FindArray(legoGlobs.config, Config_BuildStringID(legoGlobs.gameName, "RockMonsterTypes", 0));
            B32 unkBool;
            U32 i = 0;
            while (conf != NULL && (unkBool = Creature_Load(&legoGlobs.rockMonsterData[i], i, legoGlobs.rootCont, conf->dataString, legoGlobs.gameName), unkBool != FALSE))
            {
                Object_Hide(&legoGlobs.rockMonsterData[i], TRUE);

                legoGlobs.rockMonsterName[i] = Mem_Alloc(strlen(conf->itemName) + 1);
                strcpy(legoGlobs.rockMonsterName[i], conf->itemName);

                conf = Config_GetNextItem(conf);

                i++;
            }

            if (conf == NULL)
            {
                return TRUE;
            }

            Mem_Free(legoGlobs.rockMonsterName);
        }
        Mem_Free(legoGlobs.rockMonsterData);
    }

    return FALSE;
}

B32 Lego_LoadBuildingTypes()
{
    legoGlobs.buildingCount = 0;
;
    for (lpConfig c = Config_FindArray(legoGlobs.config, Config_BuildStringID(legoGlobs.gameName, "BuildingTypes", 0)); c != NULL; c = Config_GetNextItem(c))
    {
        legoGlobs.buildingCount++;
    }

    if (legoGlobs.buildingCount != 0 && (legoGlobs.buildingData = Mem_Alloc(legoGlobs.buildingCount * sizeof(BuildingModel)), legoGlobs.buildingData != NULL))
    {
        legoGlobs.buildingName = Mem_Alloc(legoGlobs.buildingCount * 4);
        if (legoGlobs.buildingName != NULL)
        {
            lpConfig conf = Config_FindArray(legoGlobs.config, Config_BuildStringID(legoGlobs.gameName, "BuildingTypes", 0));
            B32 unkBool;
            U32 i = 0;
            while (conf != NULL && (unkBool = Building_Load(&legoGlobs.buildingData[i], i, legoGlobs.rootCont, conf->dataString, legoGlobs.gameName), unkBool != FALSE))
            {
                Building_Hide(&legoGlobs.buildingData[i], TRUE);

                legoGlobs.buildingName[i] = Mem_Alloc(strlen(conf->itemName) + 1);
                strcpy(legoGlobs.buildingName[i], conf->itemName);

                conf = Config_GetNextItem(conf);

                i++;
            }

            if (conf == NULL)
            {
                return TRUE;
            }

            Mem_Free(legoGlobs.buildingName);
        }
        Mem_Free(legoGlobs.buildingData);
    }

    return FALSE;
}

void Lego_InitTextureMappings(lpMap3D map)
{
    SurfaceTexture reinWalls[5];
    SurfaceTexture inCorners[5];
    SurfaceTexture reinInCorners[5];
    SurfaceTexture outCorners[5];
    SurfaceTexture reinOutCorners[5];
    SurfaceTexture diagonalWalls[5];
    SurfaceTexture roofTypes[6];
    SurfaceTexture forwardWalls[16];
    SurfaceTexture floorTypes[31];

    roofTypes[0] = TEXTURE_ROOF_STD;
    roofTypes[1] = TEXTURE_ROOF_STD;
    roofTypes[2] = TEXTURE_ROOF_STD;
    roofTypes[3] = TEXTURE_ROOF_STD;
    roofTypes[4] = TEXTURE_ROOF_STD;
    roofTypes[5] = TEXTURE_ROOF_STD;

    forwardWalls[0] = TEXTURE_WALL_IMM;
    forwardWalls[1] = TEXTURE_WALL_HARD;
    forwardWalls[2] = TEXTURE_WALL_MED;
    forwardWalls[3] = TEXTURE_WALL_LOOSE;
    forwardWalls[4] = TEXTURE_WALL_SOIL;
    forwardWalls[5] = TEXTURE_WALL_ORESEAM;
    forwardWalls[6] = TEXTURE_WALL_CRYSTALSEAM;
    forwardWalls[7] = TEXTURE_WALL_RECHARGESEAM;
    forwardWalls[8] = TEXTURE_WALL_IMM;
    forwardWalls[9] = TEXTURE_WALL_HARD;
    forwardWalls[10] = TEXTURE_WALL_MED;
    forwardWalls[11] = TEXTURE_WALL_LOOSE;
    forwardWalls[12] = TEXTURE_WALL_SOIL;
    forwardWalls[13] = TEXTURE_WALL_ORESEAM;
    forwardWalls[14] = TEXTURE_WALL_CRYSTALSEAM;
    forwardWalls[15] = TEXTURE_WALL_RECHARGESEAM;

    floorTypes[0] = TEXTURE_FLOOR_STD;
    floorTypes[1] = TEXTURE_FLOOR_STD;
    floorTypes[2] = TEXTURE_FLOOR_STD;
    floorTypes[3] = TEXTURE_FLOOR_STD;
    floorTypes[4] = TEXTURE_FLOOR_LAVA;
    floorTypes[5] = TEXTURE_FLOOR_LAVA_NOTHOT;
    floorTypes[6] = TEXTURE_FLOOR_WATER;
    floorTypes[7] = TEXTURE_FLOOR_STD;
    floorTypes[8] = TEXTURE_FLOOR_PATH_FOUNDATION;
    floorTypes[9] = TEXTURE_FLOOR_POWERED_FOUNDATION;
    floorTypes[10] = TEXTURE_FLOOR_RUBBLE_MAX;
    floorTypes[11] = TEXTURE_FLOOR_RUBBLE_HIGH;
    floorTypes[12] = TEXTURE_FLOOR_RUBBLE_MED;
    floorTypes[13] = TEXTURE_FLOOR_RUBBLE_LOW;
    floorTypes[14] = TEXTURE_FLOOR_ERODE_LOW;
    floorTypes[15] = TEXTURE_FLOOR_ERODE_MED;
    floorTypes[16] = TEXTURE_FLOOR_ERODE_HIGH;
    floorTypes[17] = TEXTURE_FLOOR_ERODE_MAX;
    floorTypes[18] = TEXTURE_FLOOR_07;
    floorTypes[19] = TEXTURE_FLOOR_PATH_LAYED;
    floorTypes[20] = TEXTURE_FLOOR_PATH_4SIDES;
    floorTypes[21] = TEXTURE_FLOOR_PATH_2SIDES;
    floorTypes[22] = TEXTURE_FLOOR_PATH_CORNER;
    floorTypes[23] = TEXTURE_FLOOR_PATH_3SIDES;
    floorTypes[24] = TEXTURE_FLOOR_PATH_1SIDES;
    floorTypes[25] = TEXTURE_FLOOR_POWERED_4SIDES;
    floorTypes[26] = TEXTURE_FLOOR_POWERED_2SIDES;
    floorTypes[27] = TEXTURE_FLOOR_POWERED_CORNER;
    floorTypes[28] = TEXTURE_FLOOR_POWERED_3SIDES;
    floorTypes[29] = TEXTURE_FLOOR_POWERED_1SIDES;
    floorTypes[30] = TEXTURE_FLOOR_SLUGHOLE;

    reinWalls[0] = TEXTURE_REINWALL_IMM;
    reinWalls[1] = TEXTURE_REINWALL_HARD;
    reinWalls[2] = TEXTURE_REINWALL_MED;
    reinWalls[3] = TEXTURE_REINWALL_LOOSE;
    reinWalls[4] = TEXTURE_REINWALL_SOIL;

    inCorners[0] = TEXTURE_INCORNER_IMM;
    inCorners[1] = TEXTURE_INCORNER_HARD;
    inCorners[2] = TEXTURE_INCORNER_MED;
    inCorners[3] = TEXTURE_INCORNER_LOOSE;
    inCorners[4] = TEXTURE_INCORNER_SOIL;

    reinInCorners[0] = TEXTURE_INCORNER_IMM;
    reinInCorners[1] = TEXTURE_INCORNER_HARD;
    reinInCorners[2] = TEXTURE_INCORNER_MED;
    reinInCorners[3] = TEXTURE_INCORNER_LOOSE;
    reinInCorners[4] = TEXTURE_INCORNER_SOIL;

    outCorners[0] = TEXTURE_OUTCORNER_IMM;
    outCorners[1] = TEXTURE_OUTCORNER_HARD;
    outCorners[2] = TEXTURE_OUTCORNER_MED;
    outCorners[3] = TEXTURE_OUTCORNER_LOOSE;
    outCorners[4] = TEXTURE_OUTCORNER_SOIL;

    reinOutCorners[0] = TEXTURE_OUTCORNER_IMM;
    reinOutCorners[1] = TEXTURE_OUTCORNER_HARD;
    reinOutCorners[2] = TEXTURE_OUTCORNER_MED;
    reinOutCorners[3] = TEXTURE_OUTCORNER_LOOSE;
    reinOutCorners[4] = TEXTURE_OUTCORNER_SOIL;

    diagonalWalls[0] = TEXTURE_DIAGONAL_STD;
    diagonalWalls[1] = TEXTURE_DIAGONAL_STD;
    diagonalWalls[2] = TEXTURE_DIAGONAL_STD;
    diagonalWalls[3] = TEXTURE_DIAGONAL_STD;
    diagonalWalls[4] = TEXTURE_DIAGONAL_STD;

    for (U32 i = 0; i < 31; i++)
    {
        SurfaceTexture floorTex = floorTypes[i];
        for (U32 j = 0; j < 16; j++)
            Map3D_AddTextureMapping(map, floorTex, forwardWalls[j]);
        for (U32 j = 0; j < 5; j++)
            Map3D_AddTextureMapping(map, floorTex, reinWalls[j]);
        for (U32 j = 0; j < 5; j++)
            Map3D_AddTextureMapping(map, floorTex, inCorners[j]);
        for (U32 j = 0; j < 5; j++)
            Map3D_AddTextureMapping(map, floorTex, reinInCorners[j]);
        for (U32 j = 0; j < 5; j++)
            Map3D_AddTextureMapping(map, floorTex, outCorners[j]);
        for (U32 j = 0; j < 5; j++)
            Map3D_AddTextureMapping(map, floorTex, reinOutCorners[j]);
        for (U32 j = 0; j < 5; j++)
            Map3D_AddTextureMapping(map, floorTex, diagonalWalls[j]);
    }

    for (U32 i = 0; i < 6; i++)
    {
        SurfaceTexture roofTex = roofTypes[i];
        for (U32 j = 0; j < 16; j++)
            Map3D_AddTextureMapping(map, roofTex, forwardWalls[j]);
        for (U32 j = 0; j < 5; j++)
            Map3D_AddTextureMapping(map, roofTex, reinWalls[j]);
        for (U32 j = 0; j < 5; j++)
            Map3D_AddTextureMapping(map, roofTex, inCorners[j]);
        for (U32 j = 0; j < 5; j++)
            Map3D_AddTextureMapping(map, roofTex, reinInCorners[j]);
        for (U32 j = 0; j < 5; j++)
            Map3D_AddTextureMapping(map, roofTex, outCorners[j]);
        for (U32 j = 0; j < 5; j++)
            Map3D_AddTextureMapping(map, roofTex, reinOutCorners[j]);
        for (U32 j = 0; j < 5; j++)
            Map3D_AddTextureMapping(map, roofTex, diagonalWalls[j]);
    }
}

B32 Lego_LoadDetailMeshes(lpLego_Level level, const char* meshBaseName)
{
    // TODO: Implement Lego_LoadDetailMeshes
    return TRUE;
}

void Lego_LoadObjectNames(lpConfig config)
{
    // TODO: Implement Lego_LoadObjectNames
}

void Lego_LoadObjectTheNames(lpConfig config)
{
    // TODO: Implement Lego_LoadObjectTheNames
}

void Lego_LoadMiscObjects(lpConfig config)
{
    // TODO: Implement Lego_LoadMiscObjects
}

void Lego_LoadToolTips(lpConfig config)
{
    // TODO: Implement Lego_LoadToolTips
}

void Lego_LoadUpgradeNames(lpConfig config)
{
    // TODO: Implement Lego_LoadUpgradeNames
}

void Lego_LoadInfoMessages(lpConfig config)
{
    // TODO: Implement Lego_LoadInfoMessages
}

void Lego_LoadTextMessages(lpConfig config)
{
    // TODO: Implement Lego_LoadTextMessages
}

void Lego_LoadPanels(lpConfig config, U32 screenWidth, U32 screenHeight)
{
    char buff[256];
    sprintf(buff, "Panels%ix%i", screenWidth, screenHeight);
    lpConfig conf = Config_FindArray(config, Config_BuildStringID(legoGlobs.gameName, buff, 0));

    for (lpConfig i = conf; i; i = Config_GetNextItem(i))
    {
        Panel_Type panelType;
        if (Panel_GetPanelType(i->itemName, &panelType) != 0)
        {
            strcpy(buff, i->dataString);

            char* stringParts[10];
            Util_Tokenize(buff, stringParts, ",");
            Panel_LoadImage(stringParts[0], panelType, 4);
            S32 yIn = atoi(stringParts[4]);
            S32 xIn = atoi(stringParts[3]);
            S32 yOut = atoi(stringParts[2]);
            S32 xOut = atoi(stringParts[1]);
            Panel_SetArea(panelType, xOut, yOut, xIn, yIn);
        }
    }
}

void Lego_LoadPanelButtons(lpConfig config, U32 screenWidth, U32 screenHeight)
{
    // TODO: Implement Lego_LoadPanelButtons
}

void Lego_LoadTutorialIcon(lpConfig config)
{
    // TODO: Implement Lego_LoadTutorialIcon
}

void Lego_ShowBlockToolTip(Point2I* mouseBlockPos, B32 showConstruction, B32 playSound, B32 showCavern)
{
    // TODO: Implement Lego_ShowBlockToolTip
}

// Sets the current combination pointer/hover SFX, with a pointer duration of 12.5.
void Lego_SetPointerSFX(PointerSFX_Type pointerSFXType)
{
    B32 origQueueMode = SFX_IsQueueMode();
    SFX_SetQueueMode(FALSE, FALSE);

    switch (pointerSFXType)
    {
        case PointerSFX_Okay:
        {
            Pointer_SetCurrent(Pointer_Okay, 12.5f);
            SFX_Random_PlaySoundNormal(SFX_Okay, FALSE);
            break;
        }
        case PointerSFX_NotOkay:
        {
            Pointer_SetCurrent(Pointer_NotOkay, 12.5f);
            SFX_Random_PlaySoundNormal(SFX_NotOkay, FALSE);
            break;
        }
        case PointerSFX_NotOkay_ImmovableRock:
        {
            Pointer_SetCurrent(Pointer_NotOkay, 12.5f);
            SFX_Random_PlaySoundNormal(SFX_ImmovableRock, FALSE);
            break;
        }
        case PointerSFX_Okay_TopPriority:
        {
            Pointer_SetCurrent(Pointer_Okay, 12.5f);
            SFX_Random_PlaySoundNormal(SFX_TopPriority, FALSE);
            break;
        }
        case PointerSFX_Okay_Wall:
        {
            Pointer_SetCurrent(Pointer_Okay, 12.5f);
            SFX_Random_PlaySoundNormal(SFX_Wall, FALSE);
            break;
        }
        case PointerSFX_Okay_Floor:
        {
            Pointer_SetCurrent(Pointer_Okay, 12.5f);
            SFX_Random_PlaySoundNormal(SFX_Floor, FALSE);
            break;
        }
    }
    SFX_SetQueueMode(origQueueMode, FALSE);
}

void Lego_ClearSomeFlags3_FUN_00435950()
{
    legoGlobs.flags3 &= ~(GAME3_UNK_1|GAME3_UNK_2|GAME3_UNK_4|GAME3_PICKUPOBJECT|GAME3_LOADVEHICLE|GAME3_UNK_20|GAME3_UNK_40|GAME3_PLACEBUILDING);
    SelectPlace_Hide(legoGlobs.selectPlace, TRUE);
}

void Lego_DrawDragSelectionBox(lpLego_Level level)
{
    // TODO: Implement Lego_DrawDragSelectionBox
}

void Lego_DrawAllSelectedUnitBoxes(lpViewport viewMain)
{
    // TODO: Implement Lego_DrawAllSelectedUnitBoxes
}

void Lego_DrawAllLaserTrackerBoxes(lpViewport viewMain)
{
    // TODO: Implement Lego_DrawAllLaserTrackerBoxes
}

void Lego_DrawRenameInput(F32 elapsedAbs)
{
    // TODO: Implement Lego_DrawRenameInput
}

void Lego_HandleRadarInput()
{
    // TODO: Implement Lego_HandleRadarInput
}

// Returns TRUE if liveObj (or its drivenObj) is the first-person unit.
// BUG: When in topdown view, returns TRUE if the objectFP is not NULL and matches the unit's
// drivenObj.
// TODO: Fix vanilla bug
B32 Lego_IsFPObject(lpLegoObject liveObj)
{
    if ((legoGlobs.viewMode != ViewMode_FP || liveObj != legoGlobs.objectFP) &&
        (liveObj->driveObject == NULL || liveObj->driveObject != legoGlobs.objectFP))
    {
        return FALSE;
    }

    return TRUE;
}

B32 Lego_UpdateAll3DSounds(B32 stopAll)
{
    return LegoObject_RunThroughListsSkipUpgradeParts(Lego_UpdateObject3DSounds, &stopAll);
}

B32 Lego_UpdateObject3DSounds(lpLegoObject liveObj, void* context)
{
    B32* pStopAll = (B32*)context;
    // TODO: Implement Lego_UpdateObject3DSounds

    return FALSE;
}

// This is an old method for playing movies.
// It supports playing a movie that isn't just in the center of the screen
//  (which can be seen by setting certain Level CFG AVI properties).
// All movie playback seen in LegoRR is done by Front_PlayMovie (which supports
//  skipping, and scales to screen, but not playing at a specified position).
void Lego_PlayMovie_old(const char* fName, Point2F* optScreenPt)
{
    // TODO: Implement Lego_PlayMovie_old
}

#ifdef LEGORR_DEBUG_SHOW_INFO
void Lego_Debug_ShowInfo()
{
    Point3F debugCameraPos;
    //Camera_GetTopdownWorldPos(legoGlobs.cameraMain, legoGlobs.currLevel->map, &debugCameraPos);
    Container_GetPosition(legoGlobs.cameraMain->contCam, NULL, &debugCameraPos);
    Font_PrintF(legoGlobs.bmpToolTipFont, 10, 50, "Camera Position: (%f,%f,%f)", debugCameraPos.x, debugCameraPos.y,
                debugCameraPos.z);

    Point3F lightPos;
    Container_GetPosition(legoGlobs.rootLight, NULL, &lightPos);
    Font_PrintF(legoGlobs.bmpToolTipFont, 10, 60, "Light Position: (%f,%f,%f)", lightPos.x, lightPos.y, lightPos.z);

    Font_PrintF(legoGlobs.bmpToolTipFont, 10, 70, "Mouse Position: (%d,%d)", inputGlobs.msx, inputGlobs.msy);

    Lego_Debug_PrintAITaskInfo();
}

void Lego_Debug_PrintAITaskInfo()
{
    U32 debugAILen;
    AITask_Debug_GetInfoString(NULL, &debugAILen);

    char* debugAIInfo = Mem_Alloc(debugAILen + 1);
    AITask_Debug_GetInfoString(debugAIInfo, &debugAILen);
    debugAIInfo[debugAILen] = '\0';

    Font_PrintF(legoGlobs.bmpToolTipFont, 10, 80, "AI Info:");
    //Font_PrintF(legoGlobs.bmpToolTipFont, 10, 90, "{");
    char* debugAIInfoPtr = debugAIInfo;
    S32 debugAIInfoIt = 0;
    B32 firstTime = TRUE;
    while (*debugAIInfoPtr != '\0')
    {
        if (*debugAIInfoPtr == '\n' || firstTime)
        {
            if (!firstTime)
                debugAIInfoPtr++;
            U32 nextIndex = 0;
            while (debugAIInfoPtr[nextIndex + 1] != '\n' && debugAIInfoPtr[nextIndex + 1] != '\0')
                nextIndex++;
            nextIndex++;
            char* buf = Mem_Alloc(nextIndex + 1);
            memcpy(buf, debugAIInfoPtr, nextIndex);
            buf[nextIndex] = '\0';
            Font_PrintF(legoGlobs.bmpToolTipFont, 14, 90 + debugAIInfoIt * 10, buf);
            Mem_Free(buf);
            debugAIInfoIt++;
            firstTime = FALSE;
        }
        else
        {
            debugAIInfoPtr++;
        }
    }
    //Font_PrintF(legoGlobs.bmpToolTipFont, 10, 100 + debugAIInfoIt * 10, "}");
    Mem_Free(debugAIInfo);
}
#endif
