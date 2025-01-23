#pragma once

#include "platform.h"
#include "container.h"
#include "config.h"
#include "meshLOD.h"
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

    U32 objLevel;
    lpObjectStats stats;
    F32 aiTimer_338;
    F32 carryRestTimer_33c;
    F32 health; // (init: -1.0f)
    F32 energy; // (init: -1.0f)
    S32* stolenCrystalLevels; // (alloc: new int[6]) Each element is the count stolen for a level, index 0 only seems to be used for recovery

    // TODO: Implement LegoObject

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

    // TODO: Implement LegoObject_Globs

    lpLegoObject minifigureObj_9cb8;

    // TODO: Implement LegoObject_Globs
} LegoObject_Globs;

extern LegoObject_Globs objectGlobs;

extern void LegoObject_Initialize();

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

extern lpLegoObject LegoObject_Create(void** objModel, LegoObject_Type objType, LegoObject_ID objID);
extern B32 LegoObject_Remove(lpLegoObject liveObj);

extern void HiddenObject_Add(void* objModel, LegoObject_Type objType, LegoObject_ID objID, Point2F* worldPos, F32 heading, F32 health, const char* thisOLName, const char* drivingOLName);

extern lpMeshLOD LegoObject_LoadMeshLOD(lpConfig act, const char* gameName, const char* dirname, LOD_PolyLevel polyLOD, U32 numCameraFrames);
