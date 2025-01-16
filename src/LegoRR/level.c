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
    // TODO: According to ghidra a bunch of code was removed as unreachable,
    //       we should double check the assembly, and add all the code that was removed.
    //       It might not be intentionally dead code.
    //       The game is quite buggy, this might be an indication of code that if there, would fix a bug.
    if (level->width - 1 <= (U32)bx || level->height - 1 <= (U32)by)
        return;

    lpMap3D map = level->map;
    S32 blockIndex = by * level->width + bx;
    lpLego_Block blocks = level->blocks;

    const Point2I DIRECTIONS[4] = {
        {0, 0},
        {1, 0},
        {1, 1},
        {0, 1},
    };

    B32 YES_BLOCKFADE = TRUE;

    B32 isPath = blocks[blockIndex].flags1 & BLOCK1_PATH;

    if (!isPath || (blocks[blockIndex].flags2 & BLOCK2_PATHSBUILTCOUNTED) != 0)
    {
        if (!isPath && (blocks[blockIndex].flags2 & BLOCK2_PATHSBUILTCOUNTED) != 0)
        {
            LegoObject_IncLevelPathsBuilt(FALSE);
            blocks[blockIndex].flags2 &= ~BLOCK2_PATHSBUILTCOUNTED;
        }
    }
    else
    {
        LegoObject_IncLevelPathsBuilt(TRUE);
        blocks[blockIndex].flags2 |= BLOCK2_PATHSBUILTCOUNTED;
    }

    U32 exposedCaverns = 0;
    U32 unknown = 0;

    // Get surrounding blocks for orientation?
    for (U32 i = 0; i < 4; i++)
    {
        // TODO: Verify this is correct
        if (blocks[(by + DIRECTIONS[i].y) * level->width + (bx + DIRECTIONS[i].x)].predug == Lego_PredugType_Cavern_Exposed)
        {
            // TODO: Verify this is correct
            exposedCaverns++;
        }
        else
        {
            // TODO: Verify this is correct
            unknown = i + 1;
        }
    }

    BlockFlags1 ogFlags = blocks[blockIndex].flags1;
    BlockFlags1 newFlags = ogFlags & ~(BLOCK1_FLOOR|BLOCK1_WALL|BLOCK1_INCORNER|BLOCK1_OUTCORNER|BLOCK1_DIAGONAL);
    blocks[blockIndex].flags1 = newFlags;

    if (exposedCaverns == 4)
    {
        // TODO: Implement Level_BlockUpdateSurface
    }

    if (exposedCaverns != 2)
    {
        if (exposedCaverns == 1 || exposedCaverns == 3)
        {
            // TODO: Implement Level_BlockUpdateSurface
        }
        else
        {
            // TODO: Implement Level_BlockUpdateSurface
        }
    }

    blocks[blockIndex].flags1 = newFlags | (BLOCK1_SURVEYED | BLOCK1_WALL);

    const Point2I DIRECTIONS2[6] = {
        {0, 1},
        {1, 2},
        {2, 3},
        {0, 3},
        {0, 2},
        {1, 3},
    };

    Direction dir;

    for (U32 i = 0; i < 6; i++)
    {
        // TODO: Verify this is correct and figure out what it's trying to do
        if (DIRECTIONS2[i].x == (bx & 0xFF) && DIRECTIONS2[i].y == ((U32)bx >> 8 & 0xFF))
        {
            dir = (U8)level->width;
            goto somelabel;
        }
    }

    dir = (U8)bx;

somelabel:
    if (dir > 3)
    {
        if (blocks[blockIndex].texture - 1 < 5)
            blocks[blockIndex].texture = TEXTURE_DIAGONAL_STD;

        blocks[blockIndex].flags1 &= ~(BLOCK1_FLOOR | BLOCK1_INCORNER | BLOCK1_OUTCORNER);
        blocks[blockIndex].flags1 |= (BLOCK1_SURVEYED | BLOCK1_WALL | BLOCK1_DIAGONAL);

        blocks[blockIndex].direction = dir - 4;
        Map3D_SetBlockRotated(map, bx, by, (U8)(dir - 4) & 1);

        goto somelabel2;
    }

    if ((ogFlags & BLOCK1_REINFORCED) != 0)
    {
        switch (blocks[blockIndex].terrain)
        {
            case Lego_SurfaceType8_Immovable:
            case Lego_SurfaceType8_Water:
                blocks[blockIndex].texture = TEXTURE_REINWALL_IMM;
                blocks[blockIndex].direction = dir;
            break;
            case Lego_SurfaceType8_Hard:
                blocks[blockIndex].texture = TEXTURE_REINWALL_HARD;
                blocks[blockIndex].direction = dir;
            break;
            case Lego_SurfaceType8_Medium:
                blocks[blockIndex].texture = TEXTURE_REINWALL_MED;
                blocks[blockIndex].direction = dir;
            break;
            case Lego_SurfaceType8_Loose:
                blocks[blockIndex].texture = TEXTURE_REINWALL_LOOSE;
                blocks[blockIndex].direction = dir;
            break;
            case Lego_SurfaceType8_Soil:
                blocks[blockIndex].texture = TEXTURE_REINWALL_SOIL;
                blocks[blockIndex].direction = dir;
            break;
            default:
                blocks[blockIndex].direction = dir;
            break;
        }

        goto somelabel2;
    }

    // Probably randomness to choose an alternative texture style (which doesn't exist).
    if ((S32)blocks[blockIndex].randomness % 10 == 0)
    {
        switch (blocks[blockIndex].terrain)
        {
            case Lego_SurfaceType8_Immovable:
            case Lego_SurfaceType8_Water:
                blocks[blockIndex].texture = TEXTURE_WALL_IMM;
                blocks[blockIndex].direction = dir;
            break;
            case Lego_SurfaceType8_Hard:
                blocks[blockIndex].texture = TEXTURE_WALL_HARD;
                blocks[blockIndex].direction = dir;
            break;
            case Lego_SurfaceType8_Medium:
                blocks[blockIndex].texture = TEXTURE_WALL_MED;
                blocks[blockIndex].direction = dir;
            break;
            case Lego_SurfaceType8_Loose:
                blocks[blockIndex].texture = TEXTURE_WALL_LOOSE;
                blocks[blockIndex].direction = dir;
            break;
            case Lego_SurfaceType8_Soil:
                blocks[blockIndex].texture = TEXTURE_WALL_SOIL;
                blocks[blockIndex].direction = dir;
            break;
            case Lego_SurfaceType8_OreSeam:
                blocks[blockIndex].texture = TEXTURE_WALL_ORESEAM;
                blocks[blockIndex].direction = dir;
            break;
            case Lego_SurfaceType8_CrystalSeam:
                blocks[blockIndex].texture = TEXTURE_WALL_CRYSTALSEAM;
                blocks[blockIndex].direction = dir;
            break;
            case Lego_SurfaceType8_RechargeSeam:
                blocks[blockIndex].texture = TEXTURE_WALL_RECHARGESEAM;
                blocks[blockIndex].direction = dir;
            break;
            default:
                blocks[blockIndex].direction = dir;
            break;
        }
    }
    else
    {
        switch (blocks[blockIndex].terrain)
        {
            case Lego_SurfaceType8_Immovable:
            case Lego_SurfaceType8_Water:
                blocks[blockIndex].texture = TEXTURE_WALL_IMM;
                blocks[blockIndex].direction = dir;
            break;
            case Lego_SurfaceType8_Hard:
                blocks[blockIndex].texture = TEXTURE_WALL_HARD;
                blocks[blockIndex].direction = dir;
            break;
            case Lego_SurfaceType8_Medium:
                blocks[blockIndex].texture = TEXTURE_WALL_MED;
                blocks[blockIndex].direction = dir;
            break;
            case Lego_SurfaceType8_Loose:
                blocks[blockIndex].texture = TEXTURE_WALL_LOOSE;
                blocks[blockIndex].direction = dir;
            break;
            case Lego_SurfaceType8_Soil:
                blocks[blockIndex].texture = TEXTURE_WALL_SOIL;
                blocks[blockIndex].direction = dir;
            break;
            case Lego_SurfaceType8_OreSeam:
                blocks[blockIndex].texture = TEXTURE_WALL_ORESEAM;
                blocks[blockIndex].direction = dir;
            break;
            case Lego_SurfaceType8_CrystalSeam:
                blocks[blockIndex].texture = TEXTURE_WALL_CRYSTALSEAM;
                blocks[blockIndex].direction = dir;
            break;
            case Lego_SurfaceType8_RechargeSeam:
                blocks[blockIndex].texture = TEXTURE_WALL_RECHARGESEAM;
                blocks[blockIndex].direction = dir;
            break;
            default:
                blocks[blockIndex].direction = dir;
            break;
        }
    }

somelabel2:
    ogFlags = blocks[blockIndex].flags1;
    if ((ogFlags & BLOCK1_SURVEYED) != 0 && (ogFlags & BLOCK1_DIGREQUEST) != 0 &&
        blocks[blockIndex].terrain == Lego_SurfaceType8_Immovable ||
        blocks[blockIndex].terrain == Lego_SurfaceType8_Water || (ogFlags & BLOCK1_FLOOR) != 0)
    {
        blocks[blockIndex].flags1 = ogFlags & ~BLOCK1_DIGREQUEST;
        Map3D_ClearBlockHighlight(map, bx, by);
    }

    MainCLFlags clFlags = Main_GetCLFlags();

    B32 blockFade = YES_BLOCKFADE;
    if ((clFlags & CL_FLAG_BLOCKFADE) == 0)
        blockFade = FALSE;

    if (blockFade == FALSE)
        Map3D_SetBlockTexture(map, bx, by, blocks[blockIndex].texture, blocks[blockIndex].direction);
    else
        Map3D_SetBlockFadeInTexture(map, bx, by, blocks[blockIndex].texture, blocks[blockIndex].direction);

    if (level->UseRoof != BOOL3_FALSE)
        Level_Block_LowerRoofVertices(level, bx, by);
}

void Level_Block_LowerRoofVertices(lpLego_Level level, U32 bx, U32 by)
{
    // TODO: Implement Level_Block_LowerRoofVertices
}

B32 Level_AddCryOreToToolStore(lpLegoObject liveObj, SearchAddCryOre_c* search)
{
    // TODO: Implement Level_AddCryOreToToolStore
    return FALSE;
}
