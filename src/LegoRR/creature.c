#include "creature.h"

#include <stdio.h>

#include "activities.h"
#include "meshLOD.h"
#include "utils.h"

B32 Creature_Load(lpCreatureModel creature, LegoObject_ID objID, lpContainer root, const char* filename, const char* gameName)
{
    char buffName[1024];
    strcpy(buffName, filename);

    char* stringParts[100];
    U32 partCount = Util_Tokenize(buffName, stringParts, "\\");
    if (partCount > 1)
    {
        // TODO: Clean this up
        char** sp = stringParts;
        S32 i = (S32)partCount - 1;
        do
        {
            sp++;
            i--;
            (*sp)[-1] = '\\';
        } while (i != 0);
    }

    char buffPath[1024];
    sprintf(buffPath, "%s\\%s.%s", buffName, stringParts[partCount - 1], ACTIVITY_FILESUFFIX);

    lpConfig prop = Config_Load(buffPath);
    if (prop == NULL)
        return FALSE;

    B32 result = FALSE;

    creature->contAct = Container_Load(root, filename, "ACT", TRUE);
    if (creature->contAct != NULL)
    {
        creature->cameraNullName = Config_GetStringValue(prop, Config_BuildStringID(gameName, "CameraNullName", 0));
        if (creature->cameraNullName == NULL)
        {
            creature->cameraNullFrames = 0;
        }
        else
        {
            creature->cameraNullFrames = Config_GetIntValue(prop, Config_BuildStringID(gameName, "CameraNullFrames", 0));
            creature->cameraFlipDir = Config_GetBoolValue(prop, Config_BuildStringID(gameName, "CameraFlipDir", 0));
        }

        creature->drillNullName = Config_GetStringValue(prop, Config_BuildStringID(gameName, "DrillNullName", 0));
        creature->footStepNullName = Config_GetStringValue(prop, Config_BuildStringID(gameName, "FootStepNullName", 0));
        creature->carryNullName = Config_GetStringValue(prop, Config_BuildStringID(gameName, "CarryNullName", 0));
        creature->throwNullName = Config_GetStringValue(prop, Config_BuildStringID(gameName, "ThrowNullName", 0));
        creature->depositNullName = Config_GetStringValue(prop, Config_BuildStringID(gameName, "DepositNullName", 0));

        creature->objID = objID;

        creature->polyMedium = LegoObject_LoadMeshLOD(prop, gameName, filename, LOD_MediumPoly, 1);
        creature->polyHigh = LegoObject_LoadMeshLOD(prop, gameName, filename, LOD_HighPoly, 1);
        creature->polyFP = LegoObject_LoadMeshLOD(prop, gameName, filename, LOD_FPPoly, creature->cameraNullFrames);

        creature->flags = CREATURE_FLAG_SOURCE;

        result = TRUE;
    }

    Config_Free(prop);

    return result;
}

void Creature_Clone(lpCreatureModel srcCreature, lpCreatureModel destCreature)
{
    memcpy(destCreature, srcCreature, sizeof(CreatureModel));
    destCreature->flags &= ~CREATURE_FLAG_SOURCE;
    destCreature->polyMedium = MeshLOD_Clone(srcCreature->polyMedium);
    destCreature->polyHigh = MeshLOD_Clone(srcCreature->polyHigh);
    destCreature->polyFP = MeshLOD_Clone(srcCreature->polyFP);
    destCreature->contAct = Container_Clone(srcCreature->contAct);
}

void Creature_SetOrientation(lpCreatureModel creature, F32 xDir, F32 yDir)
{
    Container_SetOrientation(creature->contAct, NULL, xDir, yDir, 0.0f, 0.0f, 0.0f, -1.0f);
}

void Creature_SetPosition(lpCreatureModel creature, F32 xPos, F32 yPos, GetWorldZCallback zCallback, struct Map3D* map)
{
    Container_SetPosition(creature->contAct, NULL, xPos, yPos, zCallback(xPos, yPos, map));
}

F32 Creature_MoveAnimation(lpCreatureModel creature, F32 elapsed, U32 unkFrameNo)
{
    // TEMP: Temporarily check for NULL, original doesn't have this check.
    if (creature == NULL)
        return 0.0f;

    F32 anim = Container_MoveAnimation(creature->contAct, elapsed);
    if (unkFrameNo > 1 && anim != 0.0f)
    {
        U32 frames = Container_GetAnimationFrames(creature->contAct);
        return anim - (F32)(frames * (unkFrameNo - 1));
    }

    return anim;
}
