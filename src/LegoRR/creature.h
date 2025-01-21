#pragma once

#include "platform.h"
#include "lego_object.h"

typedef struct CreatureModel
{
    void* temp;
    // TODO: Implement CreatureModel
} CreatureModel, *lpCreatureModel;

extern B32 Creature_Load(lpCreatureModel creature, LegoObject_ID objID, lpContainer root, const char* filename, const char* gameName);
