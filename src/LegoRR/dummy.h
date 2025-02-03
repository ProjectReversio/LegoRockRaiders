#pragma once

#include "platform.h"
#include "lego_object.h"

typedef struct BasicObjectModel
{
    LegoObject_ID objID;
    lpContainer contAct; // ACT, true
} BasicObjectModel, *lpBasicObjectModel;

extern void Object_Hide(lpBasicObjectModel basicObjData, B32 hide);

extern void Object_SetOwnerObject(lpBasicObjectModel basicObjData, lpLegoObject liveObj);

extern lpContainer Object_GetActivityContainer(lpBasicObjectModel basicObjData);
