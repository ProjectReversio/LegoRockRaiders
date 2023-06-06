#pragma once

#include "types.h"
#include "movie.h"
#include "config.h"
#include "fonts.h"
#include "sfx.h"

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

typedef struct Front_Globs
{
    lpMenuSet pausedMenuSet;
    lpMenuSet mainMenuSet;
    lpMenuSet optionsMenuSet;
    lpMenuSet saveMenuSet;

    // TODO: Implement Front_Globs
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
extern B32 Front_IsTriggerAppQuit();

extern const char* Front_GetSelectedLevel();
extern void Front_LoadOptionParameters(B32 loadOptions, B32 resetFront);

extern const char* Front_Util_StringReplaceChar(const char* str, char origChar, char newChar);
