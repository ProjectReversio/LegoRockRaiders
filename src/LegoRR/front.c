#include "front.h"
#include "lego.h"
#include "input.h"
#include "images.h"
#include "mem.h"
#include "movie.h"
#include "file.h"

Front_Globs frontGlobs = { NULL };

B32 Front_IsFrontEndEnabled()
{
    Bool3 result = Config_GetBoolValue(legoGlobs.config, Config_BuildStringID(legoGlobs.gameName, "Main", "FrontEnd", 0));

    return result == BOOL3_TRUE && Main_ProgrammerMode() < PROGRAMMER_MODE_3;
}

B32 Front_IsIntrosEnabled()
{
    Bool3 result = Config_GetBoolValue(legoGlobs.config, Config_BuildStringID(legoGlobs.gameName, "Main", "DontPlayAvis", 0));

    return result != BOOL3_TRUE && Main_ProgrammerMode() < PROGRAMMER_MODE_3;
}

void Front_PlayIntroMovie(const char* aviKey, B32 skippable)
{
    if (aviKey == NULL)
        return;

    const char* fName = Config_GetStringValue(legoGlobs.config, Config_BuildStringID(legoGlobs.gameName, "Main", aviKey, 0));
    if (fName == NULL)
        return;

    if (File_Exists(fName))
    {
        Movie_t* mov = Movie_Load(fName);
        Front_PlayMovie(mov, skippable);
        Movie_Free(mov);
        Mem_Free(fName);
    }
}

void Front_PlayIntroSplash(const char* imageKey, B32 skippable, const char* timeKey)
{
    F32 time = Config_GetFloatValue(legoGlobs.config, Config_BuildStringID(legoGlobs.gameName, "Main", timeKey, 0));

    if (time == 0.0f)
        time = 3.0f;

    S32 startTime = timeGetTime() - (S32)(time * -1000.0f);

    if (imageKey == NULL)
        return;

    const char* bmpFile = Config_GetStringValue(legoGlobs.config, Config_BuildStringID(legoGlobs.gameName, "Main", imageKey, 0));

    if (bmpFile == NULL)
        return;

    lpImage img = Image_LoadBMP(bmpFile);
    if (img == NULL)
        return;

    do
    {
        Image_Display(img, 0);
        Main_LoopUpdate(FALSE);
        Sleep(100);
    } while (timeGetTime() < startTime && (!skippable || !Input_AnyKeyPressed() && !inputGlobs.msrb && !inputGlobs.mslb));
    Image_Remove(img);
}

void Front_PlayMovie(Movie_t* mov, B32 skippable)
{
    // HARDCODED SCREEN RESOLUTION
    RECT rect;
    rect.left = 0;
    rect.top = 0;
    rect.right = 640;
    rect.bottom = 480;

    F32 num = 0.0f;
    if (Movie_Update(mov, 1.0f, &rect))
    {
        do
        {
            Main_LoopUpdate(FALSE);
            num -= -1.0f;
        } while (Movie_GetDuration(mov) >= num
            && (!skippable || !Input_AnyKeyPressed() && !inputGlobs.msrb && !inputGlobs.mslb) &&
            Movie_Update(mov, 1.0f, &rect));
    }
}

void Front_Initialize(lpConfig config)
{
    const char* menuWipe = Config_GetStringValue(legoGlobs.config, Config_BuildStringID(legoGlobs.gameName, "Menu", "MenuWipe", 0));
    if (menuWipe)
    {
        S32 nul = _strnicmp(menuWipe, "Null", 4);
        if (nul != 0)
            frontGlobs.rockWipeAnim = Container_Load(NULL, menuWipe, "LWS", TRUE);

        Mem_Free(menuWipe);
    }
    else
    {
        menuWipe = Config_GetStringValue(legoGlobs.config, Config_BuildStringID(legoGlobs.gameName, "Menu", "!MenuWipe", 0));
        if (menuWipe)
        {
            if ((mainGlobs.flags & MAIN_FLAG_REDUCEANIMATION) == MAIN_FLAG_NONE)
            {
                S32 nul = _strnicmp(menuWipe, "Null", 4);
                if (nul != 0)
                    frontGlobs.rockWipeAnim = Container_Load(NULL, menuWipe, "LWS", TRUE);

                Mem_Free(menuWipe);
            }
        }
    }

    frontGlobs.saveTextWnd = (MenuTextWindow*) Mem_Alloc(sizeof(MenuTextWindow));
    memset(frontGlobs.saveTextWnd, 0, sizeof(MenuTextWindow));
    Front_LoadMenuTextWindow(config, "Menu::SaveText", frontGlobs.saveTextWnd);

    frontGlobs.saveLevelWnd = (MenuTextWindow*) Mem_Alloc(sizeof(MenuTextWindow));
    memset(frontGlobs.saveLevelWnd, 0, sizeof(MenuTextWindow));
    Front_LoadMenuTextWindow(config, "Menu::LevelText", frontGlobs.saveLevelWnd);

    const char* langSaveGame = Config_GetStringValue(legoGlobs.config, Config_BuildStringID(legoGlobs.gameName, "Menu", "Save_Game", 0));
    if (langSaveGame)
    {
        strcpy(frontGlobs.langSaveGame, Front_Util_ReplaceTextSpaces(langSaveGame));
        Mem_Free(langSaveGame);
    }

    const char* langLoadGame = Config_GetStringValue(legoGlobs.config, Config_BuildStringID(legoGlobs.gameName, "Menu", "Load_Game", 0));
    if (langLoadGame)
    {
        strcpy(frontGlobs.langLoadGame, Front_Util_ReplaceTextSpaces(langLoadGame));
        Mem_Free(langLoadGame);
    }

    const char* langOverwriteTitle = Config_GetStringValue(legoGlobs.config, Config_BuildStringID(legoGlobs.gameName, "Menu::Overwrite", "Title", 0));
    if (langOverwriteTitle)
    {
        strcpy(frontGlobs.langOverwriteTitle, Front_Util_ReplaceTextSpaces(langOverwriteTitle));
        Mem_Free(langOverwriteTitle);
    }

    const char* langOverwriteMessage = Config_GetStringValue(legoGlobs.config, Config_BuildStringID(legoGlobs.gameName, "Menu::Overwrite", "Text", 0));
    if (langOverwriteMessage)
    {
        strcpy(frontGlobs.langOverwriteMessage, Front_Util_ReplaceTextSpaces(langOverwriteMessage));
        Mem_Free(langOverwriteMessage);
    }

    const char* langOverwriteOK = Config_GetStringValue(legoGlobs.config, Config_BuildStringID(legoGlobs.gameName, "Menu::Overwrite", "Ok", 0));
    if (langOverwriteOK)
    {
        strcpy(frontGlobs.langOverwriteOK, Front_Util_ReplaceTextSpaces(langOverwriteOK));
        Mem_Free(langOverwriteOK);
    }

    const char* langOverwriteCancel = Config_GetStringValue(legoGlobs.config, Config_BuildStringID(legoGlobs.gameName, "Menu::Overwrite", "Cancel", 0));
    if (langOverwriteCancel)
    {
        strcpy(frontGlobs.langOverwriteCancel, Front_Util_ReplaceTextSpaces(langOverwriteCancel));
        Mem_Free(langOverwriteCancel);
    }

    frontGlobs.versionFont = legoGlobs.bmpMbriefFONT2;
    frontGlobs.versionString = Config_GetStringValue(legoGlobs.config, Config_BuildStringID(legoGlobs.gameName, "Main", "Version", 0));
    frontGlobs.strDefaultLevelBMPS = Config_GetStringValue(legoGlobs.config, Config_BuildStringID(legoGlobs.gameName, "Menu", "DefaultLevelBMPS", 0));
    frontGlobs.rockWipeLight = Container_MakeLight(legoGlobs.rootCont, D3DRMLIGHT_DIRECTIONAL, 0.8f, 0.8f, 0.8f);
    Container_Hide(frontGlobs.rockWipeLight, TRUE);

    frontGlobs.mainMenuSet = Front_LoadMenuSet(config, "MainMenuFull",
                                               &frontGlobs.triggerCredits, Front_Callback_TriggerPlayCredits, // "Main" Trigger "Credits"
                                               &frontGlobs.triggerQuitApp, NULL, // "ARE_YOU_SURE?" Trigger "Yes"
                                               -1); // end
    frontGlobs.saveMenuSet = Front_LoadMenuSet(config, "SaveMenu",
                                               &frontGlobs.triggerBackSave, Front_Callback_TriggerBackSave, // "Load_Level_Save" Trigger "Back"
                                               -1); // end

    Front_LoadLevelSet(legoGlobs.config, &frontGlobs.missionLevels, "StartLevel");
    Front_LoadLevelSet(legoGlobs.config, &frontGlobs.tutorialLevels, "TutorialStartLevel");

    // NOTE: Front_Levels_ResetVisited CANNOT be called between Front_LevelSelect_LoadLevelSet and Front_LevelSelect_LoadLevels.

    Front_LoadLevels(frontGlobs.mainMenuSet);

    frontGlobs.pausedMenuSet = Front_LoadMenuSet(config, "PausedMenu",
                                                 &frontGlobs.triggerContinueMission, NULL, // "Paused" Trigger "Continue_Game"
                                                 &frontGlobs.sliderGameSpeed, Front_Callback_SliderGameSpeed, // "Options" Trigger "Game_Speed"
                                                 &frontGlobs.sliderSFXVolume, Front_Callback_SliderSoundVolume, // "Options" Trigger "SFX_Volume"
                                                 &frontGlobs.sliderMusicVolume, Front_Callback_SliderMusicVolume, // "Options" Trigger "Music_Volume"
                                                 &frontGlobs.sliderBrightness, Front_Callback_SliderBrightness, // "Options" Trigger "Brightness"
                                                 &frontGlobs.cycleHelpWindow, Front_Callback_CycleHelpWindow, // "Options" Cycle "Help_Window"
                                                 &frontGlobs.triggerReplayObjective, Front_Callback_TriggerReplayObjective, // "Options" Trigger "Replay_Objective"
                                                 &frontGlobs.triggerQuitMission, NULL, // "Quit?" Trigger "Yes_-_Quit"
                                                 &frontGlobs.triggerRestartMission, NULL, // "Restart_Mission?" Trigger "Yes_-_Restart_Mission"
                                                 &frontGlobs.cycleWallDetail, Front_Callback_CycleWallDetail, // "Advanced_Options" Cycle "Wall_Detail"
                                                 &frontGlobs.cycleMusicOn, Front_Callback_CycleMusic, // "Advanced_Options" Cycle "Music"
                                                 &frontGlobs.cycleSFXOn, Front_Callback_CycleSound, // "Advanced_Options" Cycle "SFX"
                                                 &frontGlobs.cycleAutoGameSpeed, Front_Callback_CycleAutoGameSpeed, // "Advanced_Options" Cycle "Automatic_Game_Speed"
                                                 -1); // end

    frontGlobs.optionsMenuSet = Front_LoadMenuSet(config, "OptionsMenu",
                                                  &frontGlobs.sliderGameSpeed, Front_Callback_SliderGameSpeed, // "Options" Slider "Game_Speed"
                                                  &frontGlobs.sliderSFXVolume, Front_Callback_SliderSoundVolume, // "Options" Slider "SFX_Volume"
                                                  &frontGlobs.sliderMusicVolume, Front_Callback_SliderMusicVolume, // "Options" Slider "Music_Volume"
                                                  &frontGlobs.sliderBrightness, Front_Callback_SliderBrightness, // "Options" Slider "Brightness"
                                                  &frontGlobs.cycleHelpWindow, Front_Callback_CycleHelpWindow, // "Options" Cycle "Help_Window"
                                                  &frontGlobs.triggerReplayObjective, Front_Callback_TriggerReplayObjective, // "Options" Trigger "Replay_Objective"
                                                  &frontGlobs.triggerContinueMission, NULL, // "Options" Trigger "Continue_Game"
                                                  -1); // end

    frontGlobs.sliderSFXVolume = 8;
    frontGlobs.triggerContinueMission = 0;
    frontGlobs.sliderBrightness = 5;
    frontGlobs.cycleHelpWindow = 1;
    frontGlobs.triggerReplayObjective = 0;
    frontGlobs.sliderGameSpeed = Front_CalcSliderGameSpeed();
    frontGlobs.sliderMusicVolume = Front_CalcSliderCDVolume();
    frontGlobs.saveBool_540 = FALSE;

    frontGlobs.maxLevelScreens = Config_GetIntValue(config, Config_BuildStringID(legoGlobs.gameName, "Menu", "MaxLevelScreens", 0));
    if (frontGlobs.maxLevelScreens == 0)
        frontGlobs.maxLevelScreens = 1;

    frontGlobs.unused_zero_864 = 0;

    frontGlobs.saveImageBigSize.width = Config_GetIntValue(config, Config_BuildStringID(legoGlobs.gameName, "Menu", "SaveImage", "BigWidth", 0));
    if (frontGlobs.saveImageBigSize.width == 0)
        frontGlobs.saveImageBigSize.width = 80;

    frontGlobs.saveImageBigSize.height = Config_GetIntValue(config, Config_BuildStringID(legoGlobs.gameName, "Menu", "SaveImage", "BigHeight", 0));
    if (frontGlobs.saveImageBigSize.height == 0)
        frontGlobs.saveImageBigSize.height = 60;

    Front_Save_SetBool_85c(TRUE);
}

void Front_ResetSaveNumber()
{
    // TODO: Implement Front_ResetSaveNumber
}

void Front_RunScreenMenuType(Menu_ScreenType screenType)
{
    // TODO: Implement Front_RunScreenMenuType
}

B32 Front_IsTriggerAppQuit()
{
    // TODO: Implement Front_IsTriggerAppQuit
    return FALSE;
}

const char* Front_GetSelectedLevel()
{
    // TODO: Implement Front_GetSelectedLevel
    return NULL;
}

void Front_LoadOptionParameters(B32 loadOptions, B32 resetFront)
{
    // TODO: Implement Front_LoadOptionParameters
}

void Front_LoadMenuTextWindow(lpConfig config, const char* gameName, lpMenuTextWindow menuWnd)
{
    // TODO: Implement Front_LoadMenuTextWindow
}

lpMenuSet Front_LoadMenuSet(lpConfig config, const char* menuName, void* dst, void* callback, ...)
{
    // TODO: Implement Front_LoadMenuSet
    return NULL;
}

B32 Front_LoadLevelSet(lpConfig config, lpLevelSet levelSet, const char* levelKey)
{
    // TODO: Implement Front_LoadLevelSet
    return FALSE;
}

void Front_LoadLevels(lpMenuSet mainMenuFull)
{
    // TODO: Implement Front_LoadLevels
}

void Front_Callback_TriggerPlayCredits()
{
    // TODO: Implement Front_Callback_TriggerPlayCredits
}

void Front_Callback_TriggerBackSave()
{
    // TODO: Implement Front_Callback_TriggerBackSave
}

void Front_Callback_SliderGameSpeed(S32 slider_0_5)
{
    // TODO: Implement Front_Callback_SliderGameSpeed
}

void Front_Callback_SliderSoundVolume(S32 slider_0_10)
{
    // TODO: Implement Front_Callback_SliderSoundVolume
}

void Front_Callback_SliderMusicVolume(S32 slider_0_10)
{
    // TODO: Implement Front_Callback_SliderMusicVolume
}

void Front_Callback_SliderBrightness(S32 slider_0_10)
{
    // TODO: Implement Front_Callback_SliderBrightness
}

void Front_Callback_CycleHelpWindow(S32 cycle_On_Off)
{
    // TODO: Implement Front_Callback_CycleHelpWindow
}

void Front_Callback_TriggerReplayObjective()
{
    // TODO: Implement Front_Callback_TriggerReplayObjective
}

void Front_Callback_CycleWallDetail(S32 cycle_High_Low)
{
    // TODO: Implement Front_Callback_CycleWallDetail
}

void Front_Callback_CycleMusic(S32 cycle_On_Off)
{
    // TODO: Implement Front_Callback_CycleMusic
}

void Front_Callback_CycleSound(S32 cycle_On_Off)
{
    // TODO: Implement Front_Callback_CycleSound
}

void Front_Callback_CycleAutoGameSpeed(S32 cycle_On_Off)
{
    // TODO: Implement Front_Callback_CycleAutoGameSpeed
}

S32 Front_CalcSliderGameSpeed()
{
    // TODO: Implement Front_CalcSliderGameSpeed
    return 0;
}

S32 Front_CalcSliderCDVolume()
{
    // TODO: Implement Front_CalcSliderCDVolume
    return 0;
}

void Front_Save_SetBool_85c(B32 state)
{
    frontGlobs.saveBool_85c = state;
}

const char* Front_Util_StringReplaceChar(const char* str, char origChar, char newChar)
{
    char* curr = str;
    for (char i = *str; i; curr++ )
    {
        if ( i == origChar )
            *curr = newChar;
        i = curr[1];
    }
    return str;
}

const char* Front_Util_ReplaceTextSpaces(const char* str)
{
    static char s_buffer[256];

    if (strlen(str) >= 256)
        return str;

    strcpy(s_buffer, str);

    for (U32 i = 0; i < 256; i++)
    {
        if (s_buffer[i] == '_')
            s_buffer[i] = ' ';
    }

    return s_buffer;
}
