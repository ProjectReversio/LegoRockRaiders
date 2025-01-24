#include "dummy.h"

void Object_Hide(lpBasicObjectModel basicObjData, B32 hide)
{
    Container_Hide(basicObjData->contAct, hide);
}

void Object_SetOwnerObject(lpBasicObjectModel basicObjData, lpLegoObject liveObj)
{
    Container_SetUserData(basicObjData->contAct, liveObj);
}
