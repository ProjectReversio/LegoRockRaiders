#include "dummy.h"

void Object_Hide(lpBasicObjectModel basicObjData, B32 hide)
{
    // TODO: Implement Object_Hide
}

void Object_SetOwnerObject(lpBasicObjectModel basicObjData, lpLegoObject liveObj)
{
    Container_SetUserData(basicObjData->contAct, liveObj);
}
