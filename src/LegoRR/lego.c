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
    {
        // TODO: This for loop is in the original code with no contents.
        //  It's very odd as it should have been optimized out by the compiler.
        //  Either it's a bad decompilation, or it's something that got removed.
        //  Potentially something only in debug mode.
    }

    Lego_SetGameSpeed(1.0f);

    Keys_Initialize();
    Viewport_Initialize();
    LegoObject_Initialize();
    Image_Initialize();
    //Unknown_Initialize(); // TODO: Figure out what function this should be. It's empty in the decompile, so likely debug builds only.
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

    // TODO: Implement Lego_Initialize

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

void Object_LoadToolNames(lpConfig config, const char* gameName)
{
    // TODO: Implement Object_LoadToolNames
}
