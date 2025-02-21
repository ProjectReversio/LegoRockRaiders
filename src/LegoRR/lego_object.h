#pragma once

#include "platform.h"
#include "container.h"
#include "config.h"
#include "images.h"
#include "meshLOD.h"
#include "sfx.h"
#include "stats.h"
#include "upgrade.h"

typedef enum LegoObject_Type
{
    LegoObject_None              = 0,
    LegoObject_Vehicle           = 1,
    LegoObject_MiniFigure        = 2,
    LegoObject_RockMonster       = 3,
    LegoObject_Building          = 4,
    LegoObject_Boulder           = 5,
    LegoObject_PowerCrystal      = 6,
    LegoObject_Ore               = 7,
    LegoObject_Dynamite          = 8,
    LegoObject_Barrier           = 9,
    LegoObject_UpgradePart       = 10,
    LegoObject_ElectricFence     = 11,
    LegoObject_SpiderWeb         = 12,
    LegoObject_OohScary          = 13,
    LegoObject_ElectricFenceStud = 14,
    LegoObject_Path              = 15,
    LegoObject_Pusher            = 16,
    LegoObject_Freezer           = 17,
    LegoObject_IceCube           = 18,
    LegoObject_LaserShot         = 19,
    LegoObject_Type_Count        = 20,
    LegoObject_TVCamera          = -1,
} LegoObject_Type;

typedef enum LiveFlags1
{
    LIVEOBJ1_NONE              = 0,
    LIVEOBJ1_MOVING            = 0x1,
    LIVEOBJ1_LIFTING           = 0x2,
    LIVEOBJ1_TURNING           = 0x4,
    LIVEOBJ1_DRILLING          = 0x8,
    LIVEOBJ1_DRILLINGSTART     = 0x10,
    LIVEOBJ1_UNUSED_20         = 0x20,
    LIVEOBJ1_REINFORCING       = 0x40,
    LIVEOBJ1_TURNRIGHT         = 0x80,
    LIVEOBJ1_EXPANDING         = 0x100,
    LIVEOBJ1_GATHERINGROCK     = 0x200,
    LIVEOBJ1_CARRYING          = 0x400,
    LIVEOBJ1_UNK_800           = 0x800,
    LIVEOBJ1_GETTINGHIT        = 0x1000,
    LIVEOBJ1_STORING           = 0x2000,
    LIVEOBJ1_UNK_4000          = 0x4000,
    LIVEOBJ1_WAITING           = 0x8000,
    LIVEOBJ1_UNK_10000         = 0x10000,
    LIVEOBJ1_ENTERING_WALLHOLE = 0x20000,
    LIVEOBJ1_CLEARING          = 0x40000,
    LIVEOBJ1_PLACING           = 0x80000,
    LIVEOBJ1_CRUMBLING         = 0x100000,
    LIVEOBJ1_TELEPORTINGDOWN   = 0x200000,
    LIVEOBJ1_TELEPORTINGUP     = 0x400000,
    LIVEOBJ1_RUNNINGAWAY       = 0x800000,
    LIVEOBJ1_REPAIRDRAINING    = 0x1000000,
    LIVEOBJ1_CAUGHTINWEB       = 0x2000000,
    LIVEOBJ1_SLIPPING          = 0x4000000,
    LIVEOBJ1_SCAREDBYPLAYER    = 0x8000000,
    LIVEOBJ1_UNUSED_10000000   = 0x10000000,
    LIVEOBJ1_RESTING           = 0x20000000,
    LIVEOBJ1_EATING            = 0x40000000,
    LIVEOBJ1_CANTDO            = 0x80000000,
} LiveFlags1;

typedef enum LiveFlags2
{
    LIVEOBJ2_NONE                 = 0,
    LIVEOBJ2_THROWING             = 0x1,
    LIVEOBJ2_THROWN               = 0x2,
    LIVEOBJ2_UNK_4                = 0x4,
    LIVEOBJ2_DRIVING              = 0x8,
    LIVEOBJ2_UNK_10               = 0x10,
    LIVEOBJ2_UNK_20               = 0x20,
    LIVEOBJ2_UNK_40               = 0x40,
    LIVEOBJ2_UNK_80               = 0x80,
    LIVEOBJ2_UNK_100              = 0x100,
    LIVEOBJ2_BUILDPATH            = 0x200,
    LIVEOBJ2_TRAINING             = 0x400,
    LIVEOBJ2_UNK_800              = 0x800,
    LIVEOBJ2_DAMAGE_UNK_1000      = 0x1000,
    LIVEOBJ2_SHOWDAMAGENUMBERS    = 0x2000,
    LIVEOBJ2_PUSHED               = 0x4000,
    LIVEOBJ2_UPGRADING            = 0x8000,
    LIVEOBJ2_TRIGGERFRAMECALLBACK = 0x10000,
    LIVEOBJ2_UNK_20000            = 0x20000,
    LIVEOBJ2_UNK_40000            = 0x40000,
    LIVEOBJ2_UNK_80000            = 0x80000,
    LIVEOBJ2_UNK_100000           = 0x100000,
    LIVEOBJ2_UNK_200000           = 0x200000,
    LIVEOBJ2_FIRINGLASER          = 0x400000,
    LIVEOBJ2_FIRINGPUSHER         = 0x800000,
    LIVEOBJ2_FIRINGFREEZER        = 0x1000000,
    LIVEOBJ2_ACTIVEELECTRICFENCE  = 0x2000000,
    LIVEOBJ2_UNK_4000000          = 0x4000000,
    LIVEOBJ2_FROZEN               = 0x8000000,
    LIVEOBJ2_RECHARGING           = 0x10000000,
    LIVEOBJ2_UNK_20000000         = 0x20000000,
    LIVEOBJ2_DAMAGESHAKING        = 0x40000000,
    LIVEOBJ2_UNK_80000000         = 0x80000000,
} LiveFlags2;

typedef enum LiveFlags3
{
    LIVEOBJ3_NONE                = 0,
    LIVEOBJ3_UNK_1               = 0x1,
    LIVEOBJ3_CANDIG              = 0x2,
    LIVEOBJ3_CANREINFORCE        = 0x4,
    LIVEOBJ3_CANTURN             = 0x8,
    LIVEOBJ3_CANFIRSTPERSON      = 0x10,
    LIVEOBJ3_CANCARRY            = 0x20,
    LIVEOBJ3_CANPICKUP           = 0x40,
    LIVEOBJ3_CANYESSIR           = 0x80,
    LIVEOBJ3_CANSELECT           = 0x100,
    LIVEOBJ3_UNK_200             = 0x200,
    LIVEOBJ3_UNK_400             = 0x400,
    LIVEOBJ3_UNUSED_800          = 0x800,
    LIVEOBJ3_CENTERBLOCKIDLE     = 0x1000,
    LIVEOBJ3_UNK_2000            = 0x2000,
    LIVEOBJ3_UNK_4000            = 0x4000,
    LIVEOBJ3_CANDYNAMITE         = 0x8000,
    LIVEOBJ3_UNK_10000           = 0x10000,
    LIVEOBJ3_SIMPLEOBJECT        = 0x20000,
    LIVEOBJ3_CANDAMAGE           = 0x40000,
    LIVEOBJ3_UPGRADEPART         = 0x80000,
    LIVEOBJ3_ALLOWCULLING_UNK    = 0x100000,
    LIVEOBJ3_SELECTED            = 0x200000,
    LIVEOBJ3_AITASK_UNK_400000   = 0x400000,
    LIVEOBJ3_REMOVING            = 0x800000,
    LIVEOBJ3_UNK_1000000         = 0x1000000,
    LIVEOBJ3_UNK_2000000         = 0x2000000,
    LIVEOBJ3_CANGATHER           = 0x4000000,
    LIVEOBJ3_MONSTER_UNK_8000000 = 0x8000000,
    LIVEOBJ3_CANROUTERUBBLE      = 0x10000000,
    LIVEOBJ3_HASPOWER            = 0x20000000,
    LIVEOBJ3_UNK_40000000        = 0x40000000,
    LIVEOBJ3_POWEROFF            = 0x80000000,
} LiveFlags3;

typedef enum LiveFlags4
{
    LIVEOBJ4_NONE               = 0,
    LIVEOBJ4_LASERTRACKERMODE   = 0x1,
    LIVEOBJ4_DOUBLESELECTREADY  = 0x2,
    LIVEOBJ4_UNK_4              = 0x4,
    LIVEOBJ4_UNK_8              = 0x8,
    LIVEOBJ4_UNK_10             = 0x10,
    LIVEOBJ4_CALLTOARMS         = 0x20,
    LIVEOBJ4_DOCKOCCUPIED       = 0x40,
    LIVEOBJ4_ENTRANCEOCCUPIED   = 0x80,
    LIVEOBJ4_USEDBYCONSTRUCTION = 0x100,
    LIVEOBJ4_DONTSHOWHELPWINDOW = 0x200,
    LIVEOBJ4_CRYORECOSTDROPPED  = 0x800,
    LIVEOBJ4_UNK_1000           = 0x1000,
    LIVEOBJ4_UNK_2000           = 0x2000,
    LIVEOBJ4_UNK_4000           = 0x4000,
    LIVEOBJ4_UNK_8000           = 0x8000,
    LIVEOBJ4_UNK_10000          = 0x10000,
    LIVEOBJ4_UNK_20000          = 0x20000,
    LIVEOBJ4_UNK_40000          = 0x40000,
    LIVEOBJ4_ENGINESOUNDPLAYING = 0x80000,
    LIVEOBJ4_DRILLSOUNDPLAYING  = 0x100000,
    LIVEOBJ4_UNK_200000         = 0x200000,
    LIVEOBJ4_UNK_400000         = 0x400000,
} LiveFlags4;

/*typedef enum LiveFlags5
{
    LIVEOBJ5_NONE             = 0,
    LIVEOBJ5_ABILITY_PILOT    = 0x1,
    LIVEOBJ5_ABILITY_SAILOR   = 0x2,
    LIVEOBJ5_ABILITY_DRIVER   = 0x4,
    LIVEOBJ5_ABILITY_DYNAMITE = 0x8,
    LIVEOBJ5_ABILITY_REPAIR   = 0x10,
    LIVEOBJ5_ABILITY_SCANNER  = 0x20,
} LiveFlags5;*/

typedef enum LegoObject_AbilityFlags
{
    ABILITY_FLAG_NONE     = 0,
    ABILITY_FLAG_PILOT    = 0x1,
    ABILITY_FLAG_SAILOR   = 0x2,
    ABILITY_FLAG_DRIVER   = 0x4,
    ABILITY_FLAG_DYNAMITE = 0x8,
    ABILITY_FLAG_REPAIR   = 0x10,
    ABILITY_FLAG_SCANNER  = 0x20,
} LegoObject_AbilityFlags;

typedef enum LegoObject_UpgradeType
{
    LegoObject_UpgradeType_Drill = 0,
    LegoObject_UpgradeType_Speed = 1,
    LegoObject_UpgradeType_Scan  = 2,
    LegoObject_UpgradeType_Carry = 3,
} LegoObject_UpgradeType;

typedef enum LegoObject_ToolType
{
    LegoObject_ToolType_Drill      = 0,
    LegoObject_ToolType_Spade      = 1,
    LegoObject_ToolType_Hammer     = 2,
    LegoObject_ToolType_Spanner    = 3,
    LegoObject_ToolType_Laser      = 4,
    LegoObject_ToolType_PusherGun  = 5,
    LegoObject_ToolType_BirdScarer = 6,
    LegoObject_ToolType_FreezerGun = 7,
    LegoObject_ToolType_Barrier    = 8,
    LegoObject_ToolType_Dynamite   = 9,
    LegoObject_ToolType_CryOre     = 10,
    LegoObject_ToolType_Count      = 11,
} LegoObject_ToolType;

typedef enum LegoObject_AbilityType
{
    LegoObject_AbilityType_Pilot    = 0,
    LegoObject_AbilityType_Sailor   = 1,
    LegoObject_AbilityType_Driver   = 2,
    LegoObject_AbilityType_Dynamite = 3,
    LegoObject_AbilityType_Repair   = 4,
    LegoObject_AbilityType_Scanner  = 5,
    LegoObject_AbilityType_Count    = 6,
    LegoObject_AbilityType_Invalid  = -1,
} LegoObject_AbilityType;

typedef enum Activity_Type //: S32
{
	Activity_Stand               = 0,
	Activity_Route               = 1,
	Activity_RouteRubble         = 2,
	Activity_RunPanic            = 3,
	Activity_Drill               = 4,
	Activity_Teleport            = 5,
	Activity_Walk                = 6,
	Activity_Reinforce           = 7,
	Activity_Reverse             = 8,
	Activity_TurnLeft            = 9,
	Activity_TurnRight           = 10,
	Activity_CantDo              = 11,
	Activity_Emerge              = 12,
	Activity_Enter               = 13,
	Activity_EnterRein           = 14,
	Activity_Collect             = 15,
	Activity_Gather              = 16,
	Activity_Carry               = 17,
	Activity_CarryRubble         = 18,
	Activity_Throw               = 19,
	Activity_CarryTurnLeft       = 20,
	Activity_CarryTurnRight      = 21,
	Activity_CarryStand          = 22,
	Activity_HitLeft             = 23,
	Activity_HitRight            = 24,
	Activity_HitFront            = 25,
	Activity_HitBack             = 26,
	Activity_HitHard             = 27,
	Activity_Dynamite            = 28,
	Activity_Deposit             = 29,
	Activity_Clear               = 30,
	Activity_Place               = 31,
	Activity_Repair              = 32,
	Activity_Slip                = 33,
	Activity_Rest                = 34,
	Activity_Eat                 = 35,
	Activity_Stamp               = 36,
	Activity_ThrowMan            = 37,
	Activity_ThrownByRockMonster = 38,
	Activity_GetUp               = 39,
	Activity_BuildPath           = 40,
	Activity_Upgrade             = 41,
	Activity_Explode             = 42,
	Activity_Unpowered           = 43,
	Activity_FireLaser           = 44,
	Activity_Freezed             = 45,
	Activity_FreezeStart         = 46,
	Activity_FreezeMelt          = 47,
	Activity_Recharge            = 48,
	Activity_WakeUp              = 49,
	Activity_Train               = 50,
	Activity_FloatOn             = 51,
	Activity_FloatOff            = 52,
	Activity_Opening             = 53,
	Activity_Closing             = 54,
	Activity_Open                = 55,
	Activity_Waiting1            = 56,
	Activity_Waiting2            = 57,
	Activity_Waiting3            = 58,
	Activity_Waiting4            = 59,
	Activity_Waiting5            = 60,
	Activity_Waiting6            = 61,
	Activity_Waiting7            = 62,
	Activity_Waiting8            = 63,
	Activity_Waiting9            = 64,
	Activity_Waiting10           = 65,
	Activity_Waiting11           = 66,
	Activity_Waiting12           = 67,
	Activity_Waiting13           = 68,
	Activity_Waiting14           = 69,
	Activity_Waiting15           = 70,
	Activity_Waiting16           = 71,
	Activity_Waiting17           = 72,
	Activity_Waiting18           = 73,
	Activity_Waiting19           = 74,
	Activity_Waiting20           = 75,
	Activity_Waiting21           = 76,
	Activity_Crumble             = 77,
	Activity_TeleportIn          = 78,
	Activity_Type_Count          = 79,
} Activity_Type;

typedef enum LegoObject_GlobFlags
{
    OBJECT_GLOB_FLAG_NONE             = 0,
    OBJECT_GLOB_FLAG_INITIALISED      = 0x1,
    OBJECT_GLOB_FLAG_REMOVING         = 0x2,
    OBJECT_GLOB_FLAG_POWERUPDATING    = 0x4,
    OBJECT_GLOB_FLAG_POWERNEEDSUPDATE = 0x8,
    OBJECT_GLOB_FLAG_UPDATING         = 0x10,
    OBJECT_GLOB_FLAG_LEVELENDING      = 0x20,
    OBJECT_GLOB_FLAG_CYCLEUNITS       = 0x40,
} LegoObject_GlobFlags;

typedef U32 LegoObject_ID;

typedef struct HiddenObject
{
    Point2I blockPos; // (ol: xPos, yPos -> blockPos)
    Point2F worldPos; // (ol: xPos, yPos)
    F32 heading; // (ol: heading -> radians)
    void* model; // (ol: type)
    LegoObject_Type type; // (ol: type)
    LegoObject_ID id; // (ol: type)
    F32 health; // (ol: health)
    const char* olistID; // (ol: Object%i)
    const char* olistDrivingID; // (ol: driving)
} HiddenObject;

typedef struct LegoObject
{
    LegoObject_Type type;
    LegoObject_ID id;
    const char* customName; // max size is 11 (NOT null-terminated)
    struct VehicleModel* vehicle; // Model for vehicle objects only.
    struct CreatureModel* miniFigure; // Model for mini-figure objects only.
    struct CreatureModel* rockMonster; // Model for monster objects only.
    struct BuildingModel* building; // Model for building objects only.
    lpContainer other; // Model for simple objects only.
    Upgrade_PartModel* upgradePart; // First upgrade part model in linked list of parts.

    // TODO: Implement LegoObject

    U32 routeBlocksTotal; // total blocks to travel for current route
    U32 routeBlocksCurrent; // number of blocks traveled (up to routingBlocksTotal)

    // TODO: Implement LegoObject

    F32 routeCurveTotalDist;
    F32 routeCurveCurrDist;
    F32 routeCurveInitialDist; // Used as spill-over when distance taveled is beyond routeCurveTotalDist.

    Point2F point_298;
    Point3F tempPosition; // Last position used for finding new face direction when moving.
    Point3F faceDirection; // 1.0 to -1.0 directions that determine rotation with atan2
    F32 faceDirectionLength; // faceDirection length (faceDirection may be Vector4F...)

    S32 strafeSignFP; // (direction sign only, does higher numbers do not affect speed)
    S32 forwardSignFP; // (direction sign only, does higher numbers do not affect speed)
    F32 rotateSpeedFP;
    Point3F dirVector_2c8; // Always (0.0f, 0.0f, 0.0f)
    F32 animTime;
    U32 animRepeat; // Number of times an activity animation is set to repeat (i.e. number of jumping jacks/reinforce hits). Zero is default.
    lpContainer cameraNull;
    U32 cameraFrame;
    lpContainer contMiniTeleportUp;

    const char* activityName1;
    const char* activityName2;
    struct AITask* aiTask; // Linked list of tasks (or null). Linked using the `AITask::next` field.
    Point2F targetBlockPos; // (init: -1.0f, -1.0f)

    struct LegoObject* routeToObject;
    struct LegoObject* interactObject; // Used in combination with routeToObject for Upgrade station and RM boulders.
    struct LegoObject* carryingThisObject;
    struct LegoObject* carriedObjects[6]; // (includes carried vehicles)
    U32 carryingIndex; // Index of carried object in holder's carriedObjects list.
    U32 numCarriedObjects;
    U32 carryNullFrames;

    // TODO: Implement LegoObject

    U32 objLevel;
    lpObjectStats stats;
    F32 aiTimer_338;
    F32 carryRestTimer_33c;
    F32 health; // (init: -1.0f)
    F32 energy; // (init: -1.0f)
    S32* stolenCrystalLevels; // (alloc: new int[6]) Each element is the count stolen for a level, index 0 only seems to be used for recovery

    LOD_PolyLevel polyLOD;
    S32 drillSoundHandle; // Handle returned by SFX_Play functions
    S32 engineSoundHandle; // Handle returned by SFX_Play functions
    F32 weaponSlowDeath;
    U32 weaponID;
    F32 weaponRechargeTimer;

    struct LegoObject* freezeObject; // (bi-directional link between frozen RockMonster and IceCube)
    F32 freezeTimer;

    struct LegoObject* driveObject; // (bi-directional link between driver and driven)

    LegoObject_ToolType carriedTools[5];
    U32 numCarriedTools;
    F32 bubbleTimer;
    struct Image* bubbleImage;

    U32 teleporter_modeFlags;
    U32 teleporter_teleportFlags;

    // TODO: Implement LegoObject

    Point3F beamVector_39c; // (used for unkWeaponTypes 1-3 "Lazer", "Pusher", "Freezer")
    Point3F weaponVector_3a8; // (used for unkWeaponType 4 "Lazer")

    Point2F pushingVec2D;
    F32 pushingDist;

    struct LegoObject* throwObject; // (bi-directional link between thrower and thrown
    struct LegoObject* projectileObject; // Projectile fired from weapon.
    U32 field_3c8; // (unused?)
    struct LegoObject* teleportDownObject;
    F32 damageNumbers; // Used to display damage text over objects.
    F32 elapsedTime1; // elapsed time counter 1
    F32 elapsedTime2; // elapsed time counter 2
    F32 activityElapsedTime; // elapsed time since last order?
    LiveFlags1 flags1; // State flags for the object's current activity/behavior
    LiveFlags2 flags2;
    LiveFlags3 flags3;
    LiveFlags4 flags4;
    LegoObject_AbilityFlags abilityFlags; // (orig: flags5) Trained ability flags, and saved in ObjectRecall.
    U32 field_3f4; // (set to zero with tool equipped and never read?)
    B32 bool_3f8;
    F32 floatSnd_3fc;
    F32 floatSnd_400;
    LegoObject_UpgradeType upgradingType; // New upgrade type added as mask to vehicle level when upgrade if finished.
    struct LegoObject* nextFree; // (for listSet)

} LegoObject, *lpLegoObject;

typedef struct LegoObject_Globs
{
    lpLegoObject listSet[32];
    lpLegoObject freeList;
    SFX_ID objectTtSFX[20][15]; // [objType:20][objIndex:15] (cfg: ObjTtSFXs)
    const char* activityName[79]; // [activityType:79]

    F32* UnkSurfaceGrid_1_TABLE;
    F32* UnkSurfaceGrid_2_TABLE;
    U32 UnkSurfaceGrid_COUNT;
    F32 radarSurveyCycleTimer; // Timer for how often survey scans update.
    U32 listCount;
    LegoObject_GlobFlags flags;
    U32 toolNullIndex[11]; // [toolType:11]
    U32 objectTotalLevels[20][15][16]; // [objType:20][objIndex:15][objLevel:16]
    U32 objectPrevLevels[20][15][16]; // [objType:20][objIndex:15][objLevel:16]

    U32 NERPs_TrainFlags;

    lpLegoObject minifigureObj_9cb8;

    // TODO: Implement LegoObject_Globs

    const char* abilityName[6]; // [abilityType:6]
    lpImage ToolTipIcons_Abilities[6]; // [abilityType:6] (cfg: ToolTipIcons)
    lpImage ToolTipIcons_Tools[11]; // [toolType:11] (cfg: ToolTipIcons)
    lpImage ToolTipIcon_Blank; // (cfg: ToolTipIcons::Blank)
    lpImage ToolTipIcon_Ore; // (cfg: ToolTipIcons::Ore)
    U32 BuildingsTeleported;
    F32 s_sound3DUpdateTimer;
    U32 s_stepCounter_c63c; // (static, counter %4 for step SFX)
    void** s_FlocksDestroy_c640; // (static, Struct 0x10, used in Flocks activities (QUICK_DESTROY??))

} LegoObject_Globs;

extern LegoObject_Globs objectGlobs;

extern void LegoObject_Initialize();

extern void LegoObject_AddList();

extern void LegoObject_TriggerFrameCallback(lpContainer cont, void* data);

extern void Object_LoadToolNames(lpConfig config, const char* gameName);
extern void Object_LoadToolTipIcons(lpConfig config);

extern void LegoObject_LoadObjTtsSFX(lpConfig config, const char* gameName);

extern void LegoObject_SetLevelEnding(B32 ending);

extern void LegoObject_RegisterSlimySlugHole(Point2I* blockPos);
extern void LegoObject_RegisterVehicle__callsForWater(lpLegoObject liveObj);
extern void LegoObject_WaterVehicle_Register(lpLegoObject liveObj);

extern void LegoObject_IncLevelPathsBuilt(B32 incCurrent);

extern void LegoObject_SetPositionAndHeading(lpLegoObject liveObj, F32 xPos, F32 yPos, F32 theta, B32 assignHeading);

extern void LegoObject_FUN_00438720(lpLegoObject liveObj);

typedef B32 (LegoObject_RunThroughListsCallback)(lpLegoObject obj, void* search);

extern B32 LegoObject_RunThroughListsSkipUpgradeParts(LegoObject_RunThroughListsCallback callback, void* search);

extern B32 LegoObject_DoPickSphereSelection(U32 mouseX, U32 mouseY, lpLegoObject* selectedObj);

extern lpLegoObject LegoObject_Create_internal();
extern lpLegoObject LegoObject_Create(void** objModel, LegoObject_Type objType, LegoObject_ID objID);
extern B32 LegoObject_Remove(lpLegoObject liveObj);

extern void LegoObject_MiniFigure_EquipTool(lpLegoObject liveObj, LegoObject_ToolType toolType);

extern void LegoObject_UpdatePowerConsumption(lpLegoObject liveObj);

extern B32 LegoObject_UpdateActivityChange(lpLegoObject liveObj);

extern void LegoObject_SetActivity(lpLegoObject liveObj, Activity_Type activityType, U32 repeatCount);

extern lpContainer LegoObject_GetActivityContainer(lpLegoObject liveObj);

extern void LegoObject_HideAllCertainObjects();

extern void LegoObject_UpdateAll(F32 elapsedGame);

extern B32 LegoObject_Callback_Update(lpLegoObject liveObj, void* search);

extern B32 LegoObject_RunThroughLists(LegoObject_RunThroughListsCallback callback, void* search, B32 skipUpgradeParts);

extern void LegoObject_Flocks_Update_FUN_0044c1c0(F32* pElapsed);

// Marks blocks in the first PowerGrid list as unpowered and moves them to the second list. Then
// clears the first list.
extern void Level_PowerGrid_ClearBlockPowered_100_Points28C();

// Updates the block surfaces for unpowered blocks in the second PowerGrid list. Then clears the
// list.
extern void Level_PowerGrid_UpdateLevelBlocks_PointsAAC();

// If we're currently in the update loop, then delay recalculations until the end of the update loop/next tick(?).
//
// Otherwise, we can turn on power updating mode right now.
extern void LegoObject_RequestPowerGridUpdate();

// The same as `LegoObject_GetWorldZCallback`, but returns a lower Z value with over Lake terrain.
// Objects wading in a lake (aka, not sailing) will have their Z lowered a bit, and have it at the lowest near the center of a lake BLOCK.
extern F32 LegoObject_GetWorldZCallback_Lake(F32 xPos, F32 yPos, struct Map3D* map);

extern const char* Object_GetLangName(LegoObject_Type objType, LegoObject_ID objID);

extern void HiddenObject_Add(void* objModel, LegoObject_Type objType, LegoObject_ID objID, Point2F* worldPos, F32 heading, F32 health, const char* thisOLName, const char* drivingOLName);

extern lpMeshLOD LegoObject_LoadMeshLOD(lpConfig act, const char* gameName, const char* dirname, LOD_PolyLevel polyLOD, U32 numCameraFrames);

extern void LegoObject_Route_End(lpLegoObject liveObj, B32 completed);

extern void LegoObject_DropCarriedObject(lpLegoObject liveObj, B32 putAway);

extern F32 LegoObject_MoveAnimation(lpLegoObject liveObj, F32 elapsed);

extern void LegoObject_CalculateSpeeds(lpLegoObject liveObj, F32 elapsed, F32* outRouteSpeed, F32* outTransSpeed);

extern B32 LegoObject_TryDepart_FUN_004499c0(lpLegoObject liveObj);

extern void LegoObject_SimpleObject_FUN_00448160(lpLegoObject liveObj, F32 elapsed);

extern lpContainer LegoObject_GetDepositNull(lpLegoObject liveObj);

extern B32 LegoObject_DoOpeningClosing(lpLegoObject liveObj, B32 open);

extern B32 LegoObject_FUN_0043c6a0(lpLegoObject liveObj);

extern void LegoObject_Proc_FUN_0043c7f0(lpLegoObject liveObj);

extern void LegoObject_Route_UpdateMovement(lpLegoObject liveObj, F32 elapsed);

typedef B32 (*LegoObject_CheckBlock_Callback)(lpLegoObject liveObj, Point2I* blockPos, void* data);
extern B32 LegoObject_CheckBlock_FUN_00443b00(lpLegoObject liveObj, Point2I* blockPos, void* data);
extern B32 LegoObject_Route_Score_FUN_004413b0(lpLegoObject liveObj, U32 bx, U32 by, U32 bx2, U32 by2, S32** outNewBXs, S32** outNewBYs, U32* outCount, LegoObject_CheckBlock_Callback callback, void* data);
extern B32 LegoObject_Route_ScoreSub_FUN_00440f30(lpLegoObject liveObj, U32 bx, U32 by, U32 bx2, U32 by2, U32** outNewBXs, U32** outNewBYs, U32* outCount, LegoObject_CheckBlock_Callback callback, void* data);

extern B32 LegoObject_FUN_004439d0(lpLegoObject liveObj, Point2I* blockPos, Point2I* outBlockPos, U32 unused);

// Update energy drain while carrying and attempt to rest when needed
extern void LegoObject_UpdateCarryingEnergy(lpLegoObject liveObj, F32 elapsed);

extern void LegoObject_RockMonster_FUN_0043ad70(lpLegoObject liveObj);

extern B32 LegoObject_GetBlockPos(lpLegoObject liveObj, S32* outBx, S32* outBy);

extern B32 LiveObject_GetDamageFromSurface(lpLegoObject liveObj, S32 bx, S32 by, F32 elapsedGame, F32* optOutDamage);
