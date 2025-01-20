#pragma once

#include "common.h"
#include "lego_object.h"

struct Lego_Level;

extern void SpiderWeb_Add(S32 bx, S32 by, lpLegoObject webObj);

extern void SpiderWeb_Restart(struct Lego_Level* level);
