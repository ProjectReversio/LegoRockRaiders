#include "objective.h"

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
