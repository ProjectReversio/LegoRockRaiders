#pragma once

#include "platform.h"
#include "maths.h"
#include "container.h"
#include "sound.h"
#include <d3drm.h>

#define SFX_MAXSAMPLES 495
#define SFX_MAXSAMPLEGROUPS 200

#define SFX_RegisterName(n) (sfxGlobs.hashNameList[n] = Util_HashString(#n, FALSE, TRUE))

typedef enum SFX_ID
{
    SFX_NULL                    = 0,
    SFX_Stamp                   = 1,
    SFX_Drill                   = 2,
    SFX_DrillFade               = 3,
    SFX_RockBreak               = 4,
    SFX_Drip                    = 5,
    SFX_Ambient                 = 6,
    SFX_AmbientLoop             = 7,
    SFX_Step                    = 8,
    SFX_RockMonster             = 9,
    SFX_RockMonster2            = 10,
    SFX_RockMonsterStep         = 11,
    SFX_MFDeposit               = 12,
    SFX_ButtonPressed           = 13,
    SFX_MFLift                  = 14,
    SFX_MFThrow                 = 15,
    SFX_Walker                  = 16,
    SFX_YesSir                  = 17,
    SFX_Build                   = 18,
    SFX_Okay                    = 19,
    SFX_NotOkay                 = 20,
    SFX_InterfaceSlideOnScreen  = 21,
    SFX_InterfaceSlideOffScreen = 22,
    SFX_PanelSlideOnScreen      = 23,
    SFX_PanelSlideOffScreen     = 24,
    SFX_Siren                   = 25,
    SFX_CrystalRecharge         = 26,
    SFX_Laser                   = 27,
    SFX_LaserHit                = 28,
    SFX_LazerRecharge           = 29,
    SFX_TopPriority             = 30,
    SFX_ImmovableRock           = 31,
    SFX_Wall                    = 32,
    SFX_Floor                   = 33,
    SFX_BoulderHit              = 34,
    SFX_Place                   = 35,
    SFX_PlaceOre                = 36,
    SFX_PlaceCrystal            = 37,
    SFX_Lava                    = 38,
    SFX_RockWipe                = 39,
    SFX_FallIn                  = 40,
    SFX_MusicLoop               = 41,
    SFX_CaptainSlide            = 42,
    SFX_Dynamite                = 43,
    SFX_AmbientMusicLoop        = 44,
    SFX_Preload_Count           = 45,
    SFX_ID_Invalid              = -1,
} SFX_ID;

typedef enum SFX_InstanceFlags
{
    SFX_INSTANCE_FLAG_NONE    = 0,
    SFX_INSTANCE_FLAG_UNK_1   = 0x1,
    SFX_INSTANCE_FLAG_LOOPING = 0x2,
    SFX_INSTANCE_FLAG_SOUND3D = 0x3,
} SFX_InstanceFlags;

typedef enum SFX_GlobFlags
{
    SFX_GLOB_FLAG_NONE         = 0,
    SFX_GLOB_FLAG_SOUNDON      = 0x1,
    SFX_GLOB_FLAG_POPULATEMODE = 0x2,
    SFX_GLOB_FLAG_QUEUEMODE    = 0x8,
} SFX_GlobFlags;

typedef struct SFX_Instance
{
    S32 sampleIndex;
    LPDIRECT3DRMFRAME3 frame; // what happened to hiding D3DRM behind Containers and 3DSound?
    Point3F position;
    SFX_InstanceFlags flags;
} SFX_Instance, *lpSFX_Instance;

typedef struct SFX_Property
{
    S32 sound3DHandle;
    struct SFX_Property* next; // next group property
} SFX_Property, *lpSFX_Property;

typedef struct SFX_Globs
{
    SFX_Property samplePropTable[SFX_MAXSAMPLES];
    SFX_Property sampleGroupTable[SFX_MAXSAMPLEGROUPS];
    U32* hashNameList;
    U32 hashNameCount;
    U32 sampleGroupCount;
    SFX_GlobFlags flags;
    SFX_Instance sfxInstanceTable[10];
    U32 sfxInstanceCount;
    F32 globalSampleDuration;
    Sound globalSampleSoundHandle;
    SFX_ID globalSampleSFXType;
    SFX_ID soundQueueSFXTable_1[10];
    SoundMode soundQueueModesTable_1[10];
    SFX_ID soundQueueSFXTable_2[10];
    SoundMode soundQueueModesTable_2[10];
    U32 soundQueueCount_1;
    U32 soundQueueCount_2;
} SFX_Globs;

extern SFX_Globs sfxGlobs;

extern void SFX_Initialize();

extern S32 SFX_Random_PlaySoundNormal(SFX_ID sfxID, B32 loop);
extern S32 SFX_Random_PlaySound3DOnFrame(lpContainer cont, SFX_ID sfxID, B32 loop, B32 sound3D, Point3F* wPos);
extern S32 SFX_Random_PlaySound3DOnContainer(lpContainer cont, SFX_ID sfxID, B32 loop, B32 onCont, lpPoint3F wPos);
extern B32 SFX_GetType(const char* sfxName, SFX_ID *sfxID);
extern B32 SFX_IsSoundOn();
extern void SFX_SetSoundOn(B32 soundOn, B32 stopAll);
extern void SFX_SetSoundOn_AndStopAll(B32 soundOn);

extern void SFX_AddToQueue(SFX_ID sfxId, SoundMode mode);

extern void SFX_Update(F32 elapsed);
extern void SFX_Sound3D_Update();

extern B32 SFX_IsQueueMode();
extern void SFX_SetQueueMode(B32 on, B32 flushQueued);
extern void SFX_SetQueueMode_AndFlush(B32 on);

extern S32 SFX_Random_GetSound3DHandle(SFX_ID sfxID);

extern void SFX_Container_SoundTriggerCallback(const char* sfxName, lpContainer cont, void* data);
