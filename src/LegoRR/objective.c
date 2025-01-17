#include "objective.h"

#include "input.h"
#include "lego.h"
#include "teleporter.h"

Objective_Globs objectiveGlobs = { 0 };

void Objective_SetStatus(LevelStatus status)
{
    // TODO: Implement Objective_SetStatus
}

void Objective_Update(lpTextWindow textWnd, lpLego_Level level, F32 elapsedGamed, F32 elapsedAbs)
{
    if (mainGlobs.programmerLevel > PROGRAMMER_MODE_3)
        Objective_ProgrammerModeGT3_FUN_00458ba0();

    // TODO: Implement Objective_Update
    B32 bVar1 = FALSE;
    // TODO: Implement Objective_Update

    if (((bVar1) && ((legoGlobs.flags2 & GAME2_MUSICON) == GAME2_NONE)) && ((legoGlobs.flags1 & GAME1_USEMUSIC) != 0))
    {
        legoGlobs.flags2 |= GAME2_MUSICON;
        Lego_SetMusicOn(FALSE);
    }

    // TODO: Implement Objective_Update
}

B32 Objective_HandleKeys(B32 spaceKeyHeld, B32 lastLeftMouseButtonDown, B32 *outGotoNextLevel)
{
    LevelStatus status = LEVELSTATUS_FAILED_OTHER;
    if ((objectiveGlobs.flags & OBJECTIVE_GLOB_FLAG_BRIEFING) == OBJECTIVE_GLOB_FLAG_NONE)
    {
        if ((objectiveGlobs.flags & OBJECTIVE_GLOB_FLAG_COMPLETED) == OBJECTIVE_GLOB_FLAG_NONE)
        {
            if ((objectiveGlobs.flags & OBJECTIVE_GLOB_FLAG_FAILED) != OBJECTIVE_GLOB_FLAG_NONE)
            {
                // TODO: Clean this up
                status = (LevelStatus)((U8)(objectiveGlobs.flags >> 8) & 1 | 2);
            }
        }
        else
        {
            status = LEVELSTATUS_COMPLETE;
        }
    }
    else
    {
        status = LEVELSTATUS_INCOMPLETE;
    }

    *outGotoNextLevel = FALSE;
    if ((spaceKeyHeld || lastLeftMouseButtonDown) && objectiveGlobs.currentPages[status] == objectiveGlobs.pageCounts[status])
    {
        if (inputGlobs.msx < 130 || legoGlobs.BackButtonImage->width + 130 <= inputGlobs.msx ||
            inputGlobs.msy < 315 || legoGlobs.BackButtonImage->height + 315 <= inputGlobs.msy)
        {
            if ((objectiveGlobs.flags & OBJECTIVE_GLOB_FLAG_BRIEFING) == OBJECTIVE_GLOB_FLAG_NONE)
            {
                if ((objectiveGlobs.flags & (OBJECTIVE_GLOB_FLAG_COMPLETED | OBJECTIVE_GLOB_FLAG_FAILED)) != OBJECTIVE_GLOB_FLAG_NONE)
                {
                    B32 teleport = Teleporter_ServiceAll(TELEPORT_SERVIVE_VEHICLE | TELEPORT_SERVIVE_MINIFIGURE | TELEPORT_SERVIVE_BUILDING | TELEPORT_SERVIVE_ELECTRICFENCE);
                    if (teleport)
                    {
                        Objective_ProgrammerModeGT3_FUN_00458ba0();
                        *outGotoNextLevel = !Lego_EndLevel();
                    }
                }
            }
            else
            {
                Objective_ProgrammerModeGT3_FUN_00458ba0();

                // TODO: Implement Objective_HandleKeys
            }
        }
    }

    if (status == LEVELSTATUS_FAILED_OTHER)
        return Objective_IsEnded();

    // TODO: Implement Objective_HandleKeys

    return Objective_IsEnded();
}

void Objective_ProgrammerModeGT3_FUN_00458ba0()
{
    if ((objectiveGlobs.flags & OBJECTIVE_GLOB_FLAG_BRIEFING) == OBJECTIVE_GLOB_FLAG_NONE)
    {
        // TODO: Implement Objective_ProgrammerModeGT3_FUN_00458ba0
    }
    else
    {
        // TODO: Implement Objective_ProgrammerModeGT3_FUN_00458ba0

        if ((legoGlobs.flags2 & GAME2_MUSICON) != GAME2_NONE)
        {
            Lego_SetMusicOn(TRUE);
            legoGlobs.flags2 &= ~GAME2_MUSICON;
        }
    }

    // TODO: Implement Objective_ProgrammerModeGT3_FUN_00458ba0
}

// Returns true if the level has ended.
B32 Objective_IsEnded()
{
    // TODO: Implement Objective_IsEnded
    return TRUE;
}

void Objective_SetEndTeleportEnabled(B32 on)
{
    // TODO: Implement Objective_SetEndTeleportEnabled
}

void Objective_LoadLevel(lpConfig config, const char* gameName, const char* levelName, struct Lego_Level* level, U32 screenWidth, U32 screenHeight)
{
    // TODO: Implement Objective_LoadLevel
}
