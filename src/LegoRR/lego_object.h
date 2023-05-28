#pragma once

#include "types.h"
#include "container.h"

typedef struct LegoObject
{
    void* temp;
    // TODO: Implement LegoObject
} LegoObject, *lpLegoObject;

extern void LegoObject_Initialize();

extern void LegoObject_TriggerFrameCallback(lpContainer cont, void* data);
