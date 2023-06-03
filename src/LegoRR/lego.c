#include "lego.h"
#include <string.h>
#include <stdio.h>
#include "types.h"
#include "main.h"
#include "front.h"
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

Lego_Globs legoGlobs;

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
    if ((mainGlobs.flags & MAIN_FLAG_SHOWVERSION) != MAIN_FLAG_NONE)
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
    Lws_Initialize(sharedObjects, SFX_GetType, SFX_Random_Play_OrInitSoundUnk, SFX_IsSoundOn);

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

    legoGlobs.tvTiltOrZoom_334 = 15.0f;
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
        SFX_Random_Play_OrAddToQueue(SFX_AmbientLoop, TRUE);
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

B32 Lego_MainLoop(F32 time)
{
    // TODO: Implement Lego_MainLoop

    return TRUE;
}

B32 Lego_LoadLevel(const char* levelName)
{
    // TODO: Implement Lego_LoadLevel
    return TRUE;
}

void Lego_SetSoundOn(B32 isSoundOn)
{
    // TODO: Implement Lego_SetSoundOn
}

void Lego_SetGameSpeed(F32 newGameSpeed)
{
    // TODO: Implement Lego_SetGameSpeed
}

void Lego_LoadSamples(lpConfig config, B32 noReduceSamples)
{
    // TODO: Implement Lego_LoadSamples
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
    // TODO: Implement Lego_LoadLighting
    return TRUE;
}

B32 Lego_LoadGraphicsSettings()
{
    // TODO: Implement Lego_LoadGraphicsSettings
    return TRUE;
}

B32 Lego_LoadUpgradeTypes()
{
    // TODO: Implement Lego_LoadUpgradeTypes
    return TRUE;
}

B32 Lego_LoadVehicleTypes()
{
    // TODO: Implement Lego_LoadVehicleTypes
    return TRUE;
}

B32 Lego_LoadMiniFigureTypes()
{
    // TODO: Implement Lego_LoadMiniFigureTypes
    return TRUE;
}

B32 Lego_LoadRockMonsterTypes()
{
    // TODO: Implement Lego_LoadRockMonsterTypes
    return TRUE;
}

B32 Lego_LoadBuildingTypes()
{
    // TODO: Implement Lego_LoadBuildingTypes
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
    // TODO: Implement Lego_LoadPanels
}

void Lego_LoadPanelButtons(lpConfig config, U32 screenWidth, U32 screenHeight)
{
    // TODO: Implement Lego_LoadPanelButtons
}

void Lego_LoadTutorialIcon(lpConfig config)
{
    // TODO: Implement Lego_LoadTutorialIcon
}
