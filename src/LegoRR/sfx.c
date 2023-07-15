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
    if (sfxName == NULL)
        return FALSE;

    U32 hash = Util_HashString(sfxName, FALSE, TRUE);
    SFX_ID sfxID1 = SFX_NULL;
    SFX_ID sfxID2 = sfxGlobs.hashNameCount + SFX_Preload_Count;
    U32* it = sfxGlobs.hashNameList;
    if (sfxID2 != SFX_NULL)
    {
        do
        {
            if (*it == hash)
            {
                *sfxID = sfxID1;
                return TRUE;
            }
            sfxID1++;
            it++;
        } while (sfxID1 < sfxID2);
    }

    // This flag presumably states the SFX table is still being built
    if ((sfxGlobs.flags & SFX_GLOB_FLAG_POPULATEMODE) != 0)
    {
        *sfxID = sfxID2;
        hash = Util_HashString(sfxName, FALSE, TRUE);
        sfxGlobs.hashNameList[sfxID2] = hash;
        sfxGlobs.hashNameCount++;
        return TRUE;
    }

    return FALSE;
}

B32 SFX_IsSoundOn()
{
    return sfxGlobs.flags & SFX_GLOB_FLAG_SOUNDON;
}

void SFX_AddToQueue(SFX_ID sfxId, SoundMode mode)
{
    if (sfxGlobs.soundQueueCount_1 < 10)
    {
        sfxGlobs.soundQueueSFXTable_1[sfxGlobs.soundQueueCount_1] = sfxId;
        sfxGlobs.soundQueueModesTable_1[sfxGlobs.soundQueueCount_1] = mode;
        sfxGlobs.soundQueueCount_1++;
    }
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
