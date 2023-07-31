#include <stdio.h>
#include "front_end.h"
#include "lego.h"
#include "input.h"
#include "images.h"
#include "mem.h"
#include "movie.h"
#include "file.h"
#include "utils.h"
#include "3DSound.h"
#include "tooltip.h"
#include "pointer.h"
#include "credits.h"
#include "search.h"
#include "keys.h"

Front_Globs frontGlobs = { NULL };

lpFront_Cache g_ImageCache_NEXT = NULL;

S32 g_frontSelectHoverIndex = -1;
B32 g_saveMenuOverwriteShowing = FALSE;
lpMenu g_saveMenu_UnkNextMenu;
S32 g_saveMenuSelectedIndex = -1;
S32 g_saveMenuOverwriteResult = -1;
S32 g_saveMenuOverlayState = -1;
S32 g_saveMenuSelectingIndex = -1;
S32 g_saveMenuOutputSelectedIndex = -1;
B32 g_saveMenuOverlayPlaying;
B32 g_levelSelectPrinting;

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
                                               &frontGlobs.menuVars[MENUVAR_TRIGGER_CREDITS], Front_Callback_TriggerPlayCredits, // "Main" Trigger "Credits"
                                               &frontGlobs.menuVars[MENUVAR_TRIGGER_QUITAPP], NULL, // "ARE_YOU_SURE?" Trigger "Yes"
                                               -1); // end
    frontGlobs.saveMenuSet = Front_LoadMenuSet(config, "SaveMenu",
                                               &frontGlobs.menuVars[MENUVAR_TRIGGER_BACKSAVE], Front_Callback_TriggerBackSave, // "Load_Level_Save" Trigger "Back"
                                               -1); // end

    Front_LoadLevelSet(legoGlobs.config, &frontGlobs.missionLevels, "StartLevel");
    Front_LoadLevelSet(legoGlobs.config, &frontGlobs.tutorialLevels, "TutorialStartLevel");

    // NOTE: Front_Levels_ResetVisited CANNOT be called between Front_LevelSelect_LoadLevelSet and Front_LevelSelect_LoadLevels.

    Front_LoadLevels(frontGlobs.mainMenuSet);

    frontGlobs.pausedMenuSet = Front_LoadMenuSet(config, "PausedMenu",
                                                 &frontGlobs.menuVars[MENUVAR_TRIGGER_CONTINUE_MISSION], NULL, // "Paused" Trigger "Continue_Game"
                                                 &frontGlobs.menuVars[MENUVAR_SLIDER_GAMESPEED], Front_Callback_SliderGameSpeed, // "Options" Trigger "Game_Speed"
                                                 &frontGlobs.menuVars[MENUVAR_SLIDER_SFXVOLUME], Front_Callback_SliderSoundVolume, // "Options" Trigger "SFX_Volume"
                                                 &frontGlobs.menuVars[MENUVAR_SLIDER_MUSICVOLUME], Front_Callback_SliderMusicVolume, // "Options" Trigger "Music_Volume"
                                                 &frontGlobs.menuVars[MENUVAR_SLIDER_BRIGHTNESS], Front_Callback_SliderBrightness, // "Options" Trigger "Brightness"
                                                 &frontGlobs.menuVars[MENUVAR_CYCLE_HELPWINDOW], Front_Callback_CycleHelpWindow, // "Options" Cycle "Help_Window"
                                                 &frontGlobs.menuVars[MENUVAR_TRIGGER_REPLAYOBJECTIVE], Front_Callback_TriggerReplayObjective, // "Options" Trigger "Replay_Objective"
                                                 &frontGlobs.menuVars[MENUVAR_TRIGGER_QUITMISSION], NULL, // "Quit?" Trigger "Yes_-_Quit"
                                                 &frontGlobs.menuVars[MENUVAR_TRIGGER_RESTARTMISSION], NULL, // "Restart_Mission?" Trigger "Yes_-_Restart_Mission"
                                                 &frontGlobs.menuVars[MENUVAR_CYCLE_WALLDETAIL], Front_Callback_CycleWallDetail, // "Advanced_Options" Cycle "Wall_Detail"
                                                 &frontGlobs.menuVars[MENUVAR_CYCLE_MUSICON], Front_Callback_CycleMusic, // "Advanced_Options" Cycle "Music"
                                                 &frontGlobs.menuVars[MENUVAR_CYCLE_SFXON], Front_Callback_CycleSound, // "Advanced_Options" Cycle "SFX"
                                                 &frontGlobs.menuVars[MENUVAR_CYCLE_AUTOGAMESPEED], Front_Callback_CycleAutoGameSpeed, // "Advanced_Options" Cycle "Automatic_Game_Speed"
                                                 -1); // end

    frontGlobs.optionsMenuSet = Front_LoadMenuSet(config, "OptionsMenu",
                                                  &frontGlobs.menuVars[MENUVAR_SLIDER_GAMESPEED], Front_Callback_SliderGameSpeed, // "Options" Slider "Game_Speed"
                                                  &frontGlobs.menuVars[MENUVAR_SLIDER_SFXVOLUME], Front_Callback_SliderSoundVolume, // "Options" Slider "SFX_Volume"
                                                  &frontGlobs.menuVars[MENUVAR_SLIDER_MUSICVOLUME], Front_Callback_SliderMusicVolume, // "Options" Slider "Music_Volume"
                                                  &frontGlobs.menuVars[MENUVAR_SLIDER_BRIGHTNESS], Front_Callback_SliderBrightness, // "Options" Slider "Brightness"
                                                  &frontGlobs.menuVars[MENUVAR_CYCLE_HELPWINDOW], Front_Callback_CycleHelpWindow, // "Options" Cycle "Help_Window"
                                                  &frontGlobs.menuVars[MENUVAR_TRIGGER_REPLAYOBJECTIVE], Front_Callback_TriggerReplayObjective, // "Options" Trigger "Replay_Objective"
                                                  &frontGlobs.menuVars[MENUVAR_TRIGGER_CONTINUE_MISSION], NULL, // "Options" Trigger "Continue_Game"
                                                  -1); // end

    frontGlobs.menuVars[MENUVAR_SLIDER_SFXVOLUME] = 8;
    frontGlobs.menuVars[MENUVAR_TRIGGER_CONTINUE_MISSION] = 0;
    frontGlobs.menuVars[MENUVAR_SLIDER_BRIGHTNESS] = 5;
    frontGlobs.menuVars[MENUVAR_CYCLE_HELPWINDOW] = 1;
    frontGlobs.menuVars[MENUVAR_TRIGGER_REPLAYOBJECTIVE] = 0;
    frontGlobs.menuVars[MENUVAR_SLIDER_GAMESPEED] = Front_CalcSliderGameSpeed();
    frontGlobs.menuVars[MENUVAR_SLIDER_MUSICVOLUME] = Front_CalcSliderCDVolume();
    frontGlobs.saveMenuHasNoData = FALSE;

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

    Front_Save_SetShouldClearUnlockedLevels(TRUE);
}

void Front_ResetSaveNumber()
{
    // TODO: Implement Front_ResetSaveNumber
}

void Front_RunScreenMenuType(Menu_ScreenType screenType)
{
    lpMenuSet menuSet;
    U32 menuIndex;

    menuSet = frontGlobs.mainMenuSet;
    Front_PrepareScreenMenuType(screenType);
    SFX_AddToQueue(SFX_AmbientMusicLoop, SoundMode_Loop);

    switch (screenType)
    {
        case Menu_Screen_Title:
        {
            // MainMenuFull::Menu1 "Main"
            menuIndex = 0;
            break;
        }
        case Menu_Screen_Missions:
        {
            // MainMenuFull::Menu2 "Missions"
            menuIndex = 1;
            break;
        }
        case Menu_Screen_Training:
        {
            // MainMenuFull::Menu3 "Training_Missions"
            menuIndex = 2;
            break;
        }
        case Menu_Screen_Save:
        {
            // SaveMenu::Menu1 "Load_A_Saved_Game"  (save a game)
            menuIndex = 0;
            menuSet = frontGlobs.saveMenuSet;
            break;
        }
        default:
        {
            Sound3D_Stream_Stop(TRUE);
            Sound3D_Stream_Stop(FALSE);
            return;
        }
    }

    Front_RunScreenMenu(menuSet, menuIndex);
    Sound3D_Stream_Stop(TRUE);
    Sound3D_Stream_Stop(FALSE);
}

void Front_RunScreenMenu(lpMenuSet menuSet, U32 menuIndex)
{
    S32 i;
    S32 i_next;

    i = 0;
    if (menuSet->menuCount > 0)
    {
        do
        {
            i_next = i + 1;
            menuSet->menus[i]->closed = FALSE;
            i = i_next;
        } while (i_next < menuSet->menuCount);
    }
    Front_ScreenMenuLoop(menuSet->menus[menuIndex]);
}

void Front_PrepareScreenMenuType(Menu_ScreenType screenType)
{
    switch (screenType)
    {
        case Menu_Screen_Title:
            frontGlobs.menuVars[MENUVAR_SELECT_LOADSAVE_INDEX] = -1;
            Front_LoadOptionParameters(TRUE, TRUE);
            break;

        case Menu_Screen_Missions:
        case Menu_Screen_Training:
            Front_LoadOptionParameters(TRUE, TRUE);
            break;

        case Menu_Screen_Load_unused:
            Front_LoadOptionParameters(FALSE, TRUE);
            Front_Save_SetShouldClearUnlockedLevels(TRUE);
            return; // Skip logic after switch statement.

        case Menu_Screen_Save:
            Front_LoadOptionParameters(FALSE, TRUE);
            break;

        default:
            Front_Save_SetShouldClearUnlockedLevels(TRUE);
            return; // Skip logic after switch statement.
    }

    lpMenuItem_SelectData missionSelect = frontGlobs.mainMenuSet->menus[1]->items[1]->itemData.select;
    lpMenuItem_SelectData tutorialSelect = frontGlobs.mainMenuSet->menus[2]->items[1]->itemData.select;

    lpSaveData currSave = Front_Save_GetCurrentSaveData();
    if (currSave != NULL)
        Front_Levels_UpdateAvailable(frontGlobs.startMissionLink, currSave->missionsTable, &frontGlobs.missionLevels, missionSelect, FALSE);
    else if (frontGlobs.shouldClearUnlockedLevels)
    {
        // No active save, so reset all levels to their default locked state.
        Front_Levels_UpdateAvailable(frontGlobs.startMissionLink, NULL, &frontGlobs.missionLevels, missionSelect, TRUE);

        if (screenType == Menu_Screen_Save)
        {
            // We have no active save data. So if a save is selected, an empty save file will be written for it.
            frontGlobs.saveMenuHasNoData = TRUE;
        }
    }

    Front_Levels_UpdateAvailable(frontGlobs.startTutorialLink, NULL, &frontGlobs.tutorialLevels, tutorialSelect, FALSE);

    Front_Save_SetShouldClearUnlockedLevels(TRUE);
}

void Front_ScreenMenuLoop(lpMenu menu)
{
    lpMenu nextMenu;

    B8 menuChanged = FALSE;
    B32 menuTransitioning = FALSE;
    F32 timeStep = 1.0f;

    Front_LoadSaveSlotImages();

    g_saveMenu_UnkNextMenu = NULL;
    g_saveMenuOverlayState = -1;
    g_saveMenuSelectedIndex = -1;
    g_saveMenuSelectingIndex = -1;
    g_saveMenuOverwriteResult = -1;
    g_saveMenuOutputSelectedIndex = -1;
    g_saveMenuOverlayPlaying = FALSE;
    g_saveMenuOverwriteShowing = FALSE;
    g_levelSelectPrinting = FALSE;

    frontGlobs.saveMenuHasSaved = TRUE;
    frontGlobs.saveMenuKeepOpen = TRUE;
    frontGlobs.overlayImageOrFlic = NULL;

    U32 previousTime = timeGetTime();
    B32 menuClosed = menu->closed;
    F32 elapsed = timeStep;

    while (timeStep = elapsed, !menuClosed)
    {
        if (menuChanged)
            Front_Menu_Update(elapsed, menu, &menuTransitioning);
        else
            nextMenu = Front_Menu_Update(elapsed, menu, NULL);

        if (menu == frontGlobs.mainMenuSet->menus[0] && nextMenu != menu)
        {
            frontGlobs.menuVars[MENUVAR_SELECT_LOADSAVE_INDEX] = -1;
            Front_Callback_SelectLoadSave(elapsed, -1);
        }

        Front_Menu_UpdateMousePosition(menu);
        ToolTip_Update(inputGlobs.msx, inputGlobs.msy, elapsed);
        SFX_Update(elapsed);

        Main_LoopUpdate(FALSE);

        U32 newTime = timeGetTime();
        timeStep = (F32)(U64)(newTime - previousTime) * 0.025f;
        previousTime = newTime;

        if (!menuChanged && nextMenu != menu)
        {
            Front_RockWipe_Play();
            menuChanged = TRUE;
            if (frontGlobs.rockWipeAnim == NULL)
                menuTransitioning = TRUE;
        }

        if (menuChanged && menuTransitioning)
        {
            if (menu->menuImage != nextMenu->menuImage && frontGlobs.overlayImageOrFlic != NULL)
            {
                Flic_Close(frontGlobs.overlayImageOrFlic);
                Mem_Free(frontGlobs.overlayImageOrFlic);
                Sound3D_Stream_Stop(FALSE);
                frontGlobs.overlayImageOrFlic = NULL;
                frontGlobs.overlayStartTime = 0;
                frontGlobs.overlayCurrTime = 0;
            }

            frontGlobs.scrollOffset.y = 0;
            frontGlobs.scrollOffset.x = 0;
            menuChanged = FALSE;
            menuTransitioning = FALSE;
            menu = nextMenu;
        }

        elapsed = timeStep;
        menuClosed = menu->closed;
    }
    Front_FreeSaveSlotImages();

    RewardLevel *rewardLevel;
    if ((nextMenu == frontGlobs.saveMenuSet->menus[0] && -1 < g_saveMenuOutputSelectedIndex && (rewardLevel = Reward_GetRewardLevel(), rewardLevel != NULL)) &&
        (rewardLevel = Reward_GetRewardLevel(), rewardLevel->saveHasCapture != 0))
    {
        const char* saveImage = Config_GetStringValue(legoGlobs.config, Config_BuildStringID(legoGlobs.gameName, "Menu::SaveImage", "Path", 0));

        char buff[128];
        sprintf(buff, "%s\\%d.dat", saveImage, g_saveMenuOutputSelectedIndex);
        Image_SaveBMP(&rewardLevel->saveCaptureImage, buff);
    }

    if (frontGlobs.overlayImageOrFlic != NULL)
    {
        Flic_Close(frontGlobs.overlayImageOrFlic);
        Mem_Free(frontGlobs.overlayImageOrFlic);
        Sound3D_Stream_Stop(FALSE);
        frontGlobs.overlayImageOrFlic = NULL;
        frontGlobs.overlayStartTime = 0;
        frontGlobs.overlayCurrTime = 0;
    }

    Front_RockWipe_Stop();
}

lpMenu Front_Menu_Update(F32 elapsed, lpMenu menu, B32 *menuTransition)
{
    Front_Save_GetCurrentSaveData();
    Pointer_Type currPointer = Pointer_GetCurrentType();
    lpViewport view = legoGlobs.viewMain;
    lpContainer camera = Viewport_GetCamera(view);

    g_saveMenuSelectedIndex = -1;
    menu->itemFocus = -1;
    if (menuTransition)
        *menuTransition = FALSE;
    Pointer_SetCurrent_IfTimerFinished(Pointer_Standard);

    lpMenu currMenu = menu;
    lpMenu nextMenu = menu;
    if (!g_saveMenuOverwriteShowing && g_saveMenu_UnkNextMenu == NULL)
    {
        currMenu = Front_Menu_UpdateMenuItemsInput(elapsed, menu);
    }

    // Are we in the Save Game menu?
    if (menu == frontGlobs.saveMenuSet->menus[0] && g_saveMenuSelectedIndex >= 0 && g_saveMenuOverwriteResult < 0)
    {
        menu->closed = FALSE;
        currMenu->closed = FALSE;
    }
    if (g_saveMenuOverlayState >= 0)
    {
        currMenu = nextMenu;
    }

    Point3F origCamUp;
    Point3F origCamDir;
    Point3F origCamPos;

    if (frontGlobs.rockWipeAnim != NULL && (frontGlobs.rockWipeFlags & ROCKWIPE_FLAG_ANIMATING))
    {
        Viewport_Clear(view, TRUE);

        // Setup camera and add rockwipe
        Container_SetParent(frontGlobs.rockWipeAnim, legoGlobs.rootCont);
        camera = Viewport_GetCamera(view);
        Container_GetPosition(camera, NULL, &origCamPos);
        Container_GetOrientation(camera, NULL, &origCamDir, &origCamUp);
        Container_SetPosition(camera, NULL, 0.0f, 0.0f, 5.0f);
        Container_SetOrientation(camera, NULL, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f);
        Container_SetAnimationTime(frontGlobs.rockWipeAnim, frontGlobs.rockWipeSFXTimer);
    }

    Front_Menu_DrawMenuImage(menu, TRUE);

    if (menu != frontGlobs.saveMenuSet->menus[0])
        Front_Menu_UpdateOverlays(menu);

    Front_Menu_DrawLoadSaveText(&menu, &currMenu, &nextMenu);

    if (menu->displayTitle)
    {
        U32 stringWidth = Font_GetStringWidth(menu->menuFont,
                                              Front_Util_ReplaceTextSpaces(menu->fullName));

        Font_PrintF(menu->menuFont,
                    (S32)(((S32)mainGlobs.appWidth / 2) - (stringWidth >> 1)),
                    menu->anchoredPosition.y + menu->position.y,
                    Front_Util_ReplaceTextSpaces(menu->fullName));
    }

    g_frontSelectHoverIndex = -1;

    B32 BVar14;
    for (U32 i = 0; i < menu->itemCount; i++)
    {
        if ((menu->itemFocus == i &&
            !g_saveMenuOverwriteShowing) &&
            (BVar14 = Front_MenuItem_CheckNotInTutoAnyTutorialFlags(menu->items[i]), BVar14 == 0))
        {
            // Current item is focused

            lpMenuItem item = menu->items[i];

            if (item->fontHi != NULL)
            {
                Font_PrintF(item->fontHi,
                            item->centerOffHi + item->x1 + menu->position.x,
                            item->y1 + menu->position.y,
                            "%s",
                            Front_Util_ReplaceTextSpaces(item->banner));
            }

            switch (item->itemType)
            {
                case MenuItem_Type_Cycle:
                {
                    // TODO: Implement Front_Menu_Update
                    break;
                }
                case MenuItem_Type_TextInput:
                {
                    // TODO: Implement Front_Menu_Update
                    break;
                }
                case MenuItem_Type_Slider:
                {
                    // TODO: Implement Front_Menu_Update
                    break;
                }
                case MenuItem_Type_RealSlider:
                {
                    // TODO: Implement Front_Menu_Update
                    break;
                }
                case MenuItem_Type_Select:
                {
                    lpMenuItem_SelectData select = item->itemData.select;

                    U32 selIndex = Front_MenuItem_Select_TestStringCollision(menu, item, select);
                    g_frontSelectHoverIndex = selIndex;

                    for (S32 i = 0; i < select->scrollCount; i++)
                    {
                        S32 currIndex = select->scrollStart + i;
                        if ((S32)currIndex >= (S32)select->selItemCount)
                            break;

                        lpMenuItem_SelectItem selectItem = &select->selItemList[currIndex];
                        B32 isSelected = (currIndex == selIndex);

                        MenuItem_SelectImageType imageType = MenuItem_SelectImage_Light;
                        if (!(selectItem->flags & SELECTITEM_FLAG_ENABLED))
                            imageType = MenuItem_SelectImage_Locked;
                        else if (!isSelected)
                            imageType = MenuItem_SelectImage_Dark;

                        S32 centerOff = 0;
                        if (menu->autoCenter)
                            centerOff = -((S32)select->widths[MenuItem_SelectImage_Light][currIndex] / 2);

                        S32 selItemX = select->x2 + item->x1 + menu->position.x + centerOff +
                            selectItem->frontEndX + frontGlobs.scrollOffset.x;
                        S32 selItemY = select->y2 + item->y1 + menu->position.y + (select->selItemHeight * i) +
                            selectItem->frontEndY + frontGlobs.scrollOffset.y;

                        if (isSelected)
                        {
                            Front_MenuItem_DrawSelectItem(selItemX, selItemY, item->fontHi, select, currIndex, imageType);

                            if (select->callback != NULL)
                                select->callback(elapsed, selIndex);
                        }
                        else
                            Front_MenuItem_DrawSelectItem(selItemX, selItemY, item->fontLo, select, currIndex, imageType);
                        Front_MenuItem_DrawSaveImage(menu, currIndex, select, isSelected);
                    }

                    if (select->scrollStart < (S32)(select->selItemCount - select->scrollCount))
                    {
                        lpFont font = ((selIndex == -2) ? item->fontHi : item->fontLo);
                        if (font != NULL)
                        {
                            Font_PrintF(font,
                                        select->xString1 + menu->position.x,
                                        select->yString1 + menu->position.y,
                                        select->string1);
                        }
                    }

                    if (select->scrollStart > 0)
                    {
                        lpFont font = ((selIndex == -3) ? item->fontHi : item->fontLo);
                        if (font != NULL)
                        {
                            Font_PrintF(font,
                                        select->xString2 + menu->position.x,
                                        select->yString2 + menu->position.y,
                                        select->string2);
                        }
                    }
                    break;
                }
                default:
                    break;
            }

        } else {
            // Current item is unfocused

            lpMenuItem item = menu->items[i];

            if (item->fontLo != NULL)
            {
                Font_PrintF(item->fontLo,
                            item->centerOffLo + item->x1 + menu->position.x,
                            item->y1 + menu->position.y,
                            "%s",
                            Front_Util_ReplaceTextSpaces(item->banner));
            }

            switch (item->itemType)
            {
                case MenuItem_Type_Cycle:
                {
                    // TODO: Implement Front_Menu_Update
                    break;
                }
                case MenuItem_Type_TextInput:
                {
                    // TODO: Implement Front_Menu_Update
                    break;
                }
                case MenuItem_Type_Slider:
                {
                    // TODO: Implement Front_Menu_Update
                    break;
                }
                case MenuItem_Type_RealSlider:
                {
                    // TODO: Implement Front_Menu_Update
                    break;
                }
                case MenuItem_Type_Select:
                {
                    lpMenuItem_SelectData select = item->itemData.select;

                    for (S32 i = 0; i < select->scrollCount; i++)
                    {
                        S32 currIndex = select->scrollStart + i;
                        if ((S32)currIndex >= (S32)select->selItemCount)
                            break;

                        lpMenuItem_SelectItem selectItem = &select->selItemList[currIndex];

                        MenuItem_SelectImageType imageType = MenuItem_SelectImage_Dark;
                        if (!(selectItem->flags & SELECTITEM_FLAG_ENABLED))
                            imageType = MenuItem_SelectImage_Locked;

                        S32 centerOff = 0;
                        if (menu->autoCenter)
                            centerOff = -((S32)select->widths[MenuItem_SelectImage_Light][currIndex] / 2);

                        S32 selItemX = select->x2 + item->x1 + menu->position.x + centerOff +
                            selectItem->frontEndX + frontGlobs.scrollOffset.x;
                        S32 selItemY = select->y2 + item->y1 + menu->position.y + (select->selItemHeight * i) +
                            selectItem->frontEndY + frontGlobs.scrollOffset.y;

                        Front_MenuItem_DrawSelectItem(selItemX, selItemY, item->fontLo, select, currIndex, imageType);

                        Front_MenuItem_DrawSaveImage(menu, currIndex, select, FALSE);
                    }

                    if (select->scrollStart < (S32)(select->selItemCount - select->scrollCount))
                    {
                        if (item->fontLo != NULL)
                        {
                            Font_PrintF(item->fontLo,
                                        select->xString1 + menu->position.x,
                                        select->yString1 + menu->position.y,
                                        select->string1);
                        }
                    }

                    if (select->scrollStart > 0)
                    {
                        if (item->fontLo != NULL)
                        {
                            Font_PrintF(item->fontLo,
                                        select->xString2 + menu->position.x,
                                        select->yString2 + menu->position.y,
                                        select->string2);
                        }
                    }

                    break;
                }
                default:
                    break;
            }
        }
    }

    Front_MenuItem_DrawSelectTextWindow(&menu);

    for (U32 i = 0; i < menu->itemCount; i++)
    {
        lpMenuItem item = menu->items[i];
        if (item->isImageItem)
        {
            if (menu->itemFocus == i)
            {
                if (item->imageHi != NULL)
                {
                    Point2F pos;
                    pos.x = (F32)(item->x1 + menu->position.x);
                    pos.y = (F32)(item->y1 + menu->position.y);
                    Image_Display(item->imageHi, &pos);
                }
                ToolTip_AddFlag4(item->toolTipType);
            } else if (item->imageLo != NULL) {
                Point2F pos;
                pos.x = (F32)(item->x1 + menu->position.x);
                pos.y = (F32)(item->y1 + menu->position.y);
                Image_Display(item->imageLo, &pos);
            }
        }
    }

#ifndef LEGORR_FORCE_SHOW_VERSION
    if (((mainGlobs.flags & MAIN_FLAG_SHOWVERSION) != MAIN_FLAG_NONE) &&
        frontGlobs.versionFont != NULL &&
        frontGlobs.versionString != NULL &&
        mainGlobs.programmerLevel == PROGRAMMER_OFF)
#endif
    {
        Font_PrintF(frontGlobs.versionFont, 545, 450, frontGlobs.versionString);
    }

    Front_LevelSelect_LevelNamePrintF(NULL, 0, 0, NULL);

    if (menu == frontGlobs.saveMenuSet->menus[0] && g_saveMenuOverwriteShowing)
    {
        char buff[2048];
        sprintf(buff, frontGlobs.langOverwriteMessage);
        g_saveMenuOverwriteResult = Front_SaveMenu_ConfirmMessage_FUN_004354f0(frontGlobs.langOverwriteTitle, buff, frontGlobs.langOverwriteOK, frontGlobs.langOverwriteCancel);
        if (g_saveMenuOverwriteResult == 0)
        {
            frontGlobs.saveMenuKeepOpen = TRUE;
            menu->closed = TRUE;
            g_saveMenuOverwriteShowing = FALSE;
            g_saveMenuOutputSelectedIndex = -1;
        }
        else if (g_saveMenuOverwriteResult == 1)
        {
            frontGlobs.saveMenuKeepOpen = FALSE;
            menu->closed = TRUE;
            g_saveMenuOverwriteShowing = FALSE;
        }
    }

    if (frontGlobs.rockWipeAnim != NULL && (frontGlobs.rockWipeFlags & ROCKWIPE_FLAG_ANIMATING))
    {
        Container_Hide(frontGlobs.rockWipeLight, FALSE);
        Viewport_Render(view, legoGlobs.rootCont, 0.0f);
        Container_Hide(frontGlobs.rockWipeLight, TRUE);

        Container_SetPosition(camera, NULL, origCamPos.x, origCamPos.y, origCamPos.z);
        Container_SetOrientation(camera, NULL, origCamDir.x, origCamDir.y, origCamDir.z, origCamUp.x, origCamUp.y,
                                 origCamUp.z);
        Container_SetParent(frontGlobs.rockWipeAnim, NULL);

        F32 frames = Container_GetAnimationFrames(frontGlobs.rockWipeAnim);
        F32 time = Main_GetTime();

        F32 unnamed = ((F32)time - frontGlobs.rockWipeSFXStartTime) * 0.001f * 25.0f;
        F32 local_850 = unnamed + unnamed;
        if (3.0f < local_850)
            local_850 = 3.0f;

        if (frontGlobs.rockWipeSFXTimer <= frames * 0.5f && (frames * 0.5f <= local_850 + frontGlobs.rockWipeSFXTimer))
        {
            if (menuTransition != NULL)
                *menuTransition = TRUE;

            g_saveMenuOverlayState = -1;
            g_saveMenuSelectedIndex = -1;
            g_saveMenu_UnkNextMenu = NULL;
            g_saveMenuOverlayPlaying = 0;

            if (frontGlobs.overlayImageOrFlic != NULL)
            {
                frontGlobs.overlayStartTime = 0;
                frontGlobs.overlayCurrTime = 0;
            }
        }

        if (frames < frontGlobs.rockWipeSFXTimer)
        {
            Front_RockWipe_Stop();
        }

        frontGlobs.rockWipeSFXTimer += local_850;
        frontGlobs.rockWipeSFXStartTime = time;
    }
    Pointer_DrawPointer(inputGlobs.msx, inputGlobs.msy);
    Pointer_SetCurrent_IfTimerFinished(currPointer);
    return currMenu;
}

B32 Front_IsTriggerAppQuit()
{
    return frontGlobs.menuVars[MENUVAR_TRIGGER_QUITAPP];
}

lpMenu Front_Menu_UpdateMenuItemsInput(F32 elapsed, lpMenu menu)
{
    B32 isHandled = FALSE;

    lpMenuItem menuItem = NULL; // Focused item
    if (menu->itemFocus >= 0)
        menuItem = menu->items[menu->itemFocus];

    if (frontGlobs.rockWipeFlags & ROCKWIPE_FLAG_NOINPUT)
        return menu;

    B32 isPressed = TRUE;
    if (menuItem == NULL && menu->itemFocus >= 0)
        menu->closed = TRUE;

    S32 findIndex = -1;
    if (Front_Menu_FindItemUnderMouse(menu, &findIndex))
    {
        // New focused item
        menu->itemFocus = findIndex;
        menuItem = menu->items[findIndex];
    }

    if (Front_MenuItem_CheckNotInTutoAnyTutorialFlags(menuItem))
        return menu;

    if (!Front_GetMousePressedState() || !Front_Menu_FindItemUnderMouse(menu, &findIndex))
        isPressed = FALSE;

    if (menuItem == NULL && menu->itemFocus < 0)
        return menu;

    if (isPressed || Input_IsKeyPressed(KEY_CURSORRIGHT))
    {
        SFX_Random_PlaySoundNormal(SFX_Okay, FALSE);

        switch(menuItem->itemType)
        {
            case MenuItem_Type_Cycle:
            {
                // TODO: Implement Front_Menu_UpdateMenuItemsInput
                break;
            }
            case MenuItem_Type_TextInput:
            {
                // TODO: Implement Front_Menu_UpdateMenuItemsInput
                break;
            }
            case MenuItem_Type_Slider:
            {
                // TODO: Implement Front_Menu_UpdateMenuItemsInput
                break;
            }
            case MenuItem_Type_RealSlider:
            {
                // TODO: Implement Front_Menu_UpdateMenuItemsInput
                break;
            }
            case MenuItem_Type_Select:
            {
                lpMenuItem_SelectData select = menuItem->itemData.select;

                S32 selIndex = Front_MenuItem_Select_TestStringCollision(menu, menuItem, select);

                if (isPressed)
                {
                    if (selIndex == -3)
                    {
                        // string2 is selected
                        if (select->scrollStart > 0)
                        {
                            select->scrollStart--;
                        }
                    }
                    else if (selIndex == -2)
                    {
                        // string1 is selected
                        if (select->scrollStart < (S32)(select->selItemCount - select->scrollCount))
                        {
                            select->scrollStart++;
                        }
                    }
                    else if (selIndex != -1)
                    {
                        // selectItem is selected

                        *select->valuePtr = selIndex;
                        if (select->callback != NULL)
                        {
                            select->callback(elapsed, selIndex);
                        }

                        // Are we in the load/save menu?
                        if (menu == frontGlobs.mainMenuSet->menus[3] ||
                            menu == frontGlobs.mainMenuSet->menus[0])
                        {
                            g_saveMenuSelectedIndex = selIndex;
                            g_saveMenuOutputSelectedIndex = selIndex;
                        }

                        if (select->nextMenu == NULL)
                            menu->closed = TRUE;
                        else
                        {
                            // Goto next menu
                            return select->nextMenu;
                        }
                    }
                }

                break;
            }
        }
    }

    if (Input_IsKeyPressed(KEY_CURSORLEFT))
    {
        // TODO: Implement Front_Menu_UpdateMenuItemsInput
    }

    if (isPressed && !isHandled)
    {
        MenuItem_Type type = menuItem->itemType;
        if (type == MenuItem_Type_Cycle)
        {
            // TODO: Implement Front_Menu_UpdateMenuItemsInput
        } else if (type == MenuItem_Type_Trigger)
        {
            lpMenuItem_TriggerData triggerData = menuItem->itemData.trigger;
            if (triggerData->callback != NULL)
                triggerData->callback();
            if (triggerData->end)
            {
                menu->closed = TRUE;
                *triggerData->valuePtr = TRUE;
            }
        } else if (type == MenuItem_Type_Next)
        {
            return menuItem->itemData.next;
        }
    }

    if (menuItem->itemType == MenuItem_Type_TextInput)
    {
        // TODO: Implement Front_Menu_UpdateMenuItemsInput
    }

    return menu;
}

void Front_Menu_UpdateMousePosition(lpMenu menu)
{
    if (menu->menuImage == NULL || !(menu->flags & MENU_FLAG_CANSCROLL))
    {
        frontGlobs.scrollOffset.x = 0;
        frontGlobs.scrollOffset.y = 0;
        return;
    }

    F32 mouseY = (F32)inputGlobs.msy;

    // If mouseY is between 200 and 280, don't scroll
    if (mouseY >= 200.0f && mouseY <= 280.0f)
        return;

    S32 imageHeight = Image_GetHeight(menu->menuImage);
    if (mouseY < 480.0f && !inputGlobs.mslb && !inputGlobs.msrb)
    {
        F32 fVar3 = (F32)inputGlobs.msy * (100.0f / 480.0f) - 50.0f;
        fVar3 *= fVar3 * 0.02f; // squared * 0.02f
        if (mouseY < (480.0f / 2.0f))
            fVar3 = -fVar3;

        fVar3 *= -(STANDARD_FRAMERATE / 100.0f);
        frontGlobs.scrollOffset.y += (S32)fVar3;
    }

    if (frontGlobs.scrollOffset.y > 0)
        frontGlobs.scrollOffset.y = 0;

    if (frontGlobs.scrollOffset.y < 480 - imageHeight)
        frontGlobs.scrollOffset.y = 480 - imageHeight;
}

void Front_Menu_UpdateOverlays(lpMenu menu)
{

    if (frontGlobs.overlayImageOrFlic == NULL)
    {
        lpMenuOverlay overlay = &menu->overlays[0];
        if (overlay == NULL)
            return;

        if (menu->playRandom == BOOL3_TRUE)
        {
            U32 overlayCount = 0;
            if (Front_Menu_ShouldRandomPlay())
                overlayCount = Front_Menu_GetOverlayCount(menu);

            if (overlayCount != 0)
            {
                U32 i = rand();
                for (i = i % overlayCount; i != 0; i--)
                    overlay = overlay->previous;

                if (Flic_Setup(overlay->filename, &frontGlobs.overlayImageOrFlic, FLICDISK))
                {
                    SFX_AddToQueue(overlay->sfxType, SoundMode_Once);
                    frontGlobs.overlayPosition.y = overlay->position.x;
                    frontGlobs.overlayPosition.x = overlay->position.y;
                    frontGlobs.overlayStartTime = Main_GetTime();
                    frontGlobs.overlayCurrTime = frontGlobs.overlayStartTime;
                    return;
                }
            }
        } else
        {
            if (g_saveMenuOverlayState >= 0)
            {
                S32 index = 4 - g_saveMenuOverlayState;
                for (S32 i = 0; i < index; i++)
                    overlay = overlay->previous;

                if (Flic_Setup(overlay->filename, &frontGlobs.overlayImageOrFlic, FLICDISK))
                {
                    SFX_AddToQueue(overlay->sfxType, SoundMode_Once);
                    frontGlobs.overlayPosition.y = overlay->position.x;
                    frontGlobs.overlayPosition.x = overlay->position.y;
                    frontGlobs.overlayStartTime = Main_GetTime();
                    frontGlobs.overlayCurrTime = frontGlobs.overlayStartTime;
                }

                g_saveMenuOverlayPlaying = TRUE;
                g_saveMenuOverlayState = -1;
                g_saveMenuSelectedIndex = -1;
            }
            else if (g_saveMenuSelectedIndex >= 0)
            {
                g_saveMenuOverlayState = g_saveMenuSelectedIndex;
                g_saveMenu_UnkNextMenu = NULL;
            }
        }

        return;
    }

    U32 width = Flic_GetWidth(frontGlobs.overlayImageOrFlic);
    U32 height = Flic_GetHeight(frontGlobs.overlayImageOrFlic);

    Area2F overlayRect;
    overlayRect.x = frontGlobs.overlayPosition.y;
    overlayRect.y = frontGlobs.overlayPosition.x;
    overlayRect.width = width;
    overlayRect.height = height;

    U32 time = Main_GetTime();
    U32 oldTime = (U32)((F32)(frontGlobs.overlayCurrTime - frontGlobs.overlayStartTime) / 1000.0f * STANDARD_FRAMERATE);
    U32 newTime = (U32)((F32)(time - frontGlobs.overlayStartTime) / 1000.0f * STANDARD_FRAMERATE);

    if (g_saveMenuOverlayPlaying)
    {
        if (frontGlobs.overlayImageOrFlic->fsHeader.frames <= frontGlobs.overlayImageOrFlic->currentframe)
        {
            newTime = oldTime;
            g_saveMenuOverlayState = -2;
        }
    }

    frontGlobs.overlayCurrTime = time;

    if (!Flic_Animate(frontGlobs.overlayImageOrFlic, &overlayRect, (newTime != oldTime), FALSE))
    {
        Flic_Close(frontGlobs.overlayImageOrFlic);
        Mem_Free(frontGlobs.overlayImageOrFlic);
        Sound3D_Stream_Stop(FALSE);
        frontGlobs.overlayImageOrFlic = NULL;
        frontGlobs.overlayStartTime = 0;
        frontGlobs.overlayCurrTime = 0;
        Sound3D_Stream_Stop(FALSE);
    }

    if (g_saveMenuOverlayPlaying && inputGlobs.mslb)
    {
        g_saveMenuOverlayState = -2;
        g_saveMenuOverlayPlaying = FALSE;
    }
}

B32 Front_Menu_ShouldRandomPlay()
{
    return rand() % 400 == 0;
}

U32 Front_Menu_GetOverlayCount(lpMenu menu)
{
    U32 count = 0;
    for (lpMenuOverlay overlay = &menu->overlays[0]; overlay != NULL; overlay = overlay->previous)
        count++;

    return count;
}

void Front_Menu_DrawMenuImage(lpMenu menu, B32 light)
{
    lpImage image = light ? menu->menuImage : menu->menuImageDark;

    if (image != NULL)
    {
        if (menu->flags & MENU_FLAG_HASPOSITION)
        {
            Image_Display(image, &menu->currPosition);
            return;
        }

        Point2F destPos;
        Area2F srcArea;

        srcArea.x = -(F32)frontGlobs.scrollOffset.x;
        srcArea.y = -(F32)frontGlobs.scrollOffset.y;

        // TODO: Hardcoded Screen Resolution
        srcArea.width = 640.0f;
        srcArea.height = 480.0f;

        destPos.x = 0.0f;
        destPos.y = 0.0f;

        Image_DisplayScaled(image, &srcArea, &destPos, NULL);
    }
}

void Front_Menu_DrawLoadSaveText(lpMenu* pMenu, lpMenu* currMenu, lpMenu* nextMenu)
{
    lpMenu menu = *pMenu;

    // Is this the Load Game or Save Game menu?
    if ((menu == frontGlobs.mainMenuSet->menus[3]) || (menu == frontGlobs.mainMenuSet->menus[0]))
    {
        // Is this the Load Game menu?
        if ((menu == frontGlobs.saveMenuSet->menus[0]) && g_saveMenuSelectedIndex >= 0)
        {
            // TODO: Implement Front_Menu_DrawLoadSaveText
        }

        TextWindow_Clear(frontGlobs.saveTextWnd->textWindow);
        TextWindow_PrintF(frontGlobs.saveTextWnd->textWindow, "\n");

        if (g_saveMenuOverlayState == -2 || g_saveMenuOverlayState == -1)
        {
            if (g_saveMenuOverlayState == -2)
            {
                if (g_saveMenu_UnkNextMenu != NULL)
                {
                    *currMenu = g_saveMenu_UnkNextMenu;
                    g_saveMenu_UnkNextMenu = NULL;
                }

                g_frontSelectHoverIndex = -1;
                g_saveMenuSelectingIndex = -1;
            }

            *nextMenu = NULL;
            g_saveMenuSelectedIndex = -1;

            const char* format = frontGlobs.saveTextWnd->LoadText;
            if (menu != frontGlobs.mainMenuSet->menus[3])
                format = frontGlobs.saveTextWnd->SaveText;

            TextWindow_PrintF(frontGlobs.saveTextWnd->textWindow, format);
        }
        else
        {
            g_saveMenu_UnkNextMenu = *currMenu;
            *currMenu = *nextMenu;
            g_saveMenuSelectingIndex = g_saveMenuOverlayState;
        }

        // TODO: Implement Front_Menu_DrawLoadSaveText
    }
    // TODO: Implement Front_Menu_DrawLoadSaveText
}

B32 Front_Menu_AddMenuItem(lpMenu menu, lpMenuItem menuItem)
{
    lpMenuItem* newMenuItems;
    S32 capacity;

    if (menuItem == NULL || menu == NULL)
        return FALSE;

    capacity = menu->itemCapacity;
    if (menu->itemCount == capacity)
    {
        if (menu->items == NULL)
        {
            newMenuItems = Mem_Alloc(sizeof(lpMenuItem) * (capacity + 15));
        } else {
            newMenuItems = Mem_ReAlloc(menu->items, sizeof(lpMenuItem) * (capacity + 15));
        }

        if (newMenuItems == NULL)
            return FALSE;

        menu->items = newMenuItems;
        menu->itemCapacity = menu->itemCapacity + 15;
    }

    menu->items[menu->itemCount] = menuItem;
    menu->itemCount++;
    return TRUE;
}

B32 Front_Menu_FindItemUnderMouse(lpMenu menu, S32* itemIndex)
{
    S32 index = 0;
    S32 rcX = 0;
    S32 rcY = 0;
    S32 rcWidth = 0;
    S32 rcHeight = 0;
    B32 levelUnderMouse = FALSE;
    while (TRUE)
    {
        if (!Front_Menu_GetItemBounds(menu, index, &rcX, &rcY, &rcWidth, &rcHeight))
        {
            levelUnderMouse = Front_Menu_IsLevelItemUnderMouse(menu, index);
            if (!levelUnderMouse)
                return FALSE;
        }

        if (levelUnderMouse)
            break;

        if (Front_Maths_IsPointInsideRect(inputGlobs.msx, inputGlobs.msy, rcX, rcY, rcWidth, rcHeight))
            break;

        index++;
    }

    if (itemIndex != NULL)
        *itemIndex = index;

    return TRUE;
}

B32 Front_Menu_GetItemBounds(lpMenu menu, S32 itemIndex, S32* rcX, S32* rcY, S32* rcWidth, S32* rcHeight)
{
    if (menu->itemCount <= itemIndex)
        return FALSE;

    lpMenuItem item = menu->items[itemIndex];
    if (item->itemType == MenuItem_Type_Slider)
    {
        // TODO: Implement Front_Menu_GetItemBounds
    }

    if (item->itemType == MenuItem_Type_Select)
        return FALSE;

    *rcX = item->centerOffHi + menu->position.x + item->x1;
    *rcY = menu->items[itemIndex]->y1 + menu->position.y;

    if (item->isImageItem)
    {
        if (menu->itemFocus == itemIndex)
            *rcWidth = item->imageHi->width;
        else
            *rcWidth = item->imageLo->width;

        if (menu->itemFocus == itemIndex)
        {
            *rcHeight = item->imageHi->height;
            return TRUE;
        }

        *rcHeight = item->imageLo->height;
        return TRUE;
    }

    lpFont font;
    if (menu->itemFocus == itemIndex)
        font = item->fontHi;
    else
        font = item->fontLo;

    *rcWidth = Font_GetStringWidth(font, item->banner);
    if (menu->itemFocus == itemIndex)
    {
        *rcHeight = Font_GetHeight(menu->items[itemIndex]->fontHi);
        return TRUE;
    }

    *rcHeight = Font_GetHeight(menu->items[itemIndex]->fontLo);

    return TRUE;
}

B32 Front_Menu_IsLevelItemUnderMouse(lpMenu menu, S32 itemIndex)
{
    lpMenuItem menuItem;
    S32 index;

    if (itemIndex < menu->itemCount && (menuItem = menu->items[itemIndex], menuItem->itemType == MenuItem_Type_Select))
    {
        index = Front_MenuItem_Select_TestStringCollision(menu, menuItem, menuItem->itemData.select);
        return (U32)(index != -1);
    }

    return FALSE;
}

void Front_MenuItem_DrawSelectTextWindow(lpMenu* menu)
{
    Size2F destSize;
    Point2F destPos;
    lpMenuTextWindow menuWnd;

    menuWnd = frontGlobs.saveLevelWnd;
    // For the missions and training missions menus
    if (*menu != frontGlobs.mainMenuSet->menus[1] && *menu != frontGlobs.mainMenuSet->menus[2])
        return;

    if (frontGlobs.saveLevelWnd->PanelImage != NULL)
    {
        destPos.x = frontGlobs.saveLevelWnd->PanelArea.x;
        destPos.y = frontGlobs.saveLevelWnd->PanelArea.y;
        destSize.width = frontGlobs.saveLevelWnd->PanelArea.width;
        destSize.height = frontGlobs.saveLevelWnd->PanelArea.height;
        Image_DisplayScaled(frontGlobs.saveLevelWnd->PanelImage, NULL, &destPos, &destSize);
    }

    if (menuWnd->textWindow != NULL)
    {
        TextWindow_Update(menuWnd->textWindow, 0, 1.0f, NULL);
        TextWindow_Clear(menuWnd->textWindow);
        TextWindow_PrintF(menuWnd->textWindow, "\n");

        // Missions menu
        if (*menu == frontGlobs.mainMenuSet->menus[1] && menuWnd->LevelText != NULL)
        {
            TextWindow_PrintF(menuWnd->textWindow, menuWnd->LevelText);
            TextWindow_PrintF(menuWnd->textWindow, "\n");
        }

        // Training missions menu
        if (*menu == frontGlobs.mainMenuSet->menus[2] && menuWnd->TutorialText != NULL)
        {
            TextWindow_PrintF(menuWnd->textWindow, menuWnd->TutorialText);
            TextWindow_PrintF(menuWnd->textWindow, "\n");
        }

        g_levelSelectPrinting = TRUE;
    }
}

B32 Front_MenuItem_CheckNotInTutoAnyTutorialFlags(lpMenuItem menuItem)
{
    // TODO: Implement Front_MenuItem_CheckNotInTutoAnyTutorialFlags
    return FALSE;
}

MenuItem_Type Front_MenuItem_ParseTypeString(const char* itemTypeName)
{
    S32 cmp;

    cmp = _stricmp(itemTypeName, "cycle");
    if (cmp == 0)
        return MenuItem_Type_Cycle;

    cmp = _stricmp(itemTypeName, "trigger");
    if (cmp == 0)
        return MenuItem_Type_Trigger;

    cmp = _stricmp(itemTypeName, "textinput");
    if (cmp == 0)
        return MenuItem_Type_TextInput;

    cmp = _stricmp(itemTypeName, "slider");
    if (cmp == 0)
        return MenuItem_Type_Slider;

    cmp = _stricmp(itemTypeName, "realslider");
    if (cmp == 0)
        return MenuItem_Type_RealSlider;

    cmp = _stricmp(itemTypeName, "next");
    if (cmp == 0)
        return MenuItem_Type_Next;

    return MenuItem_Type_Invalid;
}

lpMenuItem Front_MenuItem_CreateBannerItem(const char* banner, lpFont loFont, lpFont hiFont, S32 x1, S32 y1, MenuItem_Type itemType, B32 centered, void* itemData, B32 notInTutorial)
{
    if (banner == NULL)
        return NULL;

    lpMenuItem menuItem = Mem_Alloc(sizeof(MenuItem));
    if (menuItem == NULL)
        return NULL;

    memset(menuItem, 0, sizeof(MenuItem));

    menuItem->length = strlen(banner);
    menuItem->banner = Front_Util_StrCpy(banner);
    menuItem->fontLo = loFont;
    menuItem->fontHi = hiFont;

    menuItem->x1 = x1;
    menuItem->y1 = y1;

    menuItem->itemData.data = itemData; // Typeless void* assignment for union
    menuItem->itemType = itemType;

    menuItem->notInTuto = notInTutorial;

    if (centered && loFont != NULL && hiFont != NULL)
    {
        menuItem->centerOffLo = -((S32)Font_GetStringWidth(loFont, banner) / 2);
        menuItem->centerOffHi = -((S32)Font_GetStringWidth(hiFont, banner) / 2);
    } else {
        menuItem->centerOffLo = 0;
        menuItem->centerOffHi = 0;
    }

    return menuItem;
}

lpMenuItem Front_MenuItem_CreateImageItem(const char* banner, lpFont loFont, lpFont hiFont, const char* loImageName, const char* hiImageName, S32 x1, S32 y1, MenuItem_Type itemType, B32 centered, const char* toolTipName, void* itemData)
{
    if (banner == NULL)
        return NULL;

    lpMenuItem menuItem = Mem_Alloc(sizeof(MenuItem));
    if (menuItem == NULL)
        return NULL;

    memset(menuItem, 0, sizeof(MenuItem));

    menuItem->length = strlen(banner);
    menuItem->banner = Front_Util_StrCpy(banner);
    menuItem->fontHi = hiFont;
    menuItem->fontLo = loFont;

    menuItem->imageLo = Image_LoadBMP(loImageName);
    if (menuItem->imageLo == NULL)
    {
#ifdef LEGORR_FIX_MEMORY_LEAKS
        Mem_Free(menuItem);
#endif
        return NULL;
    }

    menuItem->imageHi = Image_LoadBMP(hiImageName);
    if (menuItem->imageHi != NULL)
        menuItem->isImageItem = TRUE;

    menuItem->itemData.data = itemData; // Typeless void* assignment for union
    menuItem->itemType = itemType;
    menuItem->x1 = x1;
    menuItem->y1 = y1;

    if (!ToolTip_GetType(toolTipName, &menuItem->toolTipType))
        menuItem->toolTipType = ToolTip_Null;

    if (centered && menuItem->imageLo != NULL && menuItem->imageHi != NULL)
    {
        menuItem->centerOffLo = -((S32)Image_GetWidth(menuItem->imageLo) / 2);
        menuItem->centerOffHi = -((S32)Image_GetWidth(menuItem->imageHi) / 2);
    } else {
        menuItem->centerOffLo = 0;
        menuItem->centerOffHi = 0;
    }

    return menuItem;
}

lpMenuItem_TriggerData Front_MenuItem_CreateTrigger(B32* valuePtr, B32 end, MenuItem_TriggerCallback callback)
{
    lpMenuItem_TriggerData triggerData;

    triggerData = Mem_Alloc(sizeof(MenuItem_TriggerData));
    if (triggerData == NULL)
        return NULL;

    triggerData->valuePtr = valuePtr;
    triggerData->end = end;
    triggerData->callback = callback;

    return triggerData;
}

lpMenuItem_SelectData Front_MenuItem_CreateSelect(S32* valuePtr, const char* string1, const char* string2, S32 x2, S32 y2, S32 selItemHeight, S32 scrollCount, S32 xString1, S32 yString1, S32 xString2, S32 yString2, S32 field50, MenuItem_SelectCallback callback, lpMenu nextMenu)
{
    lpMenuItem_SelectData selectData = Mem_Alloc(sizeof(MenuItem_SelectData));
    memset(selectData, 0, sizeof(MenuItem_SelectData));

    selectData->string1 = Front_Util_StrCpy(string1);
    selectData->string2 = Front_Util_StrCpy(string2);

    selectData->valuePtr = valuePtr;

    selectData->x2 = x2;
    selectData->y2 = y2;

    selectData->xString1 = xString1;
    selectData->yString1 = yString1;
    selectData->xString2 = xString2;
    selectData->yString2 = yString2;

    selectData->selItemHeight = selItemHeight;
    selectData->scrollCount = scrollCount;
    selectData->scrollStart = 0;

    selectData->callback = callback;
    selectData->field_50 = field50;
    selectData->nextMenu = nextMenu;

    return selectData;
}

// To create a banner item, pass a string to bannerOrBMPName that does NOT contain
// ".bmp" (case-insensitive), font must also be non-NULL for banner items.
void Front_MenuItem_AddSelectItem(lpMenuItem_SelectData selectData, const char* bannerOrBMPName, B32 enabled, lpFont hiFont, S32 frontEndX, S32 frontEndY, B32 frontEndOpen)
{
    // TODO: Original malloc-null-checks here are faulty. LRR checks if newSelItems is non-null.
    //       But it doesn't check newWidths/newHeights... and worse, it uses realloc, so failure can be fatal.

    lpMenuItem_SelectItem newSelItems;
    U32* newWidths[MenuItem_SelectImage_Count];
    U32* newHeights[MenuItem_SelectImage_Count];

    if (selectData->selItemList == NULL)
    {
        newSelItems = (lpMenuItem_SelectItem)Mem_Alloc((selectData->selItemCount + 1) * sizeof(MenuItem_SelectItem));
        for (U32 i = 0; i < MenuItem_SelectImage_Count; i++)
        {
            newWidths[i] = (U32*)Mem_Alloc((selectData->selItemCount + 1) * sizeof(U32));
            newHeights[i] = (U32*)Mem_Alloc((selectData->selItemCount + 1) * sizeof(U32));
        }
    } else {
        newSelItems = (lpMenuItem_SelectItem)Mem_ReAlloc(selectData->selItemList, (selectData->selItemCount + 1) * sizeof(MenuItem_SelectItem));
        for (U32 i = 0; i < MenuItem_SelectImage_Count; i++)
        {
            newWidths[i] = (U32*)Mem_ReAlloc(selectData->widths[i], (selectData->selItemCount + 1) * sizeof(U32));
            newHeights[i] = (U32*)Mem_ReAlloc(selectData->heights[i], (selectData->selItemCount + 1) * sizeof(U32));
        }
    }

    if (newSelItems == NULL)
        return;

    selectData->selItemList = newSelItems;
    for (U32 i = 0; i < MenuItem_SelectImage_Count; i++)
    {
        selectData->widths[i] = newWidths[i];
        selectData->heights[i] = newHeights[i];
    }

    lpMenuItem_SelectItem selItem = &selectData->selItemList[selectData->selItemCount];

    // NOTE: images are NOT nulled out when item is treated as banner.
    //memset(&selectData->selItemList[selectData->selItemCount], 0, sizeof(MenuItem_SelectItem));

    selItem->banner = NULL;
    selItem->flags = (enabled ? SELECTITEM_FLAG_ENABLED : SELECTITEM_FLAG_NONE);
    selItem->frontEndX = frontEndX;
    selItem->frontEndY = frontEndY;
    selItem->frontEndOpen = frontEndOpen;

    // Only the hackiest of the hackiest solutions...
    // If this parameter contains ".bmp" (case-insensitive), then it's an image filename.
    // Otherwise it's a banner string.
    if (bannerOrBMPName != NULL && Util_StrIStr(bannerOrBMPName, ".bmp"))
    {
        selItem->flags |= SELECTITEM_FLAG_HASIMAGE;

        char* stringParts[3];
        U32 numParts = Util_Tokenize(bannerOrBMPName, stringParts, ",");
        Error_Fatal(numParts != 3, "Must have exactly 3 comma-separated bmp images for select item");

        for (U32 i = 0; i < MenuItem_SelectImage_Count; i++)
        {
            lpImage image = Front_Cache_LoadImage(stringParts[i]);

            // This null-check here is pointless, we'll still crash on GetWidth, GetHeight
            if (image != NULL)
                Image_SetPenZeroTrans(image);

            selItem->images[i] = image;

            selectData->widths[i][selectData->selItemCount] = Image_GetWidth(image);
            selectData->heights[i][selectData->selItemCount] = Image_GetHeight(image);
        }
    } else {
        selItem->flags |= SELECTITEM_FLAG_HASBANNER;

        selItem->banner = Front_Util_StrCpy(bannerOrBMPName);

        selectData->widths[0][selectData->selItemCount] = Font_GetStringWidth(hiFont, bannerOrBMPName);
        selectData->heights[0][selectData->selItemCount] = Font_GetHeight(hiFont);
        for (U32 i = 1; i < MenuItem_SelectImage_Count; i++)
        {
            selectData->widths[i][selectData->selItemCount] = selectData->widths[i - 1][selectData->selItemCount];
            selectData->heights[i][selectData->selItemCount] = selectData->heights[i - 1][selectData->selItemCount];
        }
    }

    selectData->selItemCount++;
}

S32 Front_MenuItem_Select_TestStringCollision(lpMenu menu, lpMenuItem menuItem, lpMenuItem_SelectData selectData)
{
    U32 rcWidth = 0;
    U32 rcHeight = 0;

    if (menuItem->fontHi != NULL)
    {
        rcWidth = Font_GetStringWidth(menuItem->fontHi, selectData->string1);
        rcHeight = Font_GetHeight(menuItem->fontHi);
    }

    if (Front_Maths_IsPointInsideRectCentered(inputGlobs.msx, inputGlobs.msy,
                                              selectData->xString1 + menu->position.x,
                                              selectData->yString1 + menu->position.y,
                                              rcWidth, rcHeight, FALSE))
    {
        return -2; // Over string1.
    }

    if (menuItem->fontHi == NULL)
        rcWidth = 0;
    else
        rcWidth = Font_GetStringWidth(menuItem->fontHi, selectData->string2);

    if (Front_Maths_IsPointInsideRectCentered(inputGlobs.msx, inputGlobs.msy,
                                              selectData->xString2 + menu->position.x,
                                              selectData->yString2 + menu->position.y,
                                              rcWidth, rcHeight, FALSE))
    {
        return -3; // Over string2.
    }

    for (S32 i = 0; i < selectData->scrollCount; i++)
    {
        S32 currIndex = selectData->scrollStart + i;
        if ((S32)currIndex >= (S32)selectData->selItemCount)
            break;

        lpMenuItem_SelectItem selectItem = &selectData->selItemList[currIndex];

        if (selectItem->flags & SELECTITEM_FLAG_ENABLED)
        {
            S32 selItemX = selectData->x2 + menuItem->x1 + menu->position.x +
                selectItem->frontEndX + frontGlobs.scrollOffset.x;
            S32 selItemY = selectData->y2 + menuItem->y1 + menu->position.y +
                (selectData->selItemHeight * i) + selectItem->frontEndY + frontGlobs.scrollOffset.y;
            S32 selItemWidth = (S32)selectData->widths[MenuItem_SelectImage_Light][currIndex];
            S32 selItemHeight = (S32)selectData->heights[MenuItem_SelectImage_Light][currIndex];

            if (Front_Maths_IsPointInsideRectCentered(inputGlobs.msx, inputGlobs.msy,
                                                      selItemX, selItemY,
                                                      selItemWidth, selItemHeight, menu->autoCenter))
            {
                return currIndex; // Over selectItem.
            }
        }
    }

    return -1; // Over nothing.
}

void Front_MenuItem_DrawSaveImage(lpMenu menu, S32 selIndex, lpMenuItem_SelectData selectData, B32 bigSize)
{
    // TODO: Implement Front_MenuItem_DrawSaveImage
}

void Front_MenuItem_DrawSelectItem(S32 x, S32 y, lpFont font, lpMenuItem_SelectData selectData, U32 selIndex, MenuItem_SelectImageType imageType)
{
    if (selectData->selItemList[selIndex].flags & SELECTITEM_FLAG_HASBANNER)
    {
        Font_PrintF(font, x, y, selectData->selItemList[selIndex].banner);
    } else {
        lpImage image = selectData->selItemList[selIndex].images[imageType];

        // TODO: Hardcoded screen resolution

        // NOTE: > 0 used here since we're adding width/height (0 would mean everything is off-screen).
        if (image &&
            ((x + Image_GetWidth(image)) > 0 && x < 640) &&
            ((y + Image_GetHeight(image)) > 0 && y < 480))
        {
            Point2F destPos;
            destPos.x = (F32)x;
            destPos.y = (F32)y;

            Image_Display(image, &destPos);
        }
    }
}

void Front_RockWipe_Play()
{
    if (frontGlobs.rockWipeAnim != NULL)
    {
        frontGlobs.rockWipeSFXTimer = 0.0f;
        frontGlobs.rockWipeFlags |= (ROCKWIPE_FLAG_ANIMATING | ROCKWIPE_FLAG_NOINPUT);
        frontGlobs.rockWipeSFXStartTime = Main_GetTime();
        SFX_AddToQueue(SFX_RockWipe, SoundMode_Once);
    }
}

void Front_RockWipe_Stop()
{
    frontGlobs.rockWipeFlags &= ~(ROCKWIPE_FLAG_ANIMATING | ROCKWIPE_FLAG_NOINPUT);
}

const char* Front_GetSelectedLevel()
{
    if (Front_IsMissionSelected())
    {
        S32 setIndex = Front_LevelLink_FindSetIndexOf(frontGlobs.startMissionLink, frontGlobs.menuVars[MENUVAR_SELECT_MISSION_INDEX]);
        if (setIndex != -1)
            return frontGlobs.missionLevels.idNames[setIndex];
    } else if (Front_IsTutorialSelected())
    {
        S32 setIndex = Front_LevelLink_FindSetIndexOf(frontGlobs.startTutorialLink, frontGlobs.menuVars[MENUVAR_SELECT_TUTORIAL_INDEX]);
        if (setIndex != -1)
            return frontGlobs.tutorialLevels.idNames[setIndex];
    }
    return NULL;
}

B32 Front_IsMissionSelected()
{
    return frontGlobs.menuVars[MENUVAR_SELECT_MISSION_INDEX] != -1;
}

B32 Front_IsTutorialSelected()
{
    return frontGlobs.menuVars[MENUVAR_SELECT_TUTORIAL_INDEX] != -1;
}

void Front_LoadOptionParameters(B32 loadOptions, B32 resetFront)
{
    // TODO: Implement Front_LoadOptionParameters
}

void Front_LoadMenuTextWindow(lpConfig config, const char* configPath, lpMenuTextWindow menuWnd)
{
    const char* str;

    str = Config_GetTempStringValue(config, Config_BuildStringID(legoGlobs.gameName, configPath, "Load", 0));
    if (str != NULL)
        strcpy(menuWnd->LoadText, Front_Util_ReplaceTextSpaces(str));

    str = Config_GetTempStringValue(config, Config_BuildStringID(legoGlobs.gameName, configPath, "Save", 0));
    if (str != NULL)
        strcpy(menuWnd->SaveText, Front_Util_ReplaceTextSpaces(str));

    str = Config_GetTempStringValue(config, Config_BuildStringID(legoGlobs.gameName, configPath, "Slot", 0));
    if (str != NULL)
        strcpy(menuWnd->SlotText, Front_Util_ReplaceTextSpaces(str));

    str = Config_GetTempStringValue(config, Config_BuildStringID(legoGlobs.gameName, configPath, "SaveSel", 0));
    if (str != NULL)
        strcpy(menuWnd->SaveSelText, Front_Util_ReplaceTextSpaces(str));

    str = Config_GetTempStringValue(config, Config_BuildStringID(legoGlobs.gameName, configPath, "LoadSel", 0));
    if (str != NULL)
        strcpy(menuWnd->LoadSelText, Front_Util_ReplaceTextSpaces(str));

    str = Config_GetTempStringValue(config, Config_BuildStringID(legoGlobs.gameName, configPath, "Level", 0));
    if (str != NULL)
        strcpy(menuWnd->LevelText, Front_Util_ReplaceTextSpaces(str));

    str = Config_GetTempStringValue(config, Config_BuildStringID(legoGlobs.gameName, configPath, "Tutorial", 0));
    if (str != NULL)
        strcpy(menuWnd->TutorialText, Front_Util_ReplaceTextSpaces(str));

    char* value;

    value = Config_GetStringValue(config, Config_BuildStringID(legoGlobs.gameName, configPath, "Window", 0));
    if (value != NULL)
    {
        char* stringParts[5];
        U32 numParts = Util_Tokenize(value, stringParts, "|");
        if (numParts == 4)
        {
            Area2F windowArea;
            windowArea.x = (F32)atoi(stringParts[0]);
            windowArea.y = (F32)atoi(stringParts[1]);
            windowArea.width = (F32)atoi(stringParts[2]);
            windowArea.height = (F32)atoi(stringParts[3]);
            menuWnd->WindowArea = windowArea;

            menuWnd->textWindow = TextWindow_Create(legoGlobs.bmpFONT5_HI, &menuWnd->WindowArea, 512);
        }

#ifdef LEGORR_FIX_MEMORY_LEAKS
        Mem_Free(value);
#endif
    }

    value = Config_GetStringValue(config, Config_BuildStringID(legoGlobs.gameName, configPath, "Panel", 0));
    if (value != NULL)
    {
        char* stringParts[6];
        U32 numParts = Util_Tokenize(value, stringParts, "|");
        if (numParts == 5 && stringParts[0] != NULL)
        {
            menuWnd->PanelImage = Image_LoadBMP(stringParts[0]);
            if (menuWnd->PanelImage != NULL)
            {
                Area2F panelArea;
                panelArea.x = (F32)atoi(stringParts[1]);
                panelArea.y = (F32)atoi(stringParts[2]);
                panelArea.width = (F32)atoi(stringParts[3]);
                panelArea.height = (F32)atoi(stringParts[4]);

                menuWnd->PanelArea = panelArea;

                Image_SetupTrans(menuWnd->PanelImage, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f);
            }
        }

#ifdef LEGORR_FIX_MEMORY_LEAKS
        Mem_Free(value);
#endif
    }
}

lpMenuSet Front_LoadMenuSet(lpConfig config, const char* menuName, ...)
{
    S32 menuNumber;
    char submenuPathBuff[1024];
    char *stringParts[100];

    char menuPathBuff[1024];
    sprintf(menuPathBuff, "Menu::%s", menuName);

    S32 menuCount = Config_GetIntValue(legoGlobs.config, Config_BuildStringID(legoGlobs.gameName, menuPathBuff, "MenuCount", 0));
    lpMenuSet menuSet = Front_CreateMenuSet(menuCount);

    S32 menuIndex = 0;
    if (menuCount > 0) {
        do
        {
            menuNumber = menuIndex + 1;

            S32 local_dc8 = 0;
            sprintf(submenuPathBuff, "%s::Menu%i", menuPathBuff, menuNumber);

            const char* title = Config_GetStringValue(legoGlobs.config, Config_BuildStringID(legoGlobs.gameName, submenuPathBuff, "Title", 0));
            const char* fullName = Config_GetStringValue(legoGlobs.config, Config_BuildStringID(legoGlobs.gameName, submenuPathBuff, "FullName", 0));
            const char* position = Config_GetStringValue(legoGlobs.config, Config_BuildStringID(legoGlobs.gameName, submenuPathBuff, "Position", 0));
            Util_Tokenize(position, stringParts, ":");
            S32 x = atoi(stringParts[0]);
            S32 y = atoi(stringParts[1]);
            if (position)
                Mem_Free(position);

            const char* menuFontName = Config_GetTempStringValue(legoGlobs.config, Config_BuildStringID(legoGlobs.gameName, submenuPathBuff, "MenuFont", 0));
            lpFont menuFont = Front_Cache_LoadFont(menuFontName);

            B32 autoCenter = Config_GetBoolValue(legoGlobs.config, Config_BuildStringID(legoGlobs.gameName, submenuPathBuff, "AutoCenter", 0)) == BOOL3_TRUE;
            B32 displayTitle = Config_GetBoolValue(legoGlobs.config, Config_BuildStringID(legoGlobs.gameName, submenuPathBuff, "DisplayTitle", 0)) != BOOL3_FALSE;

            const char* anchored = Config_GetStringValue(legoGlobs.config, Config_BuildStringID(legoGlobs.gameName, submenuPathBuff, "Anchored", 0));

            if (autoCenter && title != NULL && *title != '\0')
                local_dc8 = -((S32)Font_GetStringWidth(menuFont, title) / 2);

            B32 canScroll = Config_GetBoolValue(legoGlobs.config, Config_BuildStringID(legoGlobs.gameName, submenuPathBuff, "CanScroll", 0)) == BOOL3_TRUE;

            const char* fullName2 = Front_Util_StringReplaceChar(fullName, '_', ' ');
            const char* title2 = Front_Util_StringReplaceChar(title, '_', ' ');

            menuSet->menus[menuIndex] = Front_Menu_CreateMenu(title2, fullName2, menuFont, x, y, autoCenter, displayTitle, local_dc8, canScroll, anchored);

            const char* menuImage = Config_GetTempStringValue(legoGlobs.config, Config_BuildStringID(legoGlobs.gameName, submenuPathBuff, "MenuImage", 0));
            Front_Menu_LoadMenuImage(menuSet->menus[menuIndex], menuImage, TRUE);

            const char* menuImageDark = Config_GetTempStringValue(legoGlobs.config, Config_BuildStringID(legoGlobs.gameName, submenuPathBuff, "MenuImageDark", 0));
            Front_Menu_LoadMenuImage(menuSet->menus[menuIndex], menuImageDark, FALSE);

            sprintf(menuSet->menus[menuIndex]->name, "%s", submenuPathBuff);

            Bool3 playRandom = Config_GetBoolValue(legoGlobs.config, Config_BuildStringID(legoGlobs.gameName, submenuPathBuff, "PlayRandom", 0));
            menuSet->menus[menuIndex]->playRandom = playRandom;

            Mem_Free(title);

#ifdef LEGORR_FIX_MEMORY_LEAKS
            // Not in original code, which causes a memory leak
            Mem_Free(fullName);
            if (anchored)
                Mem_Free(anchored);
#endif


            menuIndex = menuNumber;
        } while (menuNumber < menuCount);
    }

    menuIndex = 0;
    if (menuCount < 1)
        return menuSet;

    va_list args;
    va_start(args, menuName);

    for (menuNumber = menuIndex + 1; menuNumber <= menuCount; menuNumber++)
    {
        B32 autoCenter = menuSet->menus[menuIndex]->autoCenter;

        sprintf(submenuPathBuff, "%s::Menu%i", menuPathBuff, menuNumber);

        const char* loFontName = Config_GetTempStringValue(legoGlobs.config, Config_BuildStringID(legoGlobs.gameName, submenuPathBuff, "LoFont", 0));
        lpFont loFont = Front_Cache_LoadFont(loFontName);

        const char* hiFontName = Config_GetTempStringValue(legoGlobs.config, Config_BuildStringID(legoGlobs.gameName, submenuPathBuff, "HiFont", 0));
        lpFont hiFont = Front_Cache_LoadFont(hiFontName);

        S32 itemCount = Config_GetIntValue(legoGlobs.config, Config_BuildStringID(legoGlobs.gameName, submenuPathBuff, "ItemCount", 0));

        S32 overlayNumber = 1;
        do
        {
            char overlayBuff[1024];
            sprintf(overlayBuff, "Overlay%i", overlayNumber);

            const char* overlayName = Config_GetStringValue(legoGlobs.config, Config_BuildStringID(legoGlobs.gameName, submenuPathBuff, overlayBuff, 0));
            if (overlayName == NULL)
            {
                if ((mainGlobs.flags & MAIN_FLAG_REDUCEFLICS) != MAIN_FLAG_NONE)
                    break;

                sprintf(overlayBuff, "!Overlay%i", overlayNumber);
                overlayName = Config_GetStringValue(legoGlobs.config, Config_BuildStringID(legoGlobs.gameName, submenuPathBuff, overlayBuff, 0));

                if (overlayName == NULL)
                    break;
            }

            Util_Tokenize(overlayName, stringParts, ":");

            SFX_ID sfxType;
            if(!SFX_GetType(stringParts[1], &sfxType))
                sfxType = SFX_NULL;

            S32 xPos = atoi(stringParts[2]);
            S32 yPos = atoi(stringParts[3]);
            Front_Menu_CreateOverlay(stringParts[0], &menuSet->menus[menuIndex]->overlays, xPos, yPos, sfxType);

            if (overlayName != NULL)
                Mem_Free(overlayName);

            overlayNumber++;
        } while (TRUE);

        S32 itemNumber;
        char* str = NULL;
        if (itemCount > 0)
        {
            S32 iVar6 = 0;
            do
            {
                itemNumber = iVar6 + 1;

                char menuItemBuff[1024];
                sprintf(menuItemBuff, "Item%i", itemNumber);

                if (str != NULL)
                    Mem_Free(str);

                str = Config_GetStringValue(legoGlobs.config, Config_BuildStringID(legoGlobs.gameName, submenuPathBuff, menuItemBuff, 0));
                S32 stringPartCount = Util_Tokenize(str, stringParts, ":");
                MenuItem_Type type = Front_MenuItem_ParseTypeString(stringParts[0]);

                switch (type)
                {
                    case MenuItem_Type_Cycle:
                    {
                        // TODO: Implement Front_LoadMenuSet
                        break;
                    }
                    case MenuItem_Type_Trigger:
                    {
                        if (stringPartCount == 8) // Trigger:x1:y1:loImage:hiImage:unusedImage:toolTip:end
                        {
                            B32* valuePtr = va_arg(args, B32*);
                            MenuItem_TriggerCallback callback = va_arg(args, MenuItem_TriggerCallback);

                            // TODO: Is this ACTUALLY a Bool3?
                            B32 end = atoi(stringParts[7]) == BOOL3_TRUE; // Numeric boolean
                            lpMenuItem_TriggerData trigger = Front_MenuItem_CreateTrigger(valuePtr, end, callback);

                            const char* loImageName = stringParts[3];
                            const char* hiImageName = stringParts[4];
                            const char* unusedImageName = stringParts[5];
                            const char* toolTipName = stringParts[6];

                            S32 x1 = atoi(stringParts[1]);
                            S32 y1 = atoi(stringParts[2]);

                            lpMenuItem menuItem = Front_MenuItem_CreateImageItem("",
                                                                                 loFont, hiFont,
                                                                                 loImageName, hiImageName,
                                                                                 x1, y1,
                                                                                 MenuItem_Type_Trigger,
                                                                                 autoCenter, toolTipName, trigger);

                            Front_Menu_AddMenuItem(menuSet->menus[menuIndex], menuItem);
                        } else if (stringPartCount == 5) // Trigger:x1:y1:banner:end
                        {
                            B32* valuePtr = va_arg(args, B32*);
                            MenuItem_TriggerCallback callback = va_arg(args, MenuItem_TriggerCallback);

                            // TODO: Is this ACTUALLY a Bool3?
                            B32 end = atoi(stringParts[4]) == BOOL3_TRUE; // Numeric boolean
                            lpMenuItem_TriggerData trigger = Front_MenuItem_CreateTrigger(valuePtr, end, callback);

                            S32 x1 = atoi(stringParts[1]);
                            S32 y1 = atoi(stringParts[2]);

                            const char* banner = Front_Util_StringReplaceChar(stringParts[3], '_', ' ');

                            lpMenuItem menuItem = Front_MenuItem_CreateBannerItem(banner, loFont, hiFont, x1, y1, MenuItem_Type_Trigger, autoCenter, trigger, FALSE);
                            Front_Menu_AddMenuItem(menuSet->menus[menuIndex], menuItem);
                        }
                        break;
                    }
                    case MenuItem_Type_Slider:
                    {
                        // TODO: Implement Front_LoadMenuSet
                        break;
                    }
                    case MenuItem_Type_RealSlider:
                    {
                        // TODO: Implement Front_LoadMenuSet
                        break;
                    }
                    case MenuItem_Type_Next:
                    {
                        if (stringPartCount == 8)
                        {
                            S32 nextMenuNumber = atoi(stringParts[7] + 4);

                            lpMenu nextMenu = menuSet->menus[nextMenuNumber - 1];

                            const char* loImageName = stringParts[3];
                            const char* hiImageName = stringParts[4];
                            // Image path at stringParts[5] seems to be unused.
                            const char* toolTipName = stringParts[6];
                            S32 x1 = atoi(stringParts[1]);
                            S32 y1 = atoi(stringParts[2]);

                            lpMenuItem menuItem = Front_MenuItem_CreateImageItem("",
                                                                                 loFont,
                                                                                 hiFont,
                                                                                 loImageName,
                                                                                 hiImageName,
                                                                                 x1,
                                                                                 y1,
                                                                                 MenuItem_Type_Next,
                                                                                 autoCenter,
                                                                                 toolTipName,
                                                                                 nextMenu);

                            Front_Menu_AddMenuItem(menuSet->menus[menuIndex], menuItem);
                        } else
                        {
                            S32 nextMenuNumber;
                            B32 notInTutorial;
                            lpMenu nextMenu;
                            if (stringPartCount == 6)
                            {
                                nextMenuNumber = atoi(stringParts[4] + 4);
                                nextMenu = menuSet->menus[nextMenuNumber - 1];
                                notInTutorial = TRUE;
                            } else
                            {
                                if (stringParts[4][0] == '\0')
                                {
                                    notInTutorial = FALSE;
                                    nextMenu = NULL;
                                } else
                                {
                                    nextMenuNumber = atoi(stringParts[4] + 4);
                                    nextMenu = menuSet->menus[nextMenuNumber - 1];
                                    notInTutorial = FALSE;
                                }
                            }

                            S32 x1 = atoi(stringParts[1]);
                            S32 y1 = atoi(stringParts[2]);

                            const char* banner = Front_Util_StringReplaceChar(stringParts[3], '_', ' ');

                            lpMenuItem menuItem = Front_MenuItem_CreateBannerItem(banner, loFont, hiFont, x1, y1, MenuItem_Type_Next, menuSet->menus[menuIndex]->autoCenter, nextMenu, notInTutorial);
                            Front_Menu_AddMenuItem(menuSet->menus[menuIndex], menuItem);
                        }
                        break;
                    }
                }

                iVar6 = itemNumber;
            } while (itemNumber < itemCount);
        }

        if (str != NULL)
            Mem_Free(str);

        menuIndex = menuNumber;
    }

    va_end(menuName);

    return menuSet;
}

B32 Front_LoadLevelSet(lpConfig config, lpLevelSet levelSet, const char* levelKey)
{
#ifdef LEGORR_FIX_MEMORY_LEAKS
    const char* nextName = Config_GetTempStringValue(config, Config_BuildStringID(legoGlobs.gameName, "Main", levelKey, 0));
#else
    const char* nextName = Config_GetStringValue(config, Config_BuildStringID(legoGlobs.gameName, "Main", levelKey, 0));
#endif
    if (nextName == NULL)
        return FALSE;

    S32 count = 0;
    do
    {
        count++;
#ifdef LEGORR_FIX_MEMORY_LEAKS
        nextName = Config_GetTempStringValue(config, Config_BuildStringID(legoGlobs.gameName, nextName, "NextLevel", 0));
#else
        nextName = Config_GetStringValue(config, Config_BuildStringID(legoGlobs.gameName, nextName, "NextLevel", 0));
#endif
    } while (nextName != NULL);

    levelSet->count = count;
    levelSet->idNames = Mem_Alloc(count * sizeof(char*));
    levelSet->langNames = Mem_Alloc(count * sizeof(char*));
    levelSet->levels = Mem_Alloc(count * sizeof(lpLevelLink));
    levelSet->visitedList = Mem_Alloc(count * sizeof(B32));

    memset(levelSet->levels, 0, count * sizeof(lpLevelLink));
    memset(levelSet->visitedList, 0, count * sizeof(B32));

    if (levelSet->idNames != NULL && levelSet->langNames != NULL)
    {
        char* nextNameStr = Config_GetStringValue(config, Config_BuildStringID(legoGlobs.gameName, "Main", levelKey, 0));
        for (S32 i = 0; i < levelSet->count; i++)
        {
            levelSet->idNames[i] = nextNameStr;

            levelSet->langNames[i] = Config_GetStringValue(config, Config_BuildStringID(legoGlobs.gameName, nextNameStr, "FullName", 0));

            if (levelSet->langNames[i] == NULL)
            {
                // Just use the id name as the language name.
                levelSet->langNames[i] = Front_Util_StrCpy(levelSet->idNames[i]);
            } else {
                // Don't use Front_Util_ReplaceTextSpaces because that returns a temporary buffer.
                Front_Util_StringReplaceChar(levelSet->langNames[i], '_', ' ');
            }

            nextNameStr = Config_GetStringValue(config, Config_BuildStringID(legoGlobs.gameName, nextNameStr, "NextLevel", 0));
        }

        return TRUE;
    }

    return FALSE;
}

lpMenuSet Front_CreateMenuSet(U32 menuCount)
{
    lpMenuSet menuSet = Mem_Alloc(sizeof(MenuSet));

    // While not necessary, this is in the original code
    menuSet->menus = NULL;
    menuSet->menuCount = 0;

    menuSet->menus = Mem_Alloc(menuCount * sizeof(lpMenu));
    memset(menuSet->menus, 0, menuCount * sizeof(lpMenu));
    menuSet->menuCount = menuCount;

    return menuSet;
}

lpMenu Front_Menu_CreateMenu(const char* title, const char* fullName, lpFont menuFont, S32 positionX, S32 positionY, B32 autoCenter, B32 dislayTitle, S32 centerX, B32 canScroll, const char* anchored)
{
    // TODO: This function doesn't free memory on error

    lpMenu menu = Mem_Alloc(sizeof(Menu));
    if (!menu)
        return NULL;

    memset(menu, 0, sizeof(Menu));

    menu->title = Front_Util_StrCpy(title);
    menu->fullName = Front_Util_StrCpy(fullName);

    if (!menu->title)
        return NULL;

    if (!menu->fullName)
        return NULL;

    menu->titleLength = strlen(menu->title);
    menu->itemCapacity = 15;
    menu->items = Mem_Alloc(menu->itemCapacity * sizeof(lpMenuItem));

    if (menu->items == NULL)
    {
        Front_Menu_FreeMenu(menu);
        return NULL;
    }

    menu->menuFont = menuFont;
    menu->position.y = positionY;
    menu->autoCenter = autoCenter;
    menu->displayTitle = dislayTitle;
    menu->position.x = positionX;
    //menu->itemCapacity = 15; // moved above to use in items allocation
    menu->itemCount = 0;
    menu->itemFocus = 0;
    menu->closed = FALSE;
    menu->centerX = centerX;
    menu->flags = canScroll ? MENU_FLAG_CANSCROLL : MENU_FLAG_NONE;

    if (anchored != NULL)
    {
        menu->anchored = TRUE;
        char* stringParts[100];
        Util_Tokenize(anchored, stringParts, ":");
        menu->anchoredPosition.x = atoi(stringParts[0]);
        menu->anchoredPosition.y = atoi(stringParts[1]);
    }

    return menu;
}

void Front_Menu_FreeMenu(lpMenu menu)
{
    // TODO: Implement Front_Menu_FreeMenu
}

B32 Front_Menu_LoadMenuImage(lpMenu menu, const char* filename, B32 light)
{
    if (filename == NULL)
        return FALSE;

    char buff[1024];
    strcpy(buff, filename);

    char* stringParts[4];
    U32 numParts = Util_Tokenize(buff, stringParts, ":");
    // cfg: filename.bmp[:xPos:yPos[:trans=0/1]]
    if (numParts > 2)
    {
        menu->flags |= MENU_FLAG_HASPOSITION;
        menu->currPosition.x = atof(stringParts[1]);
        menu->currPosition.y = atof(stringParts[2]);
    }

    lpImage image;
    B32 transBool;

    if (light)
    {
        image = Front_Cache_LoadImage(stringParts[0]);
        menu->menuImage = image;
        if (image != NULL && numParts == 4)
        {
            transBool = atoi(stringParts[3]);
            if (transBool)
                Image_SetPenZeroTrans(menu->menuImage);

            return menu->menuImage != NULL;
        }
    } else {
        image = Front_Cache_LoadImage(stringParts[0]);
        menu->menuImageDark = image;
        if (image != NULL && numParts == 4)
        {
            transBool = atoi(stringParts[3]);
            if (transBool)
                Image_SetupTrans(menu->menuImage, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f);

            return menu->menuImageDark != NULL;
        }
    }
}

lpMenuOverlay Front_Menu_CreateOverlay(const char* filename, lpMenuOverlay* linkedOverlay, S32 positionX, S32 positionY, SFX_ID sfxType)
{
    lpMenuOverlay overlay = Mem_Alloc(sizeof(MenuOverlay));
    memset(overlay, 0, sizeof(MenuOverlay));

    // Okay... so don't use a filename that STARTS with "null"...?
    if (_strnicmp(filename, "Null", 4) != 0)
    {
        if (filename != NULL)
            overlay->filename = Front_Util_StrCpy(filename);

        overlay->overlayType = Front_Menu_GetOverlayType(overlay);
        overlay->field_1c = 0;
        overlay->position.x = positionX;
        overlay->position.y = positionY;
        overlay->previous = *linkedOverlay;
        overlay->sfxType = sfxType;

        *linkedOverlay = overlay;
    }

    return overlay;
}

MenuOverlay_Type Front_Menu_GetOverlayType(lpMenuOverlay menuOverlay)
{
    char* ext = menuOverlay->filename;
    char c = *ext;

    // TODO: Memory access violation if there's no '.' extension.
    while (c != '.')
    {
        char* curr = ext + 1;
        ext++;
        c = *curr;
    }

    if (_stricmp(".avi", ext) == 0)
        return MenuOverlay_Type_Animation;

    if (_stricmp(".bmp", ext) == 0)
        return MenuOverlay_Type_Image;

    if (_stricmp(".lws", ext) == 0)
        return MenuOverlay_Type_Lws;

    return MenuOverlay_Type_Flic;
}

void Front_LoadLevels(lpMenuSet mainMenuFull)
{
    frontGlobs.startMissionLink = Front_LevelSet_LoadLevelLinks(&frontGlobs.missionLevels, frontGlobs.missionLevels.idNames[0]);
    frontGlobs.startTutorialLink = Front_LevelSet_LoadLevelLinks(&frontGlobs.tutorialLevels, frontGlobs.tutorialLevels.idNames[0]);

    Front_Levels_ResetVisited();

    S32 menuIDLevels = Front_GetMenuIDByName(frontGlobs.mainMenuSet, "Levels");
    S32 menuIDTutorials = Front_GetMenuIDByName(frontGlobs.mainMenuSet, "Tutorials");
    S32 menuIDLoadSave = Front_GetMenuIDByName(frontGlobs.mainMenuSet, "Load Level Save");

    // Create menu item for Load Game menu selection.
    lpMenuItem_SelectData loadSelectData = Front_MenuItem_CreateSelect(&frontGlobs.menuVars[MENUVAR_SELECT_LOADSAVE_INDEX], "", "", -200, -100, 60, 5, -20, 200, -20, -40, 0,
                                                                       Front_Callback_SelectLoadSave, frontGlobs.mainMenuSet->menus[menuIDLevels]);
    lpMenuItem menuLoadSave = frontGlobs.mainMenuSet->menus[menuIDLoadSave]->items[0];
    lpMenuItem loadSelectItem = Front_MenuItem_CreateBannerItem("", menuLoadSave->fontLo, menuLoadSave->fontHi, 0, 0, MenuItem_Type_Select, TRUE, loadSelectData, FALSE);
    Front_Menu_AddMenuItem(frontGlobs.mainMenuSet->menus[menuIDLoadSave], loadSelectItem);

    // Create menu item for Save Game menu selection.
    lpMenuItem_SelectData saveSelectData = Front_MenuItem_CreateSelect(&frontGlobs.menuVars[MENUVAR_SELECT_LOADSAVE_INDEX], "", "", -200, -100, 60, 5, -20, 200, -20, -40, 0,
                                                                       Front_Callback_SelectLoadSave, NULL);

    lpMenuItem saveSelectItem = Front_MenuItem_CreateBannerItem("", menuLoadSave->fontLo, menuLoadSave->fontHi, 0, 0, MenuItem_Type_Select, TRUE, saveSelectData, FALSE);
    Front_Menu_AddMenuItem(frontGlobs.saveMenuSet->menus[0], saveSelectItem);

    char buff[100];

    // NOTE: This DOES NOT loop through the last index [5] in the array.
    for (U32 i = 0; i < 5; i++)
    {
        const char* saveGameText = Config_GetTempStringValue(legoGlobs.config, Config_BuildStringID(legoGlobs.gameName, "Menu", "Save_Game", 0));
        sprintf(buff, "%s %i", saveGameText, (i + 1));
        Front_Util_StringReplaceChar(buff, '_', ' ');
        Front_Save_GetLevelCompleteWithPoints(&frontGlobs.saveData[i], buff);

        // Add save item for Load Game menu selection.
        Front_MenuItem_AddSelectItem(loadSelectData, buff, TRUE,
                                     frontGlobs.mainMenuSet->menus[menuIDLoadSave]->items[0]->fontHi, 0, 0, FALSE);

        // Add save item for Save Game menu selection.
        Front_MenuItem_AddSelectItem(saveSelectData, buff, TRUE,
                                     frontGlobs.mainMenuSet->menus[menuIDLoadSave]->items[0]->fontHi, 0, 0, FALSE);
    }

    Front_Save_LoadAllSaveFiles();

    MainMenuFull_AddMissionsDisplay(MENUVAR_SELECT_MISSION_INDEX, frontGlobs.startMissionLink, &frontGlobs.missionLevels, frontGlobs.mainMenuSet->menus[menuIDLevels], Front_Save_GetCurrentSaveData(), NULL, Front_Callback_SelectMissionItem);
    MainMenuFull_AddMissionsDisplay(MENUVAR_SELECT_TUTORIAL_INDEX, frontGlobs.startTutorialLink, &frontGlobs.tutorialLevels, frontGlobs.mainMenuSet->menus[menuIDTutorials], Front_Save_GetCurrentSaveData(), NULL, Front_Callback_SelectTutorialItem);
}

lpLevelLink Front_LevelSet_LoadLevelLinks(lpLevelSet levelSet, const char* levelName)
{
    B32 isLinked;;
    lpLevelLink info;
    S32 setIndex;
    char* linkNames;
    U32 numParts;
    lpLevelLink* levelLinks;
    lpLevelLink linkInfo;
    U32 i;
    char* stringParts[15]; // Maximum of 15 linked levels

    if (levelName == NULL)
        return NULL;

    isLinked = Front_LevelSet_IsLinkVisited(levelSet, levelName);
    if (isLinked)
    {
        linkInfo = Front_LevelSet_GetLevelLink(levelSet, levelName);
        return linkInfo;
    }

    info = Mem_Alloc(sizeof(LevelLink));

    Front_LevelSet_SetLinkVisited(levelSet, levelName, TRUE);
    Front_LevelSet_SetLevelLink(levelSet, levelName, info);

    if (info == NULL)
        return NULL;

    setIndex = Front_LevelSet_IndexOf(levelSet, levelName);

    if (setIndex != -1)
    {
        linkNames = Config_GetStringValue(legoGlobs.config, Config_BuildStringID(legoGlobs.gameName, levelName, "LevelLinks", 0));

        info->setIndex = setIndex;

        i = 0;
        if (linkNames == NULL)
        {
            info->linkLevels = NULL;
        } else {
            numParts = Util_Tokenize(linkNames, stringParts, ",");
            if (numParts == 0)
            {
#ifdef LEGORR_FIX_MEMORY_LEAKS
                Mem_Free(linkNames);
#endif
                Mem_Free(info);
                return NULL;
            }

            levelLinks = Mem_Alloc(numParts * 4);
            info->linkLevels = levelLinks;

            if (numParts != 0)
            {
                do
                {
                    linkInfo = Front_LevelSet_LoadLevelLinks(levelSet, stringParts[i]);
                    info->linkLevels[i] = linkInfo;
                    i++;
                } while(i < numParts);

#ifdef LEGORR_FIX_MEMORY_LEAKS
                Mem_Free(linkNames);
#endif
                info->linkCount = i;
                return info;
            }

#ifdef LEGORR_FIX_MEMORY_LEAKS
            Mem_Free(linkNames);
#endif
        }

        info->linkCount = 0;
        return info;
    }

    Mem_Free(info);
    return NULL;
}

B32 Front_LevelSet_IsLinkVisited(lpLevelSet levelSet, const char* levelName)
{
    for (S32 i = 0; i < levelSet->count; i++)
    {
        if (strcmp(levelSet->idNames[i], levelName) == 0)
            return levelSet->visitedList[i];
    }
    return FALSE;
}

void Front_LevelSet_SetLinkVisited(lpLevelSet levelSet, const char* levelName, B32 visited)
{
    for (S32 i = 0; i < levelSet->count; i++)
    {
        if (strcmp(levelSet->idNames[i], levelName) == 0)
            levelSet->visitedList[i] = visited;
    }
}

lpLevelLink Front_LevelSet_GetLevelLink(lpLevelSet levelSet, const char* levelName)
{
    for (S32 i = 0; i < levelSet->count; i++)
    {
        if (strcmp(levelSet->idNames[i], levelName) == 0)
            return levelSet->levels[i];
    }

    return NULL;
}

void Front_LevelSet_SetLevelLink(lpLevelSet levelSet, const char* levelName, lpLevelLink link)
{
    for (S32 i = 0; i < levelSet->count; i++)
    {
        if (strcmp(levelSet->idNames[i], levelName) == 0)
            levelSet->levels[i] = link;
    }
}

S32 Front_LevelSet_IndexOf(lpLevelSet levelSet, const char* levelName)
{
    S32 cmp;
    S32 i;

    i = 0;
    if (levelSet->count < 1)
        return -1;

    do
    {
        cmp = _stricmp(levelName, levelSet->idNames[i]);
        if (cmp == 0)
            return i;

        i++;
    } while (i < levelSet->count);

    return -1;
}

void Front_Levels_ResetVisited()
{
    S32 i;

    i = 0;
    if (frontGlobs.missionLevels.count > 0)
    {
        do
        {
            if (frontGlobs.missionLevels.levels[i] != NULL)
            {
                frontGlobs.missionLevels.levels[i]->visited = FALSE;
                frontGlobs.missionLevels.visitedList[i] = FALSE;
            }
            i++;
        } while (i < frontGlobs.missionLevels.count);
    }

    i = 0;

    if (frontGlobs.tutorialLevels.count > 0)
    {
        do
        {
            if (frontGlobs.tutorialLevels.levels[i] != NULL)
            {
                frontGlobs.tutorialLevels.levels[i]->visited = FALSE;
                frontGlobs.tutorialLevels.visitedList[i] = FALSE;
            }
            i++;
        } while (i < frontGlobs.tutorialLevels.count);
    }
}

B32 Front_LevelLink_RunThroughLinks(lpLevelLink startLink, LevelLink_RunThroughLinksCallback callback, void* data)
{
    if (startLink != NULL && !startLink->visited)
    {
        startLink->visited = TRUE;

        if (callback(startLink, data))
            return TRUE;

        // linkLevels is null if there are no links.
        if (startLink->linkLevels)
        {
            for (U32 i = 0; i < startLink->linkCount; i++)
            {
                if (Front_LevelLink_RunThroughLinks(startLink->linkLevels[i], callback, data))
                    return TRUE;
            }
        }
    }
    return FALSE;
}

S32 Front_LevelLink_FindSetIndexOf(lpLevelLink startLink, S32 linkIndex)
{
    lpLevelLink info = Front_LevelLink_FindByLinkIndex(startLink, linkIndex);
    if (info != NULL)
        return info->setIndex;

    return -1;
}

lpLevelLink Front_LevelLink_FindByLinkIndex(lpLevelLink startLink, S32 linkIndex)
{
    SearchLevelLinkFindIndex_10 search;

    search.resultLink = NULL;
    search.currentIndex = 0;
    search.resultIndex = 0;
    search.searchIndex = linkIndex;

    if (Front_LevelLink_RunThroughLinks(startLink, Front_LevelLink_Callback_FindByLinkIndex, &search))
    {
        Front_Levels_ResetVisited();
        return search.resultLink;
    }

    Front_Levels_ResetVisited();
    return NULL;
}

void Front_Levels_UpdateAvailable(lpLevelLink startLink, lpSaveReward saveReward, lpLevelSet levelSet, lpMenuItem_SelectData selectData, B32 keepLocked)
{
    SearchLevelSelectInfo_14 search;
    search.levelSet = levelSet;
    search.selectData = selectData;
    search.saveReward = saveReward;
    search.index = 0;
    search.keepLocked = keepLocked;

    Front_Levels_UpdateAvailable_Recursive(startLink, &search, TRUE);
    Front_Levels_ResetVisited();
}

void Front_Levels_UpdateAvailable_Recursive(lpLevelLink link, lpSearchLevelSelectInfo_14 search, B32 unlocked)
{
    lpMenuItem_SelectItem levelData;
    MenuItem_SelectItemFlags lflags;
    SaveRewardFlags completed;
    U32 i;
    lpMenuItem_SelectItem levelsList;

    i = 0;

    if (link == NULL || link->visited != 0)
        return;

    if (mainGlobs.programmerLevel > PROGRAMMER_MODE_1 || (mainGlobs.flags & MAIN_FLAG_LEVELSOPEN) != MAIN_FLAG_NONE)
        search->keepLocked = FALSE;

    link->visited = TRUE;

    if (search->saveReward == NULL ||
        mainGlobs.programmerLevel > PROGRAMMER_MODE_1 ||
        (mainGlobs.flags & MAIN_FLAG_LEVELSOPEN) != MAIN_FLAG_NONE)
    {
        completed = (SaveRewardFlags)(search->keepLocked == 0);
    } else {
        completed = search->saveReward[link->setIndex].flags & SAVEREWARD_FLAG_COMPLETED;
    }

    if (completed == SAVEREWARD_FLAG_NONE && unlocked == 0 && (levelsList = search->selectData->selItemList, levelData = levelsList + search->index, levelsList[search->index].frontEndOpen == 0))
    {
        lflags = levelData->flags & ~SELECTITEM_FLAG_ENABLED;
    } else {
        levelsList = search->selectData->selItemList;
        levelData = levelsList + search->index;
        lflags = levelsList[search->index].flags | SELECTITEM_FLAG_ENABLED;
    }

    levelData->flags = lflags;
    search->index++;

    if (link->linkCount != 0)
    {
        do
        {
            Front_Levels_UpdateAvailable_Recursive(link->linkLevels[i], search, completed);
            i++;
        } while (i < link->linkCount);
    }
}

void MainMenuFull_AddMissionsDisplay(Front_MenuVars menuVar, lpLevelLink startLink, lpLevelSet levelSet, lpMenu menu, lpSaveData saveData, lpMenu nextMenu, void* callback)
{
    U32 count = 0;

    Front_LevelLink_RunThroughLinks(startLink, Front_LevelLink_Callback_IncCount, &count);
    Front_Levels_ResetVisited();

    lpMenuItem_SelectData select = Front_MenuItem_CreateSelect(&frontGlobs.menuVars[menuVar], "", "", 0, 0, 0, count, 0, 0, 0, 0, 0, callback, nextMenu);
    lpMenuItem menuItem = Front_MenuItem_CreateBannerItem("Levels!!!", NULL, NULL, 0, 0, MenuItem_Type_Select, TRUE, select, FALSE);
    Front_Menu_AddMenuItem(menu, menuItem);

    SearchLevelSelectAdd search;
    search.levelSet = levelSet;
    search.menu_4 = menu;
    search.itemData = NULL;
    search.currSave = saveData;
    search.itemData = select;

    Front_LevelLink_RunThroughLinks(startLink, Front_LevelInfo_Callback_AddItem, &search);
    Front_Levels_ResetVisited();
}

S32 Front_GetMenuIDByName(lpMenuSet menuSet, const char* name)
{
    S32 cmp;
    S32 i;

    i = 0;
    if (menuSet->menuCount < 1)
        return -1;

    while ((menuSet->menus[i] == NULL || (cmp = _stricmp(menuSet->menus[i]->title, name), cmp != 0)))
    {
        i++;
        if (menuSet->menuCount <= i)
            return -1;
    }

    return i;
}

void Front_LevelSelect_LevelNamePrintF(lpFont font, S32 x, S32 y, const char* msg, ...)
{
    // TODO: Implement Front_LevelSelect_LevelNamePrintF
}

void Front_Callback_TriggerPlayCredits()
{
    Credits_Play(legoGlobs.CreditsTextFile, legoGlobs.bmpRSFont, legoGlobs.CreditsBackAVI);
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

void Front_Callback_SelectMissionItem(F32 elapsedAbs, S32 selectIndex)
{
    char buffLevel[64]; // Used to display level number (1-indexed) with `-testercall` cmd option.
    char buffMsg[300];

    lpMenuTextWindow menuTextWnd = frontGlobs.saveLevelWnd;
    lpFont font = frontGlobs.mainMenuSet->menus[1]->menuFont;
    lpLevelLink link = Front_LevelLink_FindByLinkIndex(frontGlobs.startMissionLink, selectIndex);
    const char* langName = frontGlobs.missionLevels.langNames[link->setIndex];

    memset(buffLevel, 0, sizeof(buffLevel));

    lpSaveData currSave = Front_Save_GetCurrentSaveData();
    frontGlobs.levelSelectLastNumber = frontGlobs.levelSelectHoverNumber;
    frontGlobs.levelSelectHoverNumber = link->setIndex + 1;

    if ((mainGlobs.flags & MAIN_FLAG_TESTERCALL) != MAIN_FLAG_NONE)
        sprintf(buffLevel, " (Level %d)", link->setIndex + 1);

    // Fallback to ID name when language name (cfg: FullName) isn't defined.
    if (langName == NULL || *langName == '\0')
        langName = frontGlobs.missionLevels.idNames[link->setIndex];

    if (menuTextWnd->textWindow == NULL)
    {
        // HARDCODED SCREEN RESOLUTION!! (390)
        Front_LevelSelect_LevelNamePrintF(font, (S32) mainGlobs.appWidth / 2 - (S32) (Font_GetStringWidth(font, langName)) / 2, 390, langName);
    }
    else if (g_levelSelectPrinting && (TextWindow_PrintF(menuTextWnd->textWindow, langName), mainGlobs.flags & MAIN_FLAG_TESTERCALL) != MAIN_FLAG_NONE)
        TextWindow_PrintF(menuTextWnd->textWindow, buffLevel);

    if (currSave != NULL && (currSave->missionsTable[link->setIndex].flags & SAVEREWARD_FLAG_COMPLETED) &&
                            !(currSave->missionsTable[link->setIndex].flags & SAVEREWARD_FLAG_TUTORIAL))
    {
        S32 score = Front_Save_GetLevelScore(link->setIndex, currSave);
        const char* completeText = Config_GetTempStringValue(legoGlobs.config, Config_BuildStringID(legoGlobs.gameName, "Menu", "Level_Completed", 0));
        sprintf(buffMsg, " %s (%i)", completeText, score);
        Front_Util_StringReplaceChar(buffMsg, '_', ' ');
    }
    else
    {
        const char* incompleteText = Config_GetTempStringValue(legoGlobs.config, Config_BuildStringID(legoGlobs.gameName, "Menu", "Level_Incomplete", 0));

        sprintf(buffMsg, " %s", incompleteText);
        Front_Util_StringReplaceChar(buffMsg, '_', ' ');
    }

    if (menuTextWnd->textWindow != NULL && g_levelSelectPrinting)
    {
        // TODO: Ahhh, lovely... nested printf calls without sanitization,
        //  it may be possible to intentionally corrupt memory with this.
        TextWindow_PrintF(menuTextWnd->textWindow, buffMsg);

        if (frontGlobs.levelSelectHoverNumber != frontGlobs.levelSelectLastNumber)
        {
            frontGlobs.levelSelectSFXStopped = TRUE;
            frontGlobs.levelSelectSFXTimer = 0.0f;
        }

        if (frontGlobs.levelSelectSFXStopped)
        {
            frontGlobs.levelSelectSFXTimer += elapsedAbs;

            if ((frontGlobs.levelSelectSFXTimer * STANDARD_FRAMERATE) > 500.0f)
            {
                B32 soundResult;

                // TODO: Uhhh, well then...
                //  tutorial levels are kind of hardcoded to a max of 9...???
                if ((S32)frontGlobs.levelSelectHoverNumber <= 8)
                    soundResult = Front_LevelSelect_PlayTutoLevelNameSFX(frontGlobs.levelSelectHoverNumber);
                else
                    soundResult = Front_LevelSelect_PlayLevelNameSFX(frontGlobs.levelSelectHoverNumber - 8); // levelSelectHoverNumber - NUM_TUTORIALS

                if (soundResult)
                    frontGlobs.levelSelectSFXStopped = FALSE;
            }
        }
    }

    g_levelSelectPrinting = FALSE;
}

void Front_Callback_SelectTutorialItem(F32 elapsedAbs, S32 selectIndex)
{
    LevelLink *info = Front_LevelLink_FindByLinkIndex(frontGlobs.startTutorialLink, selectIndex);
    char* langLevelName = frontGlobs.tutorialLevels.langNames[info->setIndex];
    frontGlobs.levelSelectLastNumber = frontGlobs.levelSelectHoverNumber;
    frontGlobs.levelSelectHoverNumber = info->setIndex + 1;
    // Fallback to config level ID name if no language name is defined.
    if (langLevelName == NULL || *langLevelName == '\0')
        langLevelName = frontGlobs.tutorialLevels.idNames[info->setIndex];

    if (frontGlobs.saveLevelWnd->textWindow != NULL && g_levelSelectPrinting)
    {
        TextWindow_PrintF(frontGlobs.saveLevelWnd->textWindow, langLevelName);
        if (frontGlobs.levelSelectHoverNumber != frontGlobs.levelSelectLastNumber)
        {
            frontGlobs.levelSelectSFXStopped = TRUE;
            frontGlobs.levelSelectSFXTimer = 0.0f;
        }

        if (frontGlobs.levelSelectSFXStopped && (frontGlobs.levelSelectSFXTimer = elapsedAbs + frontGlobs.levelSelectSFXTimer, 500.0f < frontGlobs.levelSelectSFXTimer * 25.0f))
        {
            if (Front_LevelSelect_PlayTutoLevelNameSFX(frontGlobs.levelSelectHoverNumber))
                frontGlobs.levelSelectSFXStopped = FALSE;
        }
    }

    g_levelSelectPrinting = FALSE;
}

B32 Front_LevelSelect_PlayLevelNameSFX(S32 levelNumber)
{
    SFX_ID sfxType;
    char buff[128];

    sprintf(buff, "Stream_LevelName_Level%d", levelNumber);
    if (SFX_GetType(buff, &sfxType))
        return SFX_Random_SetAndPlayGlobalSample(sfxType, NULL);

    return FALSE;
}

B32 Front_LevelSelect_PlayTutoLevelNameSFX(S32 levelNumber)
{
    SFX_ID sfxType;
    char buff[128];

    sprintf(buff, "Stream_LevelName_TLevel%d", levelNumber);
    if (SFX_GetType(buff, &sfxType))
        return SFX_Random_SetAndPlayGlobalSample(sfxType, NULL);

    return FALSE;
}

void Front_Callback_SelectLoadSave(F32 elapsedAbs, S32 selectIndex)
{
    // TODO: Implement Front_Callback_SelectLoadSave
}

B32 Front_LevelLink_Callback_IncCount(lpLevelLink link, void* pCount)
{
    (*(S32*)pCount)++;
    return FALSE;
}

B32 Front_LevelInfo_Callback_AddItem(lpLevelLink link, void* data)
{
    lpSearchLevelSelectAdd search = (lpSearchLevelSelectAdd)data;

    const char* levelName = search->levelSet->idNames[link->setIndex];

    S32 frontEndX = Config_GetIntValue(legoGlobs.config, Config_BuildStringID(legoGlobs.gameName, levelName, "FrontEndX", 0));
    S32 frontEndY = Config_GetIntValue(legoGlobs.config, Config_BuildStringID(legoGlobs.gameName, levelName, "FrontEndY", 0));
    Bool3 frontEndOpen_ = Config_GetBoolValue(legoGlobs.config, Config_BuildStringID(legoGlobs.gameName, levelName, "FrontEndOpen", 0));
    B32 frontEndOpen = (frontEndOpen_ == BOOL3_ERROR) ? FALSE : frontEndOpen_;

    const char* menuBMP = Config_GetStringValue(legoGlobs.config, Config_BuildStringID(legoGlobs.gameName, levelName, "MenuBMP", 0));
    if (menuBMP != NULL)
    {
        Front_MenuItem_AddSelectItem(search->itemData, menuBMP, FALSE, NULL, frontEndX, frontEndY, frontEndOpen);
        Mem_Free(menuBMP);
        return FALSE;
    }

    char buff[1024];
    strcpy(buff, frontGlobs.strDefaultLevelBMPS);
    Front_MenuItem_AddSelectItem(search->itemData, buff, FALSE, NULL, frontEndX, frontEndY, frontEndOpen);

    return FALSE;
}

B32 Front_LevelLink_Callback_FindByLinkIndex(lpLevelLink link, void* data)
{
    lpSearchLevelLinkFindIndex_10 search = (lpSearchLevelLinkFindIndex_10)data;

    S32 linkIndex = search->currentIndex;
    if (search->searchIndex == linkIndex)
    {
        search->resultIndex = linkIndex;
        search->resultLink = link;
        return TRUE;
    }

    search->currentIndex = linkIndex + 1;
    return FALSE;
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

void Front_Save_SetShouldClearUnlockedLevels(B32 state)
{
    frontGlobs.shouldClearUnlockedLevels = state;
}

void Front_Save_GetLevelCompleteWithPoints(lpSaveData saveData, char* buffer)
{
    // TODO: Implement Front_Save_GetLevelCompleteWithPoints
}

void Front_Save_LoadAllSaveFiles()
{
    // TODO: Implement Front_Save_LoadAllSaveFiles
}

S32 Front_Save_GetLevelScore(U32 index, lpSaveData saveData)
{
    // TODO: Implement Front_Save_GetLevelScore
    return 0;
}

S32 Front_SaveMenu_ConfirmMessage_FUN_004354f0(const char* titleText, const char* message, const char* okText, const char* cancelText)
{
    // TODO: Implement Front_SaveMenu_ConfirmMessage_FUN_004354f0
    return -1;
}

lpFront_Cache Front_Cache_Create(const char* filename)
{
    if ((filename == NULL) || (*filename == '\0'))
        return NULL;

    lpFront_Cache cache = Front_Cache_FindByName(filename);
    if (cache)
        return cache;

    cache = Mem_Alloc(sizeof(Front_Cache));

    // While strdup could be used here,
    // this code might have originally used the custom allocator,
    // so it's best to do it this way for now.
    // That way if the Mem_Free function is used and the custom allocator is used,
    // there could be a problem.
    char* fname = Mem_Alloc(strlen(filename) + 1);
    cache->path = fname;
    strcpy(fname, filename);

    cache->image = NULL;
    cache->next = g_ImageCache_NEXT;
    cache->font = NULL;
    g_ImageCache_NEXT = cache;

    return cache;
}

lpFront_Cache Front_Cache_FindByName(const char* filename)
{
    S32 cmp;
    lpFront_Cache cache;

    cache = g_ImageCache_NEXT;
    if (cache == NULL)
        return NULL;

    do
    {
        cmp = _stricmp(filename, cache->path);
        if (cmp == 0)
            return cache;
        cache = cache->next;
    } while (cache != NULL);
    return NULL;
}

lpFont Front_Cache_LoadFont(const char* filename)
{
    lpFront_Cache cache;
    lpFont font;

    if ((filename != NULL) && (*filename != '\0'))
    {
        cache = Front_Cache_Create(filename);
        if (cache->font == NULL)
        {
            font = Font_Load(filename);
            cache->font = font;
        }
        return cache->font;
    }
    return NULL;
}

lpImage Front_Cache_LoadImage(const char* filename)
{
    lpFront_Cache cache;
    lpImage image;

    if ((filename != NULL) && (*filename != '\0'))
    {
        cache = Front_Cache_Create(filename);
        if (cache->image == NULL)
        {
            image = Image_LoadBMP(filename);
            cache->image = image;
        }
        return cache->image;
    }
    return NULL;
}

void Front_LoadSaveSlotImages()
{
    // TODO: Implement Front_LoadSaveSlotImages
}

void Front_FreeSaveSlotImages()
{
    // TODO: Implement Front_FreeSaveSlotImages
}

lpSaveData Front_Save_GetCurrentSaveData()
{
    // TODO: Implement Front_Save_GetCurrentSaveData
    return NULL;
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

const char* Front_Util_StrCpy(const char* str)
{
    if (!str)
        return NULL;

    char* buffer = Mem_Alloc(strlen(str) + 1);
    if (buffer)
        strcpy(buffer, str);
    return buffer;
}

B32 Front_Maths_IsPointInsideRect(S32 ptX, S32 ptY, S32 rcX, S32 rcY, S32 rcWidth, S32 rcHeight)
{
    if (rcX <= ptX && ptX <= rcWidth + rcX)
    {
        if (rcY <= ptY && ptY <= rcHeight + rcY)
            return TRUE;
    }

    return FALSE;
}

B32 Front_Maths_IsPointInsideRectCentered(S32 ptX, S32 ptY, S32 rcX, S32 rcY, S32 rcWidth, S32 rcHeight, B32 shouldCenterX)
{
    S32 xOffset;
    if (shouldCenterX)
        xOffset = -(rcWidth / 2);
    else
        xOffset = 0;

    return Front_Maths_IsPointInsideRect(ptX, ptY, xOffset + rcX, rcY, rcWidth, rcHeight);
}

B32 Front_GetMousePressedState()
{
    static B32 s_frontMousePressedState = FALSE;

    B32 result = !inputGlobs.mslb && s_frontMousePressedState;

    s_frontMousePressedState = inputGlobs.mslb;

    return result;
}
