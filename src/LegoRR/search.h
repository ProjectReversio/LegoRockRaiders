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
