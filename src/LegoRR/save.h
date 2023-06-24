#pragma once

#include "platform.h"
#include "rewards.h"

typedef enum SaveRewardFlags
{
    SAVEREWARD_FLAG_NONE = 0,
    SAVEREWARD_FLAG_COMPLETED = 0x1,
    SAVEREWARD_FLAG_TUTORIAL = 0x2,
} SaveRewardFlags;

typedef struct SaveStruct_18
{
    U8 field_0x0_0x17[24];
} SaveStruct_18, *lpSaveStruct_18;

typedef struct SaveReward
{
    SaveRewardFlags flags;
    RewardLevel reward;
} SaveReward, *lpSaveReward;

typedef struct SaveData
{
    U32 field_0x0;
    S32 SliderMusicVolume;
    S32 SliderSoundVolume;
    S32 SliderGameSpeed;
    S32 SliderBrightness;
    U32 field_0x14;
    U32 missionsCount;
    SaveStruct_18 saveStruct18_1c;
    lpSaveReward missionsTable; // std::malloc(count_18 * 0x3190)
    U32 field_0x38;
    U32 field_0x3c;
    U32 field_0x40;
    U32 field_0x44;
    U32 field_0x48;
    U32 field_0x4c;
    U32 field_0x50;
    U32 field_0x54;
    U32 field_0x58;
    U32 field_0x5c;
    U32 field_0x60;
    U32 field_0x64;
    U32 field_0x68;
    U32 field_0x6c;
    U32 field_0x70;
    U32 field_0x74;
    U32 field_0x78;
    U32 field_0x7c;
    U32 field_0x80;
    U32 field_0x84;
    U32 field_0x88;
    U32 field_0x8c;
    U32 field_0x90;
    U32 field_0x94;
    U32 field_0x98;
    U32 field_0x9c;
    U32 field_0xa0;
    U32 field_0xa4;
    U32 field_0xa8;
    U32 field_0xac;
    U32 field_0xb0;
    U32 field_0xb4;
} SaveData, *lpSaveData;
