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

Front_Globs frontGlobs = { NULL };

lpFront_Cache g_ImageCache_NEXT = NULL;

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
    // TODO: Implement Front_PrepareScreenMenuType
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
            frontGlobs.selectLoadSaveIndex = -1;
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

    // TODO: Implement Front_ScreenMenuLoop

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

    // TODO: Implement Front_Menu_Update

    Front_Menu_DrawMenuImage(menu, TRUE);

    // TODO: Implement Front_Menu_Update

    if (menu->displayTitle)
    {
        U32 stringWidth = Font_GetStringWidth(menu->menuFont,
                                              Front_Util_ReplaceTextSpaces(menu->fullName));

        Font_PrintF(menu->menuFont,
                    (S32)(((S32)mainGlobs.appWidth / 2) - (stringWidth >> 1)),
                    menu->anchoredPosition.y + menu->position.y,
                    Front_Util_ReplaceTextSpaces(menu->fullName));
    }

    // TODO: Implement Front_Menu_Update


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
                    // TODO: Implement Front_Menu_Update
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
                    // TODO: Implement Front_Menu_Update
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
        // TODO: Implement Front_Menu_Update
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

    // TODO: Implement Front_Menu_Update

    Pointer_DrawPointer(inputGlobs.msx, inputGlobs.msy);
    Pointer_SetCurrent_IfTimerFinished(currPointer);
    return currMenu;
}

B32 Front_IsTriggerAppQuit()
{
    // TODO: Implement Front_IsTriggerAppQuit
    return FALSE;
}

lpMenu Front_Menu_UpdateMenuItemsInput(F32 elapsed, lpMenu menu)
{
    lpMenuItem menuItem = NULL; // Focused item
    if (menu->itemFocus >= 0)
        menuItem = menu->items[menu->itemFocus];

    if (frontGlobs.rockWipeFlags & ROCKWIPE_FLAG_NOINPUT)
        return menu;

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

    // TODO: Implement Front_Menu_UpdateMenuItemsInput

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
    // TODO: Implement Front_Menu_IsLevelItemUnderMouse
    return FALSE;
}

void Front_MenuItem_DrawSelectTextWindow(lpMenu* menu)
{
    // TODO: Implement Front_MenuItem_DrawSelectTextWindow
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

void Front_RockWipe_Play()
{
    // TODO: Implement Front_RockWipe_Play
}

void Front_RockWipe_Stop()
{
    // TODO: Implement Front_RockWipe_Stop
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

    for (menuNumber = menuIndex + 1; menuNumber < menuCount; menuNumber++)
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
                        // TODO: Implement Front_LoadMenuSet
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
                            // TODO: Implement Front_LoadMenuSet
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
    // TODO: Implement Front_LoadLevelSet
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
    // TODO: Implement Front_Menu_CreateOverlay
    return NULL;
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

void Front_Callback_SelectLoadSave(F32 elapsedAbs, S32 selectIndex)
{
    // TODO: Implement Front_Callback_SelectLoadSave
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
