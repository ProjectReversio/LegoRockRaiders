#include "objective.h"
#include "lego.h"

void Objective_SetStatus(LevelStatus status)
{
    // TODO: Implement Objective_SetStatus
}

void Objective_Update(lpTextWindow textWnd, lpLego_Level level, F32 elapsedGamed, F32 elapsedAbs)
{
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

void Objective_SetEndTeleportEnabled(B32 on)
{
    // TODO: Implement Objective_SetEndTeleportEnabled
}

void Objective_LoadLevel(lpConfig config, const char* gameName, const char* levelName, struct Lego_Level* level, U32 screenWidth, U32 screenHeight)
{
    // TODO: Implement Objective_LoadLevel
}
