#pragma once

#include "platform.h"
#include "movie.h"
#include "config.h"
#include "fonts.h"
#include "sfx.h"
#include "text_window.h"
#include "save.h"
#include "flic.h"

typedef enum Menu_ScreenType
{
    Menu_Screen_Title = 0,
    Menu_Screen_Missions = 1,
    Menu_Screen_Training = 2,
    Menu_Screen_Load_unused = 3,
    Menu_Screen_Save = 4,
    Menu_Screen_Count = 5,
} Menu_ScreenType;

typedef enum MenuFlags
{
    MENU_FLAG_NONE = 0,
    MENU_FLAG_HASPOSITION = 0x1,
    MENU_FLAG_CANSCROLL = 0x2,
} MenuFlags;

typedef enum Menu_ModalType
{
    Menu_Modal_Paused  = 0,
    Menu_Modal_Options = 1,
    Menu_Modal_Count   = 2,
} Menu_ModalType;

typedef enum MenuItem_Type
{
    MenuItem_Type_Cycle      = 0,
    MenuItem_Type_Trigger    = 1,
    MenuItem_Type_TextInput  = 2,
    MenuItem_Type_Slider     = 3,
    MenuItem_Type_RealSlider = 4,
    MenuItem_Type_Next       = 5,
    MenuItem_Type_Select     = 6,
    MenuItem_Type_Count      = 7,
    MenuItem_Type_Invalid    = -1,
} MenuItem_Type;

typedef enum ToolTip_Type
{
    ToolTip_Null                       = 0,
    ToolTip_RadarBlock                 = 1,
    ToolTip_UnitSelect                 = 2,
    ToolTip_Construction               = 3,
    ToolTip_InterfaceMenu              = 4,
    ToolTip_InterfaceMenuBackButton    = 5,
    ToolTip_MapBlock                   = 6,
    ToolTip_Priority                   = 7,
    ToolTip_InfoMenuContinue           = 8,
    ToolTip_InfoMenuDisableFuture      = 9,
    ToolTip_RadarToggle                = 10,
    ToolTip_RadarObjectView            = 11,
    ToolTip_RadarZoomIn                = 12,
    ToolTip_RadarZoomOut               = 13,
    ToolTip_RadarMapView               = 14,
    ToolTip_InfoDockGoto               = 15,
    ToolTip_InfoDockClose              = 16,
    ToolTip_TopPanelCallToArms         = 17,
    ToolTip_TopPanelInfo               = 18,
    ToolTip_TopPanelOptions            = 19,
    ToolTip_TopPanelPriorities         = 20,
    ToolTip_PriorityDisable            = 21,
    ToolTip_PriorityUpOne              = 22,
    ToolTip_PriorityReset              = 23,
    ToolTip_CamControlZoomIn           = 24,
    ToolTip_CamControlZoomOut          = 25,
    ToolTip_CamControlCycle            = 26,
    ToolTip_CamControlRotate           = 27,
    ToolTip_SideBar_Ore                = 28,
    ToolTip_SideBar_Crystals           = 29,
    ToolTip_Close                      = 30,
    ToolTip_PreviousMessage            = 31,
    ToolTip_NextMessage                = 32,
    ToolTip_More                       = 33,
    ToolTip_Back                       = 34,
    ToolTip_CamControlCycleMinifigures = 35,
    ToolTip_Reward_Save                = 36,
    ToolTip_Reward_Advance             = 37,
    ToolTip_FrontEnd_Back              = 38,
    ToolTip_Type_Count                 = 39,
} ToolTip_Type;

typedef enum MenuOverlay_Type
{
    MenuOverlay_Type_Flic      = 0,
    MenuOverlay_Type_Animation = 1,
    MenuOverlay_Type_Image     = 2,
    MenuOverlay_Type_Lws       = 3,
    MenuOverlay_Type_Count     = 4,
    MenuOverlay_Type_Invalid   = -1,
} MenuOverlay_Type;

typedef struct MenuOverlay
{
    const char* filename;
    MenuOverlay_Type overlayType;
    U32 field_8;
    Point2I position;
    SFX_ID sfxType;
    struct MenuOverlay* previous;
    U32 field_1c;
} MenuOverlay, *lpMenuOverlay;

typedef struct MenuItem_CycleData
{
    void* temp;
    // TODO: Implement MenuItem_CycleData
} MenuItem_CycleData, *lpMenuItem_CycleData;

typedef void (*MenuItem_TriggerCallback)();

typedef struct MenuItem_TriggerData
{
    B32* valuePtr;
    B32 end; // (cfg: Trigger:[4]) End/close the current MenuSet
    MenuItem_TriggerCallback callback;
} MenuItem_TriggerData, *lpMenuItem_TriggerData;

typedef struct MenuItem_TextInputData
{
    void* temp;
    // TODO: Implement MenuItem_TextInputData
} MenuItem_TextInputData, *lpMenuItem_TextInputData;

typedef struct MenuItem_SliderData
{
    void* temp;
    // TODO: Implement MenuItem_SliderData
} MenuItem_SliderData, *lpMenuItem_SliderData;

typedef struct MenuItem_RealSliderData
{
    void* temp;
    // TODO: Implement MenuItem_RealSliderData
} MenuItem_RealSliderData, *lpMenuItem_RealSliderData;

typedef void (*MenuItem_SelectCallback)(F32 elapsedAbs, S32 selectIndex);

typedef enum MenuItem_SelectItemFlags
{
    SELECTITEM_FLAG_NONE        = 0,
    SELECTITEM_FLAG_HASBANNER   = 0x1,
    SELECTITEM_FLAG_HASIMAGE    = 0x2,
    SELECTITEM_FLAG_ENABLED     = 0x4,
} MenuItem_SelectItemFlags;

typedef enum MenuItem_SelectImageType
{
    MenuItem_SelectImage_Light  = 0,
    MenuItem_SelectImage_Dark   = 1,
    MenuItem_SelectImage_Locked = 2,
    MenuItem_SelectImage_Count  = 3,
} MenuItem_SelectImageType;

typedef struct MenuItem_SelectItem
{
    MenuItem_SelectItemFlags flags;
    char* banner;
    lpImage images[3];
    S32 frontEndX;
    S32 frontEndY;
    B32 frontEndOpen;
} MenuItem_SelectItem, *lpMenuItem_SelectItem;

typedef struct MenuItem_SelectData
{
    lpMenuItem_SelectItem selItemList;
    U32* widths[MenuItem_SelectImage_Count]; // image (Hi, Lo, Locked) widths
    U32* heights[MenuItem_SelectImage_Count]; // image (Hi, Lo, Locked) heights
    U32 selItemCount;
    const char* string1;
    const char* string2;
    S32* valuePtr;
    S32 x2, y2;
    S32 selItemHeight;
    S32 scrollCount; // Max number of select items visible in scroll area.
    S32 xString1, yString1;
    S32 xString2, yString2;
    S32 scrollStart; // Index offset of visible scroll area
    U32 field_50;
    MenuItem_SelectCallback callback;
    struct Menu* nextMenu;
} MenuItem_SelectData, *lpMenuItem_SelectData;

typedef struct MenuItem
{
    const char* banner;
    U32 length;
    lpFont fontHi, fontLo;
    union MenuItem_Data_union{
        MenuItem_CycleData* cycle;
        MenuItem_TriggerData* trigger;
        MenuItem_TextInputData* textInput;
        MenuItem_SliderData* slider;
        MenuItem_RealSliderData* realSlider;
        MenuItem_SelectData* select;
        struct Menu* next;
        void* data;
    } itemData;
    MenuItem_Type itemType;
    S32 x1, y1;
    S32 centerOffLo, centerOffHi;
    B32 isImageItem;
    lpImage imageLo, imageHi;
    ToolTip_Type toolTipType;
    B32 notInTuto;
} MenuItem, *lpMenuItem;

typedef struct Menu
{
    const char* title;
    const char* fullName;
    U32 titleLength;
    lpFont menuFont;
    lpImage menuImage;
    lpImage menuImageDark;
    lpMenuItem* items;
    S32 itemCount;
    S32 itemCapacity;
    S32 itemFocus;
    B32 closed;
    Point2I position;
    MenuFlags flags;
    Point2F currPosition;
    lpMenuOverlay overlays;
    B32 autoCenter;
    B32 displayTitle;
    B32 anchored;
    Point2I anchoredPosition;
    S32 centerX;
    char name[64];
    Bool3 playRandom;
} Menu, *lpMenu;

typedef struct MenuSet
{
    lpMenu* menus;
    U32 menuCount;
} MenuSet, *lpMenuSet;

typedef struct LevelLink
{
    S32 setIndex;
    struct LevelLink** linkLevels;
    U32 linkCount;
    U32 field_c;
    B32 visited;
} LevelLink, *lpLevelLink;

typedef B32 (* LevelLink_RunThroughLinksCallback)(lpLevelLink link, void* data);

typedef struct LevelSet
{
    S32 count;
    char** idNames;
    char** langNames;
    LevelLink** levels;
    B32* visitedList;
} LevelSet, *lpLevelSet;

typedef enum Front_RockWipeFlags
{
    ROCKWIPE_FLAG_NONE = 0,
    ROCKWIPE_FLAG_ANIMATING = 0x1,
    ROCKWIPE_FLAG_NOINPUT = 0x2,
} Front_RockWipeFlags;

typedef struct MenuTextWindow
{
    lpTextWindow textWindow;
    lpImage PanelImage;
    Area2F WindowArea;
    Area2F PanelArea;
    char LoadText[256];
    char SaveText[256];
    char SlotText[256];
    char LoadSelText[256];
    char SaveSelText[256];
    char LevelText[256];
    char TutorialText[256];
} MenuTextWindow, *lpMenuTextWindow;

typedef struct Front_Cache
{
    const char* path;
    lpImage image;
    lpFont font;
    struct Front_Cache* next;
} Front_Cache, *lpFront_Cache;

extern lpFront_Cache g_ImageCache_NEXT;

typedef enum Front_MenuVars
{
    MENUVAR_TRIGGER_CREDITS = 0,
    MENUVAR_UNKNOWN1 = 1,
    MENUVAR_UNKNOWN2 = 2,
    MENUVAR_TRIGGER_QUITAPP = 3,
    MENUVAR_SELECT_MISSION_INDEX = 4,
    MENUVAR_SELECT_TUTORIAL_INDEX = 5,
    MENUVAR_SELECT_LOADSAVE_INDEX = 6,
    MENUVAR_TRIGGER_CONTINUE_MISSION = 7,
    MENUVAR_SLIDER_GAMESPEED = 8,
    MENUVAR_SLIDER_SFXVOLUME = 9,
    MENUVAR_SLIDER_MUSICVOLUME = 10,
    MENUVAR_SLIDER_BRIGHTNESS = 11,
    MENUVAR_CYCLE_HELPWINDOW = 12,
    MENUVAR_TRIGGER_REPLAYOBJECTIVE = 13,
    MENUVAR_TRIGGER_QUITMISSION = 14,
    MENUVAR_TRIGGER_RESTARTMISSION = 15,
    MENUVAR_CYCLE_WALLDETAIL = 16,
    MENUVAR_CYCLE_MUSICON = 17,
    MENUVAR_CYCLE_SFXON = 18,
    MENUVAR_CYCLE_AUTOGAMESPEED = 19,
    MENUVAR_TRIGGER_BACKSAVE = 20,

    MENUVAR_COUNT = 21,
} Front_MenuVars;

typedef struct Front_Globs
{
    lpMenuSet pausedMenuSet;
    lpMenuSet mainMenuSet;
    lpMenuSet optionsMenuSet;
    lpMenuSet saveMenuSet;
    LevelSet tutorialLevels;
    LevelSet missionLevels;
    lpLevelLink startMissionLink;
    lpLevelLink startTutorialLink;
    U32 reserved1[4];
    S32 menuVars[MENUVAR_COUNT];
    Point2I overlayPosition;
    lpFlic overlayImageOrFlic;
    U32 overlayStartTime;
    U32 overlayCurrTime;
    lpFont versionFont;
    const char* versionString;
    SaveData saveData[6];
    S32 saveNumber;
    const char* strDefaultLevelBMPS;
    U32 reserved3;
    Point2I scrollOffset;
    U32 reserved4[2];
    lpContainer rockWipeAnim;
    Front_RockWipeFlags rockWipeFlags;
    F32 rockWipeSFXTimer;
    F32 rockWipeSFXStartTime;
    lpContainer rockWipeLight;
    B32 saveMenuHasNoData;
    B32 saveMenuHasSaved;
    B32 saveMenuKeepOpen;
    Size2I saveImageBigSize;
    char langLoadGame[64];
    char langSaveGame[64];
    lpMenuTextWindow saveTextWnd;
    lpMenuTextWindow saveLevelWnd;
    char langOverwriteTitle[128];
    char langOverwriteMessage[256];
    char langOverwriteOK[128];
    char langOverwriteCancel[128];
    B32 shouldClearUnlockedLevels;
    U32 reserved5;
    U32 unused_zero_864; // (init: 0) Set to 0 and never touched?
    S32 maxLevelScreens;
    U32 reserved6[2];
    U32 levelSelectHoverNumber;
    U32 levelSelectLastNumber;
    B32 levelSelectSFXStopped;
    F32 levelSelectSFXTimer;
} Front_Globs;

Front_Globs frontGlobs;

extern B32 Front_IsFrontEndEnabled();
extern B32 Front_IsIntrosEnabled();

extern void Front_PlayIntroMovie(const char* aviKey, B32 skippable);
extern void Front_PlayIntroSplash(const char* imageKey, B32 skippable, const char* timeKey);

extern void Front_PlayLevelMovie(const char* levelName, B32 skippable);

///    Plays an already-loaded Movie_t from the G98CMovie C wrapper API.
///    Allows terminating the movie playback during runtime with isSkippable.
extern void Front_PlayMovie(Movie_t* mov, B32 skippable);

extern void Front_Initialize(lpConfig config);

extern void Front_ResetSaveNumber();

extern void Front_RunScreenMenuType(Menu_ScreenType screenType);
extern void Front_RunScreenMenu(lpMenuSet menuSet, U32 menuIndex);
extern void Front_PrepareScreenMenuType(Menu_ScreenType screenType);
extern void Front_ScreenMenuLoop(lpMenu menu);
extern lpMenu Front_Menu_Update(F32 elapsed, lpMenu menu, B32 *menuTransition);
extern B32 Front_IsTriggerAppQuit();

extern lpMenu Front_Menu_UpdateMenuItemsInput(F32 elapsed, lpMenu menu);
extern void Front_Menu_UpdateMousePosition(lpMenu menu);

extern void Front_Menu_UpdateOverlays(lpMenu menu);
extern B32 Front_Menu_ShouldRandomPlay();
extern U32 Front_Menu_GetOverlayCount(lpMenu menu);

extern void Front_Menu_DrawMenuImage(lpMenu menu, B32 light);
extern void Front_Menu_DrawLoadSaveText(lpMenu* pMenu, lpMenu* currMenu, lpMenu* nextMenu);

extern B32 Front_Menu_AddMenuItem(lpMenu menu, lpMenuItem menuItem);

extern B32 Front_Menu_FindItemUnderMouse(lpMenu menu, S32* itemIndex);
extern B32 Front_Menu_GetItemBounds(lpMenu menu, S32 itemIndex, S32* rcX, S32* rcY, S32* rcWidth, S32* rcHeight);
extern B32 Front_Menu_IsLevelItemUnderMouse(lpMenu menu, S32 itemIndex);

extern void Front_MenuItem_DrawSelectTextWindow(lpMenu* menu);
extern B32 Front_MenuItem_CheckNotInTutoAnyTutorialFlags(lpMenuItem menuItem);
extern MenuItem_Type Front_MenuItem_ParseTypeString(const char* itemTypeName);
extern lpMenuItem Front_MenuItem_CreateBannerItem(const char* banner, lpFont loFont, lpFont hiFont, S32 x1, S32 y1, MenuItem_Type itemType, B32 centered, void* itemData, B32 notInTutorial);
extern lpMenuItem Front_MenuItem_CreateImageItem(const char* banner, lpFont loFont, lpFont hiFont, const char* loImageName, const char* hiImageName, S32 x1, S32 y1, MenuItem_Type itemType, B32 centered, const char* toolTipName, void* itemData);
extern lpMenuItem_TriggerData Front_MenuItem_CreateTrigger(B32* valuePtr, B32 end, MenuItem_TriggerCallback callback);
extern lpMenuItem_SelectData Front_MenuItem_CreateSelect(S32* valuePtr, const char* string1, const char* string2, S32 x2, S32 y2, S32 selItemHeight, S32 scrollCount, S32 xString1, S32 yString1, S32 xString2, S32 yString2, S32 field50, MenuItem_SelectCallback callback, lpMenu nextMenu);

// To create a banner item, pass a string to bannerOrBMPName that does NOT contain
// ".bmp" (case-insensitive), font must also be non-NULL for banner items.
extern void Front_MenuItem_AddSelectItem(lpMenuItem_SelectData selectData, const char* bannerOrBMPName, B32 enabled, lpFont hiFont, S32 frontEndX, S32 frontEndY, B32 frontEndOpen);

extern S32 Front_MenuItem_Select_TestStringCollision(lpMenu menu, lpMenuItem menuItem, lpMenuItem_SelectData selectData);

extern void Front_MenuItem_DrawSaveImage(lpMenu menu, S32 selIndex, lpMenuItem_SelectData selectData, B32 bigSize);
extern void Front_MenuItem_DrawSelectItem(S32 x, S32 y, lpFont font, lpMenuItem_SelectData selectData, U32 selIndex, MenuItem_SelectImageType imageType);

extern void Front_RockWipe_Play();
extern void Front_RockWipe_Stop();

extern const char* Front_GetSelectedLevel();
extern B32 Front_IsMissionSelected();
extern B32 Front_IsTutorialSelected();
extern void Front_LoadOptionParameters(B32 loadOptions, B32 resetFront);

extern void Front_LoadMenuTextWindow(lpConfig config, const char* configPath, lpMenuTextWindow menuWnd);

extern lpMenuSet Front_LoadMenuSet(lpConfig config, const char* menuName, ...);
extern B32 Front_LoadLevelSet(lpConfig config, lpLevelSet levelSet, const char* levelKey);

extern lpMenuSet Front_CreateMenuSet(U32 menuCount);

extern lpMenu Front_Menu_CreateMenu(const char* title, const char* fullName, lpFont menuFont, S32 positionX, S32 positionY, B32 autoCenter, B32 dislayTitle, S32 centerX, B32 canScroll, const char* anchored);
extern void Front_Menu_FreeMenu(lpMenu menu);

extern B32 Front_Menu_LoadMenuImage(lpMenu menu, const char* filename, B32 light);

extern lpMenuOverlay Front_Menu_CreateOverlay(const char* filename, lpMenuOverlay* linkedOverlay, S32 positionX, S32 positionY, SFX_ID sfxType);
extern MenuOverlay_Type Front_Menu_GetOverlayType(lpMenuOverlay menuOverlay);

extern void Front_LoadLevels(lpMenuSet mainMenuFull);
extern lpLevelLink Front_LevelSet_LoadLevelLinks(lpLevelSet levelSet, const char* levelName);
extern B32 Front_LevelSet_IsLinkVisited(lpLevelSet levelSet, const char* levelName);
extern void Front_LevelSet_SetLinkVisited(lpLevelSet levelSet, const char* levelName, B32 visited);
extern lpLevelLink Front_LevelSet_GetLevelLink(lpLevelSet levelSet, const char* levelName);
extern void Front_LevelSet_SetLevelLink(lpLevelSet levelSet, const char* levelName, lpLevelLink link);
extern S32 Front_LevelSet_IndexOf(lpLevelSet levelSet, const char* levelName);
extern void Front_Levels_ResetVisited();
extern B32 Front_LevelLink_RunThroughLinks(lpLevelLink startLink, LevelLink_RunThroughLinksCallback callback, void* data);
extern S32 Front_LevelLink_FindSetIndexOf(lpLevelLink startLink, S32 linkIndex);
extern lpLevelLink Front_LevelLink_FindByLinkIndex(lpLevelLink startLink, S32 linkIndex);
extern void Front_Levels_UpdateAvailable(lpLevelLink startLink, lpSaveReward saveReward, lpLevelSet levelSet, lpMenuItem_SelectData selectData, B32 keepLocked);
struct SearchLevelSelectInfo_14;
extern void Front_Levels_UpdateAvailable_Recursive(lpLevelLink link, struct SearchLevelSelectInfo_14* search, B32 unlocked);
extern void MainMenuFull_AddMissionsDisplay(Front_MenuVars menuVar, lpLevelLink startLink, lpLevelSet levelSet, lpMenu menu, lpSaveData saveData, lpMenu nextMenu, void* callback);

extern S32 Front_GetMenuIDByName(lpMenuSet menuSet, const char* name);

extern void Front_LevelSelect_LevelNamePrintF(lpFont font, S32 x, S32 y, const char* msg, ...);

extern void Front_Callback_TriggerPlayCredits();
extern void Front_Callback_TriggerBackSave();
extern void Front_Callback_SliderGameSpeed(S32 slider_0_5);
extern void Front_Callback_SliderSoundVolume(S32 slider_0_10);
extern void Front_Callback_SliderMusicVolume(S32 slider_0_10);
extern void Front_Callback_SliderBrightness(S32 slider_0_10);
extern void Front_Callback_CycleHelpWindow(S32 cycle_On_Off);
extern void Front_Callback_TriggerReplayObjective();
extern void Front_Callback_CycleWallDetail(S32 cycle_High_Low);
extern void Front_Callback_CycleMusic(S32 cycle_On_Off);
extern void Front_Callback_CycleSound(S32 cycle_On_Off);
extern void Front_Callback_CycleAutoGameSpeed(S32 cycle_On_Off);
extern void Front_Callback_SelectMissionItem(F32 elapsedAbs, S32 selectIndex);
extern void Front_Callback_SelectTutorialItem(F32 elapsedAbs, S32 selectIndex);

extern B32 Front_LevelSelect_PlayLevelNameSFX(S32 levelNumber);
extern B32 Front_LevelSelect_PlayTutoLevelNameSFX(S32 levelNumber);

extern void Front_Callback_SelectLoadSave(F32 elapsedAbs, S32 selectIndex);

extern B32 Front_LevelLink_Callback_IncCount(lpLevelLink link, void* pCount);
extern B32 Front_LevelInfo_Callback_AddItem(lpLevelLink link, void* data);

extern B32 Front_LevelLink_Callback_FindByLinkIndex(lpLevelLink link, void* data);

extern S32 Front_CalcSliderGameSpeed();
extern S32 Front_CalcSliderCDVolume();

extern void Front_UpdateOptionsSliders();
extern void Front_UpdateSliderGameSpeed();

extern B32 Front_Options_Update(F32 elapsed, Menu_ModalType modalType);

extern void Front_Save_SetShouldClearUnlockedLevels(B32 state);
extern void Front_Save_GetLevelCompleteWithPoints(lpSaveData saveData, char* buffer);
extern void Front_Save_LoadAllSaveFiles();
extern S32 Front_Save_GetLevelScore(U32 index, lpSaveData saveData);
extern S32 Front_Save_GetSaveNumber();

extern S32 Front_SaveMenu_ConfirmMessage_FUN_004354f0(const char* titleText, const char* message, const char* okText, const char* cancelText);

extern lpFront_Cache Front_Cache_Create(const char* filename);
extern lpFront_Cache Front_Cache_FindByName(const char* filename);
extern lpFont Front_Cache_LoadFont(const char* filename);
extern lpImage Front_Cache_LoadImage(const char* filename);

extern void Front_LoadSaveSlotImages();
extern void Front_FreeSaveSlotImages();

extern lpSaveData Front_Save_GetCurrentSaveData();

extern const char* Front_Util_StringReplaceChar(const char* str, char origChar, char newChar);
extern const char* Front_Util_ReplaceTextSpaces(const char* str);
extern const char* Front_Util_StrCpy(const char* str);

extern B32 Front_Maths_IsPointInsideRect(S32 ptX, S32 ptY, S32 rcX, S32 rcY, S32 rcWidth, S32 rcHeight);
extern B32 Front_Maths_IsPointInsideRectCentered(S32 ptX, S32 ptY, S32 rcX, S32 rcY, S32 rcWidth, S32 rcHeight, B32 shouldCenterX);

extern B32 Front_GetMousePressedState();

extern S32 g_frontSelectHoverIndex;
extern B32 g_saveMenuOverwriteShowing;
extern lpMenu g_saveMenu_UnkNextMenu;
extern S32 g_saveMenuSelectedIndex;
extern S32 g_saveMenuOverwriteResult;
extern S32 g_saveMenuOverlayState;
extern S32 g_saveMenuSelectingIndex;
extern S32 g_saveMenuOutputSelectedIndex;
extern B32 g_saveMenuOverlayPlaying;
extern B32 g_levelSelectPrinting;
