#include "sfx.h"
#include "mem.h"
#include "utils.h"

SFX_Globs sfxGlobs = { 0 };

void SFX_Initialize()
{
    sfxGlobs.hashNameList = Mem_Alloc(sizeof(U32) * SFX_MAXSAMPLES);

    SFX_RegisterName(SFX_NULL);
    SFX_RegisterName(SFX_Stamp);
    SFX_RegisterName(SFX_Drill);
    SFX_RegisterName(SFX_DrillFade);
    SFX_RegisterName(SFX_RockBreak);
    SFX_RegisterName(SFX_Drip);
    SFX_RegisterName(SFX_Ambient);
    SFX_RegisterName(SFX_AmbientLoop);
    SFX_RegisterName(SFX_Step);
    SFX_RegisterName(SFX_RockMonster);
    SFX_RegisterName(SFX_RockMonster2);
    SFX_RegisterName(SFX_RockMonsterStep);
    SFX_RegisterName(SFX_MFDeposit);
    SFX_RegisterName(SFX_ButtonPressed);
    SFX_RegisterName(SFX_MFLift);
    SFX_RegisterName(SFX_MFThrow);
    SFX_RegisterName(SFX_Walker);
    SFX_RegisterName(SFX_YesSir);
    SFX_RegisterName(SFX_Build);
    SFX_RegisterName(SFX_Okay);
    SFX_RegisterName(SFX_NotOkay);
    SFX_RegisterName(SFX_InterfaceSlideOnScreen);
    SFX_RegisterName(SFX_InterfaceSlideOffScreen);
    SFX_RegisterName(SFX_PanelSlideOnScreen);
    SFX_RegisterName(SFX_PanelSlideOffScreen);
    SFX_RegisterName(SFX_Siren);
    SFX_RegisterName(SFX_CrystalRecharge);
    SFX_RegisterName(SFX_Laser);
    SFX_RegisterName(SFX_LaserHit);
    SFX_RegisterName(SFX_LazerRecharge);
    SFX_RegisterName(SFX_TopPriority);
    SFX_RegisterName(SFX_ImmovableRock);
    SFX_RegisterName(SFX_Wall);
    SFX_RegisterName(SFX_Floor);
    SFX_RegisterName(SFX_BoulderHit);
    SFX_RegisterName(SFX_Place);
    SFX_RegisterName(SFX_PlaceOre);
    SFX_RegisterName(SFX_PlaceCrystal);
    SFX_RegisterName(SFX_Lava);
    SFX_RegisterName(SFX_RockWipe);
    SFX_RegisterName(SFX_FallIn);
    SFX_RegisterName(SFX_MusicLoop);
    SFX_RegisterName(SFX_CaptainSlide);
    SFX_RegisterName(SFX_Dynamite);
    SFX_RegisterName(SFX_AmbientMusicLoop);

    Container_SetSoundTriggerCallback(SFX_Container_SoundTriggerCallback, NULL);
}

S32 SFX_Random_PlaySoundNormal(SFX_ID sfxID, B32 loop)
{
    // TODO: Implement SFX_Random_PlaySoundNormal
    return 0;
}

S32 SFX_Random_PlaySound3DOnFrame(lpContainer cont, SFX_ID sfxID, B32 loop, B32 sound3D, Point3F* wPos)
{
    // TODO: Implement SFX_Random_PlaySound3DOnFrame
    return -1;
}

S32 SFX_Random_PlaySound3DOnContainer(lpContainer cont, SFX_ID sfxID, B32 loop, B32 onCont, lpPoint3F wPos)
{
    // TODO: Implement SFX_Random_PlaySound3DOnContainer
    return -1;
}

B32 SFX_GetType(const char* sfxName, SFX_ID *sfxID)
{
    // TODO: Implement SFX_GetType
    return FALSE;
}

B32 SFX_IsSoundOn()
{
    // TODO: Implement SFX_IsSoundOn
    return FALSE;
}

void SFX_AddToQueue(SFX_ID sfxId, SoundMode mode)
{
    // TODO: Implement SFX_AddToQueue
}

void SFX_Update(F32 elapsed)
{
    // TODO: Implement SFX_Update
}

void SFX_Container_SoundTriggerCallback(const char* sfxName, lpContainer cont, void* data)
{
    SFX_ID sfxID = SFX_NULL;
    if (SFX_GetType(sfxName, &sfxID))
        SFX_Random_PlaySound3DOnContainer(cont, sfxID, FALSE, TRUE, NULL);
}
