#pragma once

#include "platform.h"
#include "config.h"
#include "main.h"
#include "maths.h"
#include "container.h"
#include "viewport.h"
#include "lego_camera.h"
#include "fonts.h"
#include "text_window.h"
#include "lego_object.h"
#include "sfx.h"
#include "common.h"
#include "select_place.h"
#include "vehicle.h"
#include "creature.h"
#include "building.h"
#include "upgrade.h"
#include "map3d.h"
#include "nerps.h"
#include "radarmap.h"

typedef enum TextureType
{
    TEXTURES_ROCK = 0,
    TEXTURES_LAVA = 1,
    TEXTURES_ICE = 2,
    TEXTURES_INVALID = -1,
} TextureType;

typedef enum LevelStatus
{
    LEVELSTATUS_INCOMPLETE = 0,
    LEVELSTATUS_COMPLETE = 1,
    LEVELSTATUS_FAILED = 2,
    LEVELSTATUS_FAILED_CRYSTALS = 3,
    LEVELSTATUS_FAILED_OTHER = 4,
} LevelStatus;

typedef enum BlockFlags1
{
    BLOCK1_NONE                = 0,
    BLOCK1_RUBBLE_LOW          = 0x1,
    BLOCK1_RUBBLE_MEDIUM       = 0x2,
    BLOCK1_RUBBLE_FULL         = 0x3,
    BLOCK1_SURVEYED            = 0x4,
    BLOCK1_FLOOR               = 0x8,
    BLOCK1_WALL                = 0x10,
    BLOCK1_REINFORCED          = 0x20,
    BLOCK1_INCORNER            = 0x40,
    BLOCK1_OUTCORNER           = 0x80,
    BLOCK1_HIGHPOLY            = 0x100,
    BLOCK1_NOTHOT              = 0x200,
    BLOCK1_BUILDINGSOLID       = 0x400,
    BLOCK1_ROCKFALLFX          = 0x800,
    BLOCK1_LANDSLIDING         = 0x1000,
    BLOCK1_DIAGONAL            = 0x2000,
    BLOCK1_SMOKE               = 0x4000,
    BLOCK1_BUILDINGPATH        = 0x8000,
    BLOCK1_HIDDEN              = 0x20000,
    BLOCK1_DESTROYEDCONNECTION = 0x40000,
    BLOCK1_BUSY_FLOOR          = 0x80000,
    BLOCK1_FOUNDATION          = 0x100000,
    BLOCK1_CLEARED             = 0x200000,
    BLOCK1_EXPOSEDFLOORCHECKS  = 0x400000,
    BLOCK1_DIGREQUEST          = 0x800000,
    BLOCK1_UNUSED_PATHTYPE     = 0x1000000,
    BLOCK1_EXPOSED             = 0x4000000,
    BLOCK1_BUSY_WALL           = 0x8000000,
    BLOCK1_DOZERCLEARING       = 0x10000000,
    BLOCK1_PATH                = 0x20000000,
    BLOCK1_LAYEDPATH           = 0x40000000,
    BLOCK1_ERODEACTIVE         = 0x80000000,
} BlockFlags1;

typedef enum BlockFlags2
{
    BLOCK2_NONE              = 0,
    BLOCK2_GENERATEPOWER     = 0x1,
    BLOCK2_DRAINPOWER_TEMP   = 0x2,
    BLOCK2_TOOLSTORE         = 0x4,
    BLOCK2_FENCEREQUEST      = 0x8,
    BLOCK2_ERODELOCKED       = 0x10,
    BLOCK2_SLUGHOLE_EXPOSED  = 0x20,
    BLOCK2_EMERGE_POINT      = 0x40,
    BLOCK2_EMERGE_TRIGGER    = 0x80,
    BLOCK2_POWERED           = 0x100,
    BLOCK2_SLUGHOLE_HIDDEN   = 0x200,
    BLOCK2_PATHSBUILTCOUNTED = 0x400,
} BlockFlags2;

typedef enum Lego_PredugType // : U8
{
    Lego_PredugType_Wall             = 0,
    Lego_PredugType_Cavern_Exposed   = 1,
    Lego_PredugType_Cavern_Hidden    = 2,
    Lego_PredugType_SlugHole_Exposed = 3,
    Lego_PredugType_SlugHole_Hidden  = 4,
} Lego_PredugType;

typedef enum Lego_SurfaceType8 // : U8
{
    Lego_SurfaceType8_Tunnel        = 0,
    Lego_SurfaceType8_Immovable     = 1,
    Lego_SurfaceType8_Hard          = 2,
    Lego_SurfaceType8_Medium        = 3,
    Lego_SurfaceType8_Loose         = 4,
    Lego_SurfaceType8_Soil          = 5,
    Lego_SurfaceType8_Lava          = 6,
    Lego_SurfaceType8_Water         = 7,
    Lego_SurfaceType8_OreSeam       = 8,
    Lego_SurfaceType8_Lake          = 9,
    Lego_SurfaceType8_CrystalSeam   = 10,
    Lego_SurfaceType8_RechargeSeam  = 11,
    Lego_SurfaceType8_Rubble        = 12,
    Lego_SurfaceType8_Reinforcement = 13,
    Lego_SurfaceType8_Path          = 14,
    Lego_SurfaceType8_SlugHole      = 15,
    Lego_SurfaceType8_Undiscovered  = 16,
    Lego_SurfaceType8_Cavern        = 17,
} Lego_SurfaceType8;

typedef enum Lego_CryOreType
{
    Lego_CryOreType_None           = 0,
    Lego_CryOreType_Crystal_Lv0_1  = 1,
    Lego_CryOreType_Ore_Lv0_1      = 2,
    Lego_CryOreType_Crystal_Lv1_1  = 3,
    Lego_CryOreType_Ore_Lv1_1      = 4,
    Lego_CryOreType_Crystal_Lv0_3  = 5,
    Lego_CryOreType_Ore_Lv0_3      = 6,
    Lego_CryOreType_Crystal_Lv1_3  = 7,
    Lego_CryOreType_Ore_Lv1_3      = 8,
    Lego_CryOreType_Crystal_Lv0_5  = 9,
    Lego_CryOreType_Ore_Lv0_5      = 10,
    Lego_CryOreType_Crystal_Lv1_5  = 11,
    Lego_CryOreType_Ore_Lv1_5      = 12,
    Lego_CryOreType_Crystal_Lv0_10 = 13,
    Lego_CryOreType_Ore_Lv0_10     = 14,
    Lego_CryOreType_Crystal_Lv1_10 = 15,
    Lego_CryOreType_Ore_Lv01_10    = 16,
    Lego_CryOreType_Crystal_Lv0_25 = 17,
    Lego_CryOreType_Ore_Lv0_25     = 18,
    Lego_CryOreType_Crystal_Lv1_25 = 19,
    Lego_CryOreType_Ore_Lv1_25     = 20,
} Lego_CryOreType;

typedef enum Lego_ErodeType
{
    Lego_ErodeType_None              = 0,
    Lego_ErodeType_Adjacent_VerySlow = 1,
    Lego_ErodeType_Source_VerySlow   = 2,
    Lego_ErodeType_Adjacent_Slow     = 3,
    Lego_ErodeType_Source_Slow       = 4,
    Lego_ErodeType_Adjacent_Medium   = 5,
    Lego_ErodeType_Source_Medium     = 6,
    Lego_ErodeType_Adjacent_Fast     = 7,
    Lego_ErodeType_Source_Fast       = 8,
    Lego_ErodeType_Adjacent_VeryFast = 9,
    Lego_ErodeType_Source_VeryFast   = 10,
} Lego_ErodeType;

typedef struct Lego_BlockActivity
{
    lpContainer cont;
    Point2I blockPos;

    // TODO: Implement Lego_BlockActivity

    struct Lego_BlockActivity* next;
    struct Lego_BlockActivity* previous;
    B32 staticEffect; // (0 = reinforcement, 1 = recharge seam twinkle?)

} Lego_BlockActivity, *lpLego_BlockActivity;

typedef struct Lego_Block
{
    Lego_PredugType predug;
    SurfaceTexture texture;
    Lego_SurfaceType8 terrain;
    U8 direction; // clockwise (does not determine corner/wall type)
    U8 blockpointer;
    Lego_CryOreType cryOre;
    Lego_ErodeType erodeSpeed;
    U8 erodeLevel; // 0 = low, 1 = med, 2 = high, 3 = max, 4 = lava
    BlockFlags1 flags1;
    BlockFlags2 flags2;

    // TODO: Implement Lego_Block

    F32 damage;
    Lego_BlockActivity* activity;

    // TODO: Implement Lego_Block

    S16 randomness; // (sometimes sign-extended)
    U16 seamDigCount; // Number of times a seam has been dug (destroyed at 4).
    U32 numLandSlides;
    U32 clickCount;
    S32 generateCrystals;
    S32 generateOre;
    U8 aiNode;

    // TODO: Implement Lego_Block

    B32 fallinUpper; // (fallin upper: 1 if fallin > 4)
    S32 fallinIntensity; // (fallin scale: 1-4)
    F32 fallinTimer; // (randomized with full fallin value)
    B32 tutoHighlightState; // Tutorial block highlight color (false = tutorial color, true = normal)

} Lego_Block, *lpLego_Block;

typedef struct Lego_Level
{
    char* name; // (format: "Levels::level")
    lpMap3D map;

    // TODO: Implement Lego_Level

    U32 emergeCount;
    U32 unused_rewardID; // Presumably used to lookup level rewards by some constant ID value.
    U32 width;
    U32 height;
    F32 BlockSize;
    F32 DigDepth;
    F32 RoofHeight;
    F32 RoughLevel;
    Bool3 UseRoof;
    Bool3 SafeCaverns;
    F32 SelBoxHeight;

    // TODO: Implement Lego_Level

    lpRadarMap radarMap;
    Lego_BlockActivity* blockActLast;
    S32 crystals; // (init: 0) Total number of Crystals stored.
    U32 unused_crystals_90; // (init: 0, unused) Presumably once for Crystals.
    S32 crystalsPickedUp; // (init: 0) Used by NERPs:GetCrystalsPickedUp.
    S32 crystalsDrained; // Crystals stored, but in-use powering buildings.
    S32 crystalsStolen; // For checking MaxStolen failure condition.
    S32 ore; // (init: 0) Total number of normal Ore stored. (EXCLUDES STUDS)
    U32 unused_ore_a4; // (init: 0, unused) Presumably once for Ore.
    S32 orePickedUp; // (init: 0) Used by NERPs:GetOrePickedUp.
    S32 studs; // (init: 0) Total number of Studs stored. (1 stud == 5 ore)
    S32 EmergeCreature; // (searches for Object ID by name, expects RockMonsterType)

    char* nextLevelID; // (cfg: NextLevel)
    lpLego_Block blocks;

    // TODO: Implement Lego_Level

    B32 hasBlockPointers;
    BlockPointer* blockPointers[56];
    B32 StartFP;
    B32 NoDrain;
    F32 oxygenLevel; // (init: 100.0, Oxygen level?)
    F32 OxygenRate;
    F32 oxygenLevelMessage; // Tracks change in oxygen since the last message (new message after change of 5%).
    F32 BuildingTolerance;
    F32 BuildingMaxVariation;
    F32 UpgradeTimes[20]; // [objType] (mult: 25.0, 1:Vehicle, 2:MiniFigure, 3:Building)
    F32 TrainTime; // (mult: 25.0)
    F32 EmergeTimeOut; // (default: 1500.0)
    F32 SlugTime; // (default: 60.0, mult: 25.0 (applies to default))
    S32 Slug; // (default: 20 (invalid), searches for object index by name, expects RockMonsterType)
    char* FullName; // (replace '_' with ' ')
    TextureType BoulderAnimation; // (texture index, hardcoded: Rock, Lava, Ice)
    F32 MaxStolen;
    LevelStatus status; // (init: 0) 2 is used for Crystal failure as well
    B32 IsStartTeleportEnabled;
} Lego_Level, *lpLego_Level;

typedef enum GameFlags1
{
    GAME1_NONE                  = 0,
    GAME1_RADARON               = 0x2,
    GAME1_LEVELSTART            = 0x4,
    GAME1_USEMUSIC              = 0x8,
    GAME1_USESFX                = 0x10,
    GAME1_USEDETAIL             = 0x20,
    GAME1_SHOWFPS               = 0x40,
    GAME1_SHOWMEMORY            = 0x80,
    GAME1_MULTISELECT           = 0x100,
    GAME1_UNK_200               = 0x200,
    GAME1_DDRAWCLEAR            = 0x400,
    GAME1_RENDERPANELS          = 0x800,
    GAME1_RADAR_MAPVIEW         = 0x1000,
    GAME1_RADAR_TRACKOBJECTVIEW = 0x2000,
    GAME1_RADAR_UNK_4000        = 0x4000,
    GAME1_FOGCOLOURRGB          = 0x8000,
    GAME1_HIGHFOGCOLOURRGB      = 0x10000,
    GAME1_RADAR_NOTRACKOBJECT   = 0x20000,
    GAME1_VERTEXMODE            = 0x40000,
    GAME1_DYNAMICPM             = 0x80000,
    GAME1_FREEZEINTERFACE       = 0x100000,
    GAME1_CAMERAGOTO            = 0x200000,
    GAME1_ONLYBUILDONPATHS      = 0x400000,
    GAME1_ALWAYSROCKFALL        = 0x800000,
    GAME1_DEBUG_NONERPS         = 0x1000000,
    GAME1_PAUSED                = 0x2000000,
    GAME1_STREAMNERPSSPEACH     = 0x4000000,
    GAME1_LEVELENDING           = 0x8000000,
    GAME1_LASERTRACKER          = 0x10000000,
    GAME1_DEBUG_SHOWVERTEXMODE  = 0x20000000,
    GAME1_DEBUG_NOCLIP_FPS      = 0x40000000,
    GAME1_UNK_80000000          = 0x80000000,
} GameFlags1;

typedef enum GameFlags2
{
    GAME2_NONE                 = 0,
    GAME2_CALLTOARMS           = 0x1,
    GAME2_UNK_2                = 0x2,
    GAME2_ATTACKDEFER          = 0x4,
    GAME2_SHOWDEBUGTOOLTIPS    = 0x8,
    GAME2_ALLOWDEBUGKEYS       = 0x10,
    GAME2_ALLOWEDITMODE        = 0x20,
    GAME2_UNK_40               = 0x40,
    GAME2_UNK_80               = 0x80,
    GAME2_INOPTIONSMENU        = 0x100,
    GAME2_CAMERAMOVING         = 0x200,
    GAME2_MOUSE_INSIDEGAMEVIEW = 0x400,
    GAME2_MUSICON              = 0x800,
    GAME2_INMENU               = 0x1000,
    GAME2_NOMULTISELECT        = 0x2000,
    GAME2_MENU_HASNEXT         = 0x4000,
    GAME2_MENU_HASPREVIOUS     = 0x8000,
    GAME2_ALLOWRENAME          = 0x20000,
    GAME2_RECALLOLOBJECTS      = 0x40000,
    GAME2_GENERATESPIDERS      = 0x80000,
    GAME2_DISABLETOOLTIPSOUND  = 0x100000,
    GAME2_NOAUTOEAT            = 0x200000,
} GameFlags2;

typedef enum GameFlags3
{
    GAME3_NONE          = 0,
    GAME3_UNK_1         = 0x1,
    GAME3_UNK_2         = 0x2,
    GAME3_UNK_4         = 0x4,
    GAME3_PICKUPOBJECT  = 0x8,
    GAME3_LOADVEHICLE   = 0x10,
    GAME3_UNK_20        = 0x20,
    GAME3_UNK_40        = 0x40,
    GAME3_PLACEBUILDING = 0x80,
} GameFlags3;

typedef enum Lego_SurfaceType
{
    Lego_SurfaceType_Tunnel,
    Lego_SurfaceType_Immovable,
    Lego_SurfaceType_Hard,
    Lego_SurfaceType_Medium,
    Lego_SurfaceType_Loose,
    Lego_SurfaceType_Soil,
    Lego_SurfaceType_Lava,
    Lego_SurfaceType_Water,
    Lego_SurfaceType_OreSeam,
    Lego_SurfaceType_Lake,
    Lego_SurfaceType_CrystalSeam,
    Lego_SurfaceType_RechargeSeam,
    Lego_SurfaceType_Rubble,
    Lego_SurfaceType_Reinforcement,
    Lego_SurfaceType_Path,
    Lego_SurfaceType_SlugHole,
    Lego_SurfaceType_Undiscovered,
    Lego_SurfaceType_Cavern,
    Lego_SurfaceType_Count,
} Lego_SurfaceType;

typedef enum ViewMode
{
    ViewMode_FP  = 0,
    ViewMode_Top = 1,
} ViewMode;

typedef struct Lego_Globs
{
    lpConfig config;
    const char* gameName;
    U32 field_8;
    U32 field_c;
    Graphics_Quality quality;
    lpLego_Level currLevel;
    lpContainer rootCont;
    lpViewport viewMain;
    lpViewport viewTrack;
    lpLegoCamera cameraMain;
    lpLegoCamera cameraTrack;
    lpLegoCamera cameraFP;
    lpContainer spotlightTop;
    lpContainer spotlightTrack;
    lpContainer pointLightFP;
    lpContainer dirLightFP;
    lpContainer ambientLight;
    lpContainer rootSpotlight;
    lpContainer rootLight;
    lpContainer dirLightCallToArms;
    F32 FPClipBlocks;
    F32 TVClipDist;
    U32 field_58;
    U32 field_5c;
    U32 field_60;
    lpFont bmpFONT5_HI;
    lpFont bmpToolTipFont;
    lpFont bmpDesktopFont;
    lpFont bmpfont5_HI;
    lpFont bmpMbriefFONT;
    lpFont bmpMbriefFONT2;
    lpFont bmpRSFont;
    lpTextWindow textWnd_80;
    lpTextWindow textWnd_84;
    Point2F gotoNewPos;
    Point2F gotoTargetPos;
    B32 gotoSmooth;
    F32 gameSpeed;
    lpLegoObject objectFP;

    Point3F vectorDragStartUnk_a4;
    F32 float_b0;
    F32 float_b4;
    F32 float_b8;
    F32 float_bc;
    Point2I pointi_c0;
    B32 bool_c8;

    const char* langPowerCrystal_name;
    const char* langOre_name;
    const char* langProcessedOre_name;
    const char* langDynamite_name;
    const char* langBarrier_name;
    const char* langElectricFence_name;
    const char* langSpiderWeb_name;
    const char* langOohScary_name;
    const char* langPath_name;

    const char* langPowerCrystal_theName;
    const char* langOre_theName;
    const char* langProcessedOre_theName;
    const char* langDynamite_theName;
    const char* langBarrier_theName;
    const char* langElectricFence_theName;
    const char* langSpiderWeb_theName;
    const char* langOohScary_theName;
    const char* langPath_theName;

    lpVehicleModel vehicleData;
    lpCreatureModel miniFigureData;
    lpCreatureModel rockMonsterData;
    lpBuildingModel buildingData;
    lpUpgrade_PartModel upgradeData;

    const char** vehicleName;
    const char** miniFigureName;
    const char** rockMonsterName;
    const char** buildingName;
    const char** upgradeName;

    const char* toolName[11]; // TODO: Maybe use a define or enum for the length?
    const char* langTool_name[11]; // TODO: Maybe use a define or enum for the length?

    const char** langVehicle_name;
    const char** langMiniFigure_name;
    const char** langRockMonster_name;
    const char** langBuilding_name;
    const char** langUpgrade_name;

    const char** langVehicle_theName;
    const char** langMiniFigure_theName;
    const char** langRockMonster_theName;
    const char** langBuilding_theName;
    const char** langUpgrade_theName;

    U32 vehicleCount;
    U32 miniFigureCount;
    U32 rockMonsterCount;
    U32 buildingCount;
    U32 upgradeCount;

    const char* surfaceName[Lego_SurfaceType_Count];
    const char* langSurface_name[Lego_SurfaceType_Count];
    SFX_ID langSurface_sound[Lego_SurfaceType_Count];

    lpContainer contBoulder;
    lpContainer contBoulderExplode;
    lpContainer contBoulderExplodeIce;
    lpContainer contCrystal;
    lpContainer contDynamite;
    lpContainer contOresTable[2]; // TODO: Maybe use a define or enum for the length?
    lpContainer contOohScary;
    lpContainer contBarrier;
    lpContainer contElectricFence;
    lpContainer contSpiderWeb;
    lpContainer contRechargeSparkle;
    lpContainer contMiniTeleportUp;
    lpContainer contElectricFenceStud;
    lpContainer contPusher;
    lpContainer contFreezer;
    lpContainer contIceCube;
    lpContainer contSmashPath;
    lpContainer contLaserShot;

    lpSelectPlace selectPlace;

    lpLegoObject recordObjs[10]; // TODO: Maybe use a define or enum for the length?
    U32 recordObjsCount;

    Area2F radarScreenRect;
    F32 radarZoom;
    Point2F radarCenter;
    F32 MedPolyRange;
    F32 HighPolyRange;
    S32 HPBlocks;
    ColourRGBF FogColourRGB;
    ColourRGBF HighFogColourRGB;
    F32 float_364; // level-related?

    ColourRGBF PowerCrystalRGB;
    ColourRGBF UnpoweredCrystalRGB;

    LegoObject_Type placeObjType;
    LegoObject_ID placeObjID;
    Direction placeObjDirection;
    lpLegoObject placeDestSmallTeleporter;
    lpLegoObject placeDestBigTeleporter;
    lpLegoObject placeDestWaterTeleporter;
    F32 MinEnergyForEat;
    lpImage TutorialIcon;

    U32 field_3a0; // unknown

    F32 DynamiteDamageRadius;
    F32 DynamiteMaxDamage;
    F32 DynamiteWakeRadius;
    F32 BirdScarerRadius;

    LegoObject_Type objTeleportQueueTypes_TABLE[LegoObject_Type_Count];
    LegoObject_ID objTeleportQueueIDs_TABLE[LegoObject_Type_Count];
    U32 objTeleportQueue_COUNT;

    F32 MiniFigureRunAway;

    Point3F mouseWorldPos;

    Point2I powerDrainBlocks[100]; // Temporary list used during power-grid calculation.
    U32 powerDrainCount;
    Point2I points2x100_78c[2][100]; // Related to power grid calculation.
    U32 pointsCount2_dcc[2];

    U32 MaxReturnedCrystals;
    S32 MouseScrollBorder;

    const char* langHealth_toolTip;
    const char* langEnergy_toolTip;
    const char* langCrystals_toolTip;
    const char* langOre_toolTip;
    const char* langStuds_toolTip;
    const char* langTools_toolTip;
    const char* langCarryObject_toolTip;
    const char* langDrivenBy_toolTip;
    const char* langOreRequired_toolTip;

    B32 IsFallinsEnabled;
    F32 MinDistFor3DSoundsOnTopView;

    ViewMode viewMode; // FirstPerson or TopDown

    GameFlags1 flags1;
    GameFlags2 flags2;
    GameFlags3 flags3; // only first byte is used(?)

    F32 InitialSlugTime;
    Point2F NextButtonPos;
    Point2F BackButtonPos;
    lpImage NextButtonImage;
    lpImage BackButtonImage;
    lpImage BackArrowImage;
    F32 FogRate;
    F32 timerGame_e3c;
    F32 elapsedAbs;
    F32 DrainTime;
    U32 ReinforceHits;
    U32 CDStartTrack;
    U32 CDTracks;
    U32 FallinMultiplier;
    B32 hasFallins;
    Point2F menuNextPoint;
    Point2F menuPrevPoint;
    ColourRGBF DragBoxRGB;
    lpImage DialogImage;
    lpImage DialogContrastOverlay;
    lpTextWindow DialogTextWndTitle;
    lpTextWindow DialogTextWndMessage;
    lpTextWindow DialogTextWndOK;
    lpTextWindow DialogTextWndCancel;
    const char* CreditsTextFile;
    const char* CreditsBackAVI;
    const char* lanUpgradeLevel_name[16];
    S32 BuildingUpgradeCostOre;
    S32 BuildingUpgradeCostStuds;
    const char* renameInput;
    Point2F renamePosition;
    const char* RenameReplace;
    const char* EndGameAVI1;
    const char* EndGameAVI2;
    Point2I mouseBlockPos;

} Lego_Globs, *lpLego_Globs;

extern Lego_Globs legoGlobs;

extern B32 Gods_Go(const char* programName);

extern B32 Lego_Initialize();
extern B32 Lego_Shutdown_Full();
extern B32 Lego_Shutdown_Quick();
extern B32 Lego_MainLoop(F32 elapsed);

extern B32 Lego_HandleKeys(F32 elapsedGame, F32 elapsed, B32 *outKeyDownT, B32 *outKeyDownR, B32 *outKeyDownAnyShift);

extern void Lego_HandleWorld(F32 elapsedGame, F32 elapsedAbs, B32 keyDownT, B32 keyDownR, B32 keyDownAnyShift);
extern void Lego_UpdateTopdownCamera(F32 elapsedAbs);

extern void Lego_MainView_MouseTransform(U32 mouseX, U32 mouseY, F32 *outXPos, F32* outYPos);

extern void Lego_XYCallback_AddVisibleSmoke(S32 bx, S32 by);

extern void Lego_UnkUpdateMapsWorldUnk_FUN_004290d0(F32 elapsedAbs, B32 pass2);

extern B32 Lego_LoadLevel(const char* levelName);
extern B32 Lego_LoadMapSet(lpLego_Level level, const char* surfaceMap, const char* predugMap, S32 predugParam, const char* terrainMap, S32 terrainParam, const char* blockPointersMap, S32 blockPointersParam, const char* cryOreMap, S32 cryOreParam, const char* erodeMap, const char* pathMap, S32 pathParam, const char* textureSet, const char* emergeMap, const char* aiMap, const char* fallinMap);
extern B32 Lego_LoadOLObjectList(lpLego_Level level, const char* filename);
extern B32 Lego_LoadTextureSet(lpLego_Level level, const char* keyTexturePath);
extern B32 Lego_LoadPreDugMap(lpLego_Level level, const char* filename, S32 modifier);
extern B32 Lego_LoadTerrainMap(lpLego_Level level, const char* filename, S32 modifier);
extern B32 Lego_LoadCryOreMap(lpLego_Level level, const char* filename, S32 modifier);
extern B32 Lego_LoadErodeMap(lpLego_Level level, const char* filename);
extern B32 Lego_LoadPathMap(lpLego_Level level, const char* filename, S32 modifier);
extern B32 Lego_LoadEmergeMap(lpLego_Level level, const char* filename);
extern B32 Lego_LoadAIMap(lpLego_Level level, const char* filename);
extern B32 Lego_LoadFallinMap(lpLego_Level level, const char* filename);
extern B32 Lego_LoadBlockPointersMap(lpLego_Level level, const char* filename, S32 modifier);

extern lpLego_Level Lego_GetLevel();
extern lpMap3D Lego_GetMap();

extern void Lego_SetSoundOn(B32 isSoundOn);
extern void Lego_SetMusicOn(B32 isMusicOn);
extern void Lego_CDTrackPlayNextCallback();

extern void Lego_SetGameSpeed(F32 newGameSpeed);
extern F32 Lego_GetGameSpeed();

extern void Lego_SetViewMode(ViewMode viewMode, lpLegoObject liveObj, U32 cameraFrame);

extern void Lego_Goto(lpLegoObject liveObj, Point2I* blockPos, B32 smooth);

extern B32 Lego_GetObjectByName(const char* objName, LegoObject_Type* outObjType, LegoObject_ID* outObjID, lpContainer* outModel);

extern void Lego_LoadSamples(lpConfig config, B32 noReduceSamples);
extern void Lego_LoadSurfaceTypeDescriptions_sound(lpConfig config, const char* gameName);
extern void Lego_LoadToolTipInfos(lpConfig config, const char* gameName);

extern B32 Lego_LoadLighting();
extern B32 Lego_LoadGraphicsSettings();
extern B32 Lego_LoadUpgradeTypes();
extern B32 Lego_LoadVehicleTypes();
extern B32 Lego_LoadMiniFigureTypes();
extern B32 Lego_LoadRockMonsterTypes();
extern B32 Lego_LoadBuildingTypes();

extern void Lego_LoadObjectNames(lpConfig config);
extern void Lego_LoadObjectTheNames(lpConfig config);
extern void Lego_LoadMiscObjects(lpConfig config);
extern void Lego_LoadToolTips(lpConfig config);
extern void Lego_LoadUpgradeNames(lpConfig config);
extern void Lego_LoadInfoMessages(lpConfig config);
extern void Lego_LoadTextMessages(lpConfig config);
extern void Lego_LoadPanels(lpConfig config, U32 screenWidth, U32 screenHeight);
extern void Lego_LoadPanelButtons(lpConfig config, U32 screenWidth, U32 screenHeight);
extern void Lego_LoadTutorialIcon(lpConfig config);
