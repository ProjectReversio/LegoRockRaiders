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

B32 Lego_Initialize()
{
    // TODO: Implement Lego_Initialize

    Image_Initialize();

    // TODO: Implement Lego_Initialize

    legoGlobs.bmpFONT5_HI = Font_Load("Interface\\Fonts\\FONT5_HI.bmp");
    legoGlobs.bmpToolTipFont = Font_Load("Interface\\Fonts\\ToolTipFont.bmp");
    legoGlobs.bmpDesktopFont = Font_Load("Interface\\Fonts\\DeskTopFont.bmp");
    legoGlobs.bmpfont5_HI = Font_Load("Interface\\Fonts\\font5_HI.bmp");
    legoGlobs.bmpMbriefFONT = Font_Load("Interface\\Fonts\\MbriefFONT.bmp");
    legoGlobs.bmpMbriefFONT2 = Font_Load("Interface\\Fonts\\MbriefFONT2.bmp");
    legoGlobs.bmpRSFont = Font_Load("Interface\\Fonts\\RSFont.bmp");

    // TODO: Implement Lego_Initialize

    legoGlobs.rootCont = Container_Initialize(legoGlobs.gameName);
    if (legoGlobs.rootCont == NULL)
    {
        Error_Warn(TRUE, "Failed to initialize root container");
        return FALSE;
    }

    // TODO: Implement Lego_Initialize

    legoGlobs.config = Config_Load("Lego.cfg");
    if (legoGlobs.config == NULL)
    {
        Error_Warn(TRUE, "Failed to load config file 'Lego.cfg'");
        Container_Shutdown();
        return FALSE;
    }

    // TODO: Implement Lego_Initialize

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

    // TODO: Implement Lego_Initialize

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
