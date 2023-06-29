#include <stdio.h>
#include "pointer.h"
#include "anim_clone.h"
#include "flic.h"
#include "lego.h"
#include "utils.h"

Pointer_Globs pointerGlobs = { 0 };

void Pointer_Initialize()
{
    Pointer_RegisterName(Pointer_Standard);
    Pointer_RegisterName(Pointer_Blank);
    Pointer_RegisterName(Pointer_Selected);
    Pointer_RegisterName(Pointer_Drill);
    Pointer_RegisterName(Pointer_CantDrill);
    Pointer_RegisterName(Pointer_Clear);
    Pointer_RegisterName(Pointer_Go);
    Pointer_RegisterName(Pointer_CantGo);
    Pointer_RegisterName(Pointer_Teleport);
    Pointer_RegisterName(Pointer_CantTeleport);
    Pointer_RegisterName(Pointer_Reinforce);
    Pointer_RegisterName(Pointer_CantReinforce);
    Pointer_RegisterName(Pointer_RadarPan);
    Pointer_RegisterName(Pointer_TrackObject);
    Pointer_RegisterName(Pointer_CantZoom);
    Pointer_RegisterName(Pointer_Zoom);
    Pointer_RegisterName(Pointer_Help);
    Pointer_RegisterName(Pointer_CantHelp);
    Pointer_RegisterName(Pointer_PutDown);
    Pointer_RegisterName(Pointer_GetIn);
    Pointer_RegisterName(Pointer_GetOut);
    Pointer_RegisterName(Pointer_TutorialBlockInfo);
    Pointer_RegisterName(Pointer_Okay);
    Pointer_RegisterName(Pointer_NotOkay);
    Pointer_RegisterName(Pointer_CanBuild);
    Pointer_RegisterName(Pointer_CannotBuild);
    Pointer_RegisterName(Pointer_Dynamite);
    Pointer_RegisterName(Pointer_CantDynamite);
    Pointer_RegisterName(Pointer_PickUp);
    Pointer_RegisterName(Pointer_CantPickUp);
    Pointer_RegisterName(Pointer_PickUpOre);
    Pointer_RegisterName(Pointer_LegoManCantDig);
    Pointer_RegisterName(Pointer_VehicleCantDig);
    Pointer_RegisterName(Pointer_LegoManDig);
    Pointer_RegisterName(Pointer_VehicleDig);
    Pointer_RegisterName(Pointer_LegoManCantPickUp);
    Pointer_RegisterName(Pointer_VehicleCantPickUp);
    Pointer_RegisterName(Pointer_LegoManPickUp);
    Pointer_RegisterName(Pointer_VehiclePickUp);
    Pointer_RegisterName(Pointer_LegoManCantGo);
    Pointer_RegisterName(Pointer_VehicleCantGo);
    Pointer_RegisterName(Pointer_LegoManGo);
    Pointer_RegisterName(Pointer_VehicleGo);
    Pointer_RegisterName(Pointer_LegoManClear);
    Pointer_RegisterName(Pointer_VehicleClear);
    Pointer_RegisterName(Pointer_SurfaceType_Immovable);
    Pointer_RegisterName(Pointer_SurfaceType_Hard);
    Pointer_RegisterName(Pointer_SurfaceType_Medium);
    Pointer_RegisterName(Pointer_SurfaceType_Loose);
    Pointer_RegisterName(Pointer_SurfaceType_Soil);
    Pointer_RegisterName(Pointer_SurfaceType_Lava);
    Pointer_RegisterName(Pointer_SurfaceType_Water);
    Pointer_RegisterName(Pointer_SurfaceType_OreSeam);
    Pointer_RegisterName(Pointer_SurfaceType_Lake);
    Pointer_RegisterName(Pointer_SurfaceType_CrystalSeam);
    Pointer_RegisterName(Pointer_SurfaceType_RechargeSeam);
}

void Pointer_LoadPointers(lpConfig config)
{
    char* pointerName;
    for (lpConfig i = config; i; i = Config_GetNextItem(i))
    {
        pointerName = i->itemName;
        B32 reduced = TRUE;
        if (pointerName && strlen(pointerName) && pointerName[0] == '!')
        {
            if ((mainGlobs.flags & MAIN_FLAG_REDUCEIMAGES) == 0)
                pointerName++;
            else
                reduced = FALSE;
        }

        Pointer_Type pointerType;
        if (reduced && Pointer_GetType(pointerName, &pointerType))
        {
            char buffer[1024];
            sprintf(buffer, "%s", i->dataString);
            char* stringParts[10];
            if (Util_Tokenize(buffer, stringParts, ",") == 1)
            {
                pointerGlobs.pointerImages[pointerType] = Image_LoadBMP(stringParts[0]);
                pointerGlobs.pointerIsFlic[pointerType] = FALSE;
            } else {

                Flic_Setup(stringParts[0], &pointerGlobs.pointerImages[pointerType], FLICMEMORY | FLICLOOPINGON);
                pointerGlobs.pointerIsFlic[pointerType] = TRUE;
                pointerGlobs.pointerFlicPositions[pointerType].x = atoi(stringParts[1]);
                pointerGlobs.pointerFlicPositions[pointerType].y = atoi(stringParts[2]);
            }
            if (!pointerGlobs.pointerIsFlic[pointerType])
                Image_SetupTrans(pointerGlobs.pointerImages[pointerType], 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f);
        }
    }
}

Pointer_Type Pointer_GetCurrentType()
{
    return pointerGlobs.currentType;
}

void Pointer_SetCurrent_IfTimerFinished(Pointer_Type pointerType)
{
    if (pointerGlobs.timer <= 0.0f)
        pointerGlobs.currentType = pointerType;
}

void Pointer_DrawPointer(U32 mouseX, U32 mouseY)
{
    lpImage image;
    U32 uVar1;
    Point2F destPos;
    Area2F destArea;

    destPos.x = (F32)mouseX;
    destPos.y = (F32)mouseY;

    if (!pointerGlobs.pointerIsFlic[pointerGlobs.currentType])
    {
        // Draw a BMP image pointer
        image = pointerGlobs.pointerImages[pointerGlobs.currentType];
        if (image == NULL)
            image = pointerGlobs.pointerImages[Pointer_Standard];

        Image_Display(image, &destPos);

        return;
    }

    // Draw a FLIC image pointer
    destArea.x = pointerGlobs.pointerFlicPositions[pointerGlobs.currentType].x + destPos.x;
    destArea.y = pointerGlobs.pointerFlicPositions[pointerGlobs.currentType].y + destPos.y;
    destArea.width = (F32)Flic_GetWidth((lpFlic)pointerGlobs.pointerImages[pointerGlobs.currentType]);
    destArea.height = (F32)Flic_GetHeight((lpFlic)pointerGlobs.pointerImages[pointerGlobs.currentType]);

    Image_Display(pointerGlobs.pointerImages[Pointer_Blank], &destArea);

    B32 freezeInterface = legoGlobs.flags1 & GAME1_FREEZEINTERFACE;

    Flic_Animate((lpFlic)pointerGlobs.pointerImages[pointerGlobs.currentType], &destArea, !freezeInterface, TRUE);
}

B32 Pointer_GetType(const char* name, Pointer_Type* pointerType)
{
    Pointer_Type currPointer = Pointer_Standard;
    char **currPointerName = pointerGlobs.pointerName;

    while(_strcmpi(*currPointerName, name))
    {
        ++currPointerName;
        ++currPointer;
        if (currPointer >= Pointer_Type_Count)
            return FALSE;
    }

    *pointerType = currPointer;

    return TRUE;
}
