#pragma once

#include "types.h"
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
} MenuItem_CycleData;

typedef struct MenuItem_TriggerData
{
    void* temp;
    // TODO: Implement MenuItem_TriggerData
} MenuItem_TriggerData;

typedef struct MenuItem_TextInputData
{
    void* temp;
    // TODO: Implement MenuItem_TextInputData
} MenuItem_TextInputData;

typedef struct MenuItem_SliderData
{
    void* temp;
    // TODO: Implement MenuItem_SliderData
} MenuItem_SliderData;

typedef struct MenuItem_RealSliderData
{
    void* temp;
    // TODO: Implement MenuItem_RealSliderData
} MenuItem_RealSliderData;

typedef struct MenuItem_SelectData
{
    void* temp;
    // TODO: Implement MenuItem_SelectData
} MenuItem_SelectData;

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
    ROCKWIPE_FLAG_UNK_1 = 0x1,
    ROCKWIPE_FLAG_UNK_2 = 0x2,
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
    U64 reserved1a;
    U64 reserved1b;
    S32 triggerCredits;
    U64 reserved2;
    S32 triggerQuitApp;
    S32 selectMissionIndex;
    S32 selectTutorialIndex;
    S32 selectLoadSaveIndex;
    S32 triggerContinueMission;
    S32 sliderGameSpeed;
    S32 sliderSFXVolume;
    S32 sliderMusicVolume;
    S32 sliderBrightness;
    S32 cycleHelpWindow;
    S32 triggerReplayObjective;
    S32 triggerQuitMission;
    S32 triggerRestartMission;
    S32 cycleWallDetail;
    S32 cycleMusicOn;
    S32 cycleSFXOn;
    S32 cycleAutoGameSpeed;
    S32 triggerBackSave;
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
    B32 saveBool_540;
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
    B32 saveBool_85c;
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

extern void Front_Menu_UpdateMousePosition(lpMenu menu);

extern void Front_Menu_DrawMenuImage(lpMenu menu, B32 light);

extern void Front_RockWipe_Play();
extern void Front_RockWipe_Stop();

extern const char* Front_GetSelectedLevel();
extern void Front_LoadOptionParameters(B32 loadOptions, B32 resetFront);

extern void Front_LoadMenuTextWindow(lpConfig config, const char* gameName, lpMenuTextWindow menuWnd);

extern lpMenuSet Front_LoadMenuSet(lpConfig config, const char* menuName, ...);
extern B32 Front_LoadLevelSet(lpConfig config, lpLevelSet levelSet, const char* levelKey);

extern lpMenuSet Front_CreateMenuSet(U32 menuCount);

extern lpMenu Front_Menu_CreateMenu(const char* title, const char* fullName, lpFont menuFont, S32 positionX, S32 positionY, B32 autoCenter, B32 dislayTitle, S32 centerX, B32 canScroll, const char* anchored);
extern void Front_Menu_FreeMenu(lpMenu menu);

extern B32 Front_Menu_LoadMenuImage(lpMenu menu, const char* filename, B32 light);

extern MenuItem_Type Front_MenuItem_ParseTypeString(const char* itemTypeName);

extern lpMenuOverlay Front_Menu_CreateOverlay(const char* filename, lpMenuOverlay* linkedOverlay, S32 positionX, S32 positionY, SFX_ID sfxType);

extern void Front_LoadLevels(lpMenuSet mainMenuFull);

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

extern void Front_Callback_SelectLoadSave(F32 elapsedAbs, S32 selectIndex);

extern S32 Front_CalcSliderGameSpeed();
extern S32 Front_CalcSliderCDVolume();

extern void Front_Save_SetBool_85c(B32 state);

extern lpFront_Cache Front_Cache_Create(const char* filename);
extern lpFront_Cache Front_Cache_FindByName(const char* filename);
extern lpFont Front_Cache_LoadFont(const char* filename);
extern lpImage Front_Cache_LoadImage(const char* filename);

extern void Front_LoadSaveSlotImages();
extern void Front_FreeSaveSlotImages();

extern const char* Front_Util_StringReplaceChar(const char* str, char origChar, char newChar);
extern const char* Front_Util_ReplaceTextSpaces(const char* str);
extern const char* Front_Util_StrCpy(const char* str);
