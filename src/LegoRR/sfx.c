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

// value is modified via Util_Tokenize
B32 SFX_LoadSampleProperty(char* value, SFX_ID sfxID)
{
    // TODO: See here for OpenLRR implementation which fixes a bug from the original game:
    //  https://github.com/trigger-segfault/OpenLRR/blob/9ec34ee254d3c1846cc41a348e160dc4fcfb966f/src/openlrr/game/audio/SFX.cpp#L140

    // TODO: Cleanup Decompiled Code

    lpSFX_Property prop = &sfxGlobs.samplePropTable[sfxID];
    B32 stream = FALSE;
    B32 success = TRUE;
    lpSFX_Property lastItem = NULL;
    sfxGlobs.samplePropTable[sfxID].next = NULL;

    char* filename_parts[100];
    S32 numParts = Util_Tokenize(value, filename_parts, ",");
    if (numParts == 0)
        return success;

    char** partIter = filename_parts;

    do
    {
        char* charIter = *partIter;
        char currentChar = *charIter;
        if (currentChar == '*')
            charIter++;

        S32 volume = 0;
        if (*charIter == '#')
        {
            char cVar2 = charIter[1];
            charIter++;
            char volBuff[64];
            if (cVar2 != '#')
            {
                char* pcVar3 = volBuff;
                do
                {
                    *pcVar3 = cVar2;
                    cVar2 = charIter[1];
                    pcVar3++;
                    charIter++;
                } while (cVar2 != '#');
            }
            volume = atoi(volBuff);
            charIter++;
        }

        if (*charIter == '@')
        {
            stream = TRUE;
            charIter++;
        }

        S32 soundHandle = Sound3D_Load(charIter, stream, (currentChar == '*' ? 1 : 0), volume);
        prop->sound3DHandle = soundHandle;
        U32 sampleGroupCount = sfxGlobs.sampleGroupCount;

        lpSFX_Property pSVar1;
        if (soundHandle == -1)
        {
            success = FALSE;
            pSVar1 = lastItem;
        } else
        {
            pSVar1 = prop;
            if (lastItem != NULL)
            {
                prop = &sfxGlobs.sampleGroupTable[sfxGlobs.sampleGroupCount];
                sfxGlobs.sampleGroupCount++;
                lastItem->next = prop;
                sfxGlobs.sampleGroupTable[sampleGroupCount].next = NULL;
                pSVar1 = prop;
            }
        }

        lastItem = pSVar1;
        partIter++;
        numParts--;
    } while (numParts != 0);

    return success;
}

S32 SFX_Random_PlaySoundNormal(SFX_ID sfxID, B32 loop)
{
    // TODO: See here for OpenLRR implementation which fixes a bug from the original game:
    //  https://github.com/trigger-segfault/OpenLRR/blob/9ec34ee254d3c1846cc41a348e160dc4fcfb966f/src/openlrr/game/audio/SFX.cpp#L311

    SFX_ID result;
    U32 instCount;
    lpSFX_Instance instance;

    instCount = sfxGlobs.sfxInstanceCount;
    if ((sfxGlobs.flags & SFX_GLOB_FLAG_QUEUEMODE) == 0)
    {
        result = sfxID;
        if (sfxID != SFX_NULL && (sfxGlobs.flags & SFX_GLOB_FLAG_SOUNDON) != 0)
        {
            result = SFX_Random_GetSound3DHandle(sfxID);
            if (result != SFX_NULL)
                result = Sound3D_Play2(Sound3D_Play_Normal, NULL, result, loop, NULL);
        }
    } else if (sfxGlobs.sfxInstanceCount < 10)
    {
        instance = &sfxGlobs.sfxInstanceTable[sfxGlobs.sfxInstanceCount];
        sfxGlobs.sfxInstanceCount++;
        instance->sampleIndex = sfxID;
        sfxGlobs.sfxInstanceTable[instCount].flags &= 0xFFFFFFFE;
        return (S32)instance;

    }

    return result;
}

S32 SFX_Random_PlaySound3DOnFrame(LPDIRECT3DRMFRAME3 frame, SFX_ID sfxID, B32 loop, B32 onFrame, Point3F* wPos)
{
    if (((U8)sfxGlobs.flags & SFX_GLOB_FLAG_QUEUEMODE) == 0)
    {
        if (sfxID != SFX_NULL && (((U8)sfxGlobs.flags & SFX_GLOB_FLAG_SOUNDON) != 0))
        {
            S32 rngSound3DHandle = SFX_Random_GetSound3DHandle(sfxID);
            if (rngSound3DHandle != 0)
            {
                if (onFrame)
                    return Sound3D_Play2(Sound3D_Play_OnFrame, frame, rngSound3DHandle, loop, NULL);

                return Sound3D_Play2(Sound3D_Play_OnPos, NULL, rngSound3DHandle, loop, wPos);
            }
        }
    } else if (sfxGlobs.sfxInstanceCount < 10)
    {
        lpSFX_Instance sfxInst = &sfxGlobs.sfxInstanceTable[sfxGlobs.sfxInstanceCount];
        sfxInst->sampleIndex = sfxID;
        sfxInst->frame = frame;

        sfxInst->flags &= ~(SFX_INSTANCE_FLAG_ONFRAME | SFX_INSTANCE_FLAG_LOOPING);
        sfxInst->flags |= SFX_INSTANCE_FLAG_SOUND3D;

        if (loop)
            sfxInst->flags |= SFX_INSTANCE_FLAG_LOOPING;

        if (onFrame)
            sfxInst->flags |= SFX_INSTANCE_FLAG_ONFRAME;

        if (wPos != NULL)
        {
            sfxInst->position.x = wPos->x;
            sfxInst->position.y = wPos->y;
            sfxInst->position.z = wPos->z;
        }

        sfxGlobs.sfxInstanceCount++;
    }

    return 0;
}

S32 SFX_Random_PlaySound3DOnContainer(lpContainer cont, SFX_ID sfxID, B32 loop, B32 onCont, lpPoint3F wPos)
{
    LPDIRECT3DRMFRAME3 frame;
    S32 playHandle;

    if (cont == NULL)
        frame = NULL;
    else
        frame = Container_GetMasterFrame(cont);

    playHandle = SFX_Random_PlaySound3DOnFrame(frame, sfxID, loop, onCont, wPos);
    return playHandle;
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

void SFX_SetSoundOn(B32 soundOn, B32 stopAll)
{
    if (soundOn)
    {
        sfxGlobs.flags |= SFX_GLOB_FLAG_SOUNDON;
        return;
    }

    sfxGlobs.flags &= ~SFX_GLOB_FLAG_SOUNDON;

    if (stopAll)
        Sound3D_StopAllSounds();

    sfxGlobs.globalSampleSFXType = SFX_NULL;
}

void SFX_SetSoundOn_AndStopAll(B32 soundOn)
{
    SFX_SetSoundOn(soundOn, TRUE);
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
    B32 origQueueMode = SFX_IsQueueMode();
    sfxGlobs.globalSampleDuration -= elapsed;
    SFX_SetQueueMode(FALSE, FALSE);
    U32 i = 0;
    if (sfxGlobs.soundQueueCount_2 != 0)
    {
        SFX_ID* it = sfxGlobs.soundQueueSFXTable_2;
        do
        {
            SFX_Random_PlaySoundNormal(*it, it[10]);
            i++;
            it++;
        } while (i < sfxGlobs.soundQueueCount_2);
    }

    if (origQueueMode)
        SFX_SetQueueMode_AndFlush(TRUE);

    memcpy(sfxGlobs.soundQueueSFXTable_2, sfxGlobs.soundQueueSFXTable_1, sizeof(SFX_ID) * sfxGlobs.soundQueueCount_1);
    sfxGlobs.soundQueueCount_2 = sfxGlobs.soundQueueCount_1;

    memcpy(sfxGlobs.soundQueueModesTable_2, sfxGlobs.soundQueueModesTable_1, sizeof(SoundMode) * sfxGlobs.soundQueueCount_1);

    sfxGlobs.soundQueueCount_1 = 0;

    SFX_Sound3D_Update();
}

void SFX_Sound3D_Update()
{
    Sound3D_Update();
}

B32 SFX_IsQueueMode()
{
    return sfxGlobs.flags & SFX_GLOB_FLAG_QUEUEMODE;
}

void SFX_SetQueueMode(B32 on, B32 flushQueued)
{
    // TODO: Implement SFX_SetQueueMode
}

void SFX_SetQueueMode_AndFlush(B32 on)
{
    SFX_SetQueueMode(on, TRUE);
}

void SFX_SetSamplePopulateMode(B32 on)
{
    if (on)
    {
        sfxGlobs.flags |= SFX_GLOB_FLAG_POPULATEMODE;
        return;
    }

    sfxGlobs.flags &= ~SFX_GLOB_FLAG_POPULATEMODE;
}

S32 SFX_Random_GetSound3DHandle(SFX_ID sfxID)
{
    lpSFX_Property pSVar1;
    S16 rng;
    U32 groupIndex;
    lpSFX_Property sfxGroup;
    S32 swapHandle;

    groupIndex = 0;
    if (sfxID != SFX_NULL)
    {
        if (sfxGlobs.samplePropTable[sfxID].next != NULL)
        {
            pSVar1 = &sfxGlobs.samplePropTable[sfxID];
            for (sfxGroup = pSVar1; sfxGroup != NULL; sfxGroup = sfxGroup->next)
            {
                groupIndex++;
            }

            rng = Maths_Rand();
            sfxGroup = pSVar1;

            for (groupIndex = rng % groupIndex; groupIndex != 0; groupIndex--)
            {
                sfxGroup = sfxGroup->next;
            }

            swapHandle = sfxGroup->sound3DHandle;
            sfxGroup->sound3DHandle = pSVar1->sound3DHandle;
            pSVar1->sound3DHandle = swapHandle;
        }

        //for (U32 i = 0; i < MAX_SAMPLES; i++)
        //{
        //    Error_Warn(TRUE, Error_Format("samplePropTable[%d].sound3DHandle = %d\n", i, sfxGlobs.samplePropTable[i].sound3DHandle));
        //}

        return sfxGlobs.samplePropTable[sfxID].sound3DHandle;
    }
    return -1;
}

void SFX_Container_SoundTriggerCallback(const char* sfxName, lpContainer cont, void* data)
{
    SFX_ID sfxID = SFX_NULL;
    if (SFX_GetType(sfxName, &sfxID))
        SFX_Random_PlaySound3DOnContainer(cont, sfxID, FALSE, TRUE, NULL);
}
