#include "dummy.h"

void Object_Hide(lpBasicObjectModel basicObjData, B32 hide)
{
    Container_Hide(basicObjData->contAct, hide);
}

void Object_SetOwnerObject(lpBasicObjectModel basicObjData, lpLegoObject liveObj)
{
    Container_SetUserData(basicObjData->contAct, liveObj);
}

lpContainer Object_GetActivityContainer(lpBasicObjectModel basicObjData)
{
    return basicObjData->contAct;
}

lpContainer Object_SearchForPartName(lpBasicObjectModel basicObjData, const char* name, U32 frameNo)
{
    const char* partName = Container_FormatPartName(basicObjData->contAct, name, &frameNo);
    return Container_SearchTree(basicObjData->contAct, partName, Container_SearchMode_FirstMatch, NULL);
}
