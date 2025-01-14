#include "level.h"

B32 Level_DestroyWall(lpLego_Level level, U32 bx, U32 by, B32 isHiddenCavern)
{
    Point2I ANGLES[8];

    ANGLES[0].x = 0;
    ANGLES[0].y = 0;
    ANGLES[1].x = 1;
    ANGLES[1].y = 0;
    ANGLES[2].x = 1;
    ANGLES[2].y = 1;
    ANGLES[3].x = 0;
    ANGLES[3].y = 1;

    if (bx == 0 || by == 0 || level->width - 2 <= bx || level->height - 2 <= by)
        return FALSE;

    lpLego_Block block = &level->blocks[by * level->width + bx];
    if ((block->flags1 & BLOCK1_FLOOR) != 0)
        return 0;

    if (block->terrain == Lego_SurfaceType8_OreSeam || block->terrain == Lego_SurfaceType8_CrystalSeam)
    {
        // TODO: Implement Level_DestroyWall
    }
    else
    {
        if (block->predug == Lego_PredugType_Wall)
        {
            Map3D_MoveBlockVertices(level->map, bx, by, level->DigDepth);
            level->blocks[by * level->width + bx].predug = Lego_PredugType_Cavern_Exposed;
        }

        if (level->blocks[by * level->width + 1 + bx].predug == Lego_PredugType_Wall)
        {
            Map3D_MoveBlockVertices(level->map, bx + 1, by, level->DigDepth);
            level->blocks[by * level->width + 1 + bx].predug = Lego_PredugType_Cavern_Exposed;
        }

        if (level->blocks[(by + 1) * level->width + 1 + bx].predug == Lego_PredugType_Wall)
        {
            Map3D_MoveBlockVertices(level->map, bx + 1, by + 1, level->DigDepth);
            level->blocks[(by + 1) * level->width + 1 + bx].predug = Lego_PredugType_Cavern_Exposed;
        }

        if (level->blocks[(by + 1) * level->width + bx].predug == Lego_PredugType_Wall)
        {
            Map3D_MoveBlockVertices(level->map, bx, by + 1, level->DigDepth);
            level->blocks[(by + 1) * level->width + bx].predug = Lego_PredugType_Cavern_Exposed;
        }

        ANGLES[0].x = -1;
        ANGLES[0].y = -1;
        ANGLES[1].x = 0;
        ANGLES[1].y = -1;
        ANGLES[2].x = 1;
        ANGLES[2].y = -1;
        ANGLES[3].x = -1;
        ANGLES[3].y = 0;
        ANGLES[4].x = 1;
        ANGLES[4].y = 0;
        ANGLES[5].x = -1;
        ANGLES[5].y = 1;
        ANGLES[6].x = 0;
        ANGLES[6].y = 1;
        ANGLES[7].x = 1;
        ANGLES[7].y = 1;

        // TODO: Implement Level_DestroyWall
    }

    return TRUE;
}

void Level_BlockUpdateSurface(lpLego_Level level, S32 bx, S32 by, B32 reserved)
{
    // TODO: Implement Level_BlockUpdateSurface
}
