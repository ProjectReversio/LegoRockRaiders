#pragma once

#include "platform.h"
#include "maths.h"
#include "container.h"
#include "sound.h"
#include <d3drm.h>

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

extern void SFX_Initialize();

extern S32 SFX_Random_Play_OrAddToQueue(SFX_ID sfxID, B32 loop);
extern S32 SFX_Random_Play_OrInitSoundUnk(lpContainer cont, SFX_ID sfxID, B32 loop, B32 sound3D, Point3F* wPos);
extern B32 SFX_GetType(const char* sfxName, SFX_ID *sfxID);
extern B32 SFX_IsSoundOn();

extern void SFX_AddToQueue(SFX_ID sfxId, SoundMode mode);

extern void SFX_Update(F32 elapsed);
