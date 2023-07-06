#pragma once

#include "platform.h"
#include "save.h"
#include "front_end.h"

typedef struct SearchLevelSelectAdd
{
    lpLevelSet levelSet;
    lpMenu menu_4;
    lpMenuItem_SelectData itemData;
    lpSaveData currSave;
} SearchLevelSelectAdd, *lpSearchLevelSelectAdd;

typedef struct SearchLevelSelectInfo_14
{
    lpSaveReward saveReward;
    lpLevelSet levelSet;
    lpMenuItem_SelectData selectData;
    U32 index;
    B32 keepLocked;
} SearchLevelSelectInfo_14, *lpSearchLevelSelectInfo_14;