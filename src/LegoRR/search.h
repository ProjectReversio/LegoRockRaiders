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

typedef struct SearchLevelLinkFindIndex_10
{
    lpLevelLink resultLink; // Output level link identifier (not modified on failure)
    S32 searchIndex; // Link index to search for
    S32 currentIndex; // Current index in linked list.
    S32 resultIndex; // Link index of found result, should end up equal to searchIndex, or 0 on failure.
} SearchLevelLinkFindIndex_10, *lpSearchLevelLinkFindIndex_10;
