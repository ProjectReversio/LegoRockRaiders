#include "level.h"

#include "aitask.h"
#include "construction.h"
#include "erode.h"
#include "interface.h"
#include "pointer.h"
#include "smoke.h"

B32 Level_DestroyWall(lpLego_Level level, U32 bx, U32 by, B32 isPredug)
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

        for (U32 i = 0; i < 8; i++)
        {
            S32 ax = ANGLES[i].x + bx;
            S32 ay = ANGLES[i].y + by;
            Lego_Block* neighborBlock = &level->blocks[ay * level->width + ax];
            if ((neighborBlock->flags1 & BLOCK1_FLOOR) == BLOCK1_NONE)
            {
                neighborBlock->flags1 |= BLOCK1_WALL;
                neighborBlock->flags1 &= ~BLOCK1_DESTROYEDCONNECTION;
            }
        }

        level->blocks[by * level->width + bx].flags1 |= BLOCK1_FLOOR;

        Point2I blockPos = { bx, by };
        Level_BlockActivity_Destroy(level, blockPos, TRUE);

        level->blocks[by * level->width + bx].flags1 &= ~BLOCK1_REINFORCED;
        level->blocks[by * level->width + bx].flags1 |= BLOCK1_EXPOSEDFLOORCHECKS;

        BlockFlags1 flags1 = level->blocks[by * level->width + bx].flags1;

        if (((flags1 & BLOCK1_EXPOSED) == BLOCK1_NONE || (flags1 & BLOCK1_HIDDEN) != BLOCK1_NONE) && (flags1 & BLOCK1_DIGREQUEST) != BLOCK1_NONE)
        {
            RewardQuota_WallDestroyed();
        }

        if (isPredug)
        {
            level->blocks[by * level->width + bx].flags1 |= BLOCK1_CLEARED;
        }
        else
        {
            for (U32 ly = by - 1; ly <= by + 1; ly++)
            {
                for (U32 lx = bx - 1; lx <= bx + 1; lx++)
                {
                    if ((level->blocks[ly * level->width + lx].flags1 & BLOCK1_HIDDEN) != BLOCK1_NONE)
                    {
                        Level_UncoverHiddenCavern(lx, ly);

                        // TODO: Implement Level_DestroyWall
                    }
                }
            }

            // TODO: Implement Level_DestroyWall

            Point2I clearPos;
            clearPos.x = bx;
            clearPos.y = by;

            for (U32 i = 0; i < 4; i++)
            {
                AITask_DoClear_AtPosition(&clearPos, Message_ClearComplete);
            }

            legoGlobs.currLevel->blocks[legoGlobs.currLevel->width * by + bx].flags1 |= BLOCK1_RUBBLE_FULL;
        }

        Point2I digRefPos;
        digRefPos.x = bx;
        digRefPos.y = by;
        AITask_RemoveDigReferences(&digRefPos, FALSE);

        for (U32 ly = by - 1; ly < by + 2; ly++)
        {
            for (U32 lx = bx - 1; lx < bx + 2; lx++)
            {
                Level_BlockUpdateSurface(level, lx, ly, isPredug);
            }
        }

        level->blocks[by * level->width + bx].flags1 &= ~BLOCK1_DIGREQUEST;

        for (U32 ly = by - 1; ly <= by + 2; ly++)
        {
            for (U32 lx = bx - 1; lx <= bx + 2; lx++)
            {
                Map3D_SetBlockVertexModified(level->map, lx, ly);
            }
        }

        if (!isPredug && (legoGlobs.flags2 & GAME2_GENERATESPIDERS) != GAME2_NONE)
        {
            // TODO: Implement Level_DestroyWall
        }

        // TODO: Implement Level_DestroyWall

        if ((level->blocks[by * level->width + bx].flags2 & BLOCK2_SLUGHOLE_HIDDEN) != BLOCK2_NONE)
        {
            // TODO: Implement Level_DestroyWall
        }

        if (!isPredug)
        {
            // TODO: Implement Level_DestroyWall
            return TRUE;
        }
    }

    return TRUE;
}

B32 Level_DestroyWallConnection(lpLego_Level level, U32 bx, U32 by)
{
    // TODO: Implement Level_DestroyWallConnection
    return FALSE;
}

void Level_UncoverHiddenCavern(U32 bx, U32 by)
{
    // TODO: Implement Level_UncoverHiddenCavern
}

// Increases damage on the block, used in mining
B32 Level_Block_Damage(U32 bx, U32 by, F32 param3, F32 param4)
{
    lpLego_Level level = legoGlobs.currLevel;

    // Divide-by-zero guard
    if (param3 != 0.0f)
    {
        S32 idx = by * level->width + bx;
        level->blocks[idx].damage += param4 / param3;
        if (level->blocks[idx].damage > 1.0f)
            return TRUE;
    }
    return FALSE;
}

Lego_SurfaceType Level_Block_ChoosePathTexture(S32 bx, S32 by, U8* ref_direction, B32 powered)
{
    // TODO: Implement Level_Block_ChoosePathTexture

    return TEXTURE_FLOOR_PATH_1SIDES; // TEMP:
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
    Direction nonExposedCavernsDirections[4] = {0};
    U32 nonExposedCavernCount = 0;
    Direction lastExposedCavernDirection = 0;

    // Get surrounding blocks for orientation?
    for (Direction i = 0; i < 4; i++)
    {
        if (blocks[(by + DIRECTIONS[i].y) * level->width + (bx + DIRECTIONS[i].x)].predug == Lego_PredugType_Cavern_Exposed)
        {
            lastExposedCavernDirection = i;

            exposedCaverns++;
        }
        else
        {
            nonExposedCavernsDirections[nonExposedCavernCount] = i;

            nonExposedCavernCount++;
        }
    }

    BlockFlags1 ogFlags = blocks[blockIndex].flags1;
    BlockFlags1 newFlags = ogFlags & ~(BLOCK1_FLOOR|BLOCK1_WALL|BLOCK1_INCORNER|BLOCK1_OUTCORNER|BLOCK1_DIAGONAL);
    blocks[blockIndex].flags1 = newFlags;

    if (exposedCaverns == 4)
    {
        Point2I blockPos = { bx, by };

        blocks[blockIndex].flags1 = ogFlags & ~(BLOCK1_FLOOR | BLOCK1_WALL | BLOCK1_REINFORCED | BLOCK1_INCORNER | BLOCK1_OUTCORNER | BLOCK1_DIAGONAL) | (BLOCK1_SURVEYED | BLOCK1_FLOOR);
        blocks[blockIndex].direction = DIRECTION_UP;

        if ((ogFlags & BLOCK1_FLOOR) == BLOCK1_NONE)
        {
            // TODO: Implement Level_BlockUpdateSurface
        }

        if ((blocks[blockIndex].flags1 & BLOCK1_EXPOSEDFLOORCHECKS) == BLOCK1_NONE)
        {
            U32 someIdx = legoGlobs.currLevel->width * by + bx;
            // TODO: Is this correct?
            if ((legoGlobs.currLevel->blocks[someIdx].flags1 & BLOCK1_BUILDINGSOLID) == BLOCK1_NONE &&
                ((legoGlobs.currLevel->blocks[someIdx].flags2 & 0xFF) & BLOCK2_TOOLSTORE) == BLOCK2_NONE &&
                ((legoGlobs.currLevel->blocks[someIdx].flags2 & 0xFF) & BLOCK2_EMERGE_TRIGGER) == BLOCK2_NONE &&
                !Construction_Zone_ExistsAtBlock(&blockPos))
            {
                legoGlobs.currLevel->blocks[someIdx].flags1 &= ~BLOCK1_CLEARED;
                legoGlobs.currLevel->blocks[someIdx].flags1 &= ~BLOCK1_PATH;
                legoGlobs.currLevel->blocks[someIdx].flags1 &= ~BLOCK1_LAYEDPATH;
                LegoObject_RequestPowerGridUpdate();
                AITask_RemoveAttackPathReferences(&blockPos);

                //unknownemptyfunc(&blockPos); // TODO: Figure out what this was

                for (U32 i = 0; i < 4; i++)
                {
                    AITask_DoClear_AtPosition(&blockPos, Message_ClearComplete);
                }

                legoGlobs.currLevel->blocks[someIdx].flags1 |= BLOCK1_RUBBLE_FULL;
            }

            legoGlobs.currLevel->blocks[someIdx].flags1 |= BLOCK1_EXPOSEDFLOORCHECKS;
            Interface_BackToMain_IfSelectedWall_IsBlockPos(&blockPos);
        }

        // TODO: Implement Level_BlockUpdateSurface

        if (blocks[blockIndex].terrain == Lego_SurfaceType8_Lava || (ogFlags = blocks[blockIndex].flags1, (ogFlags & BLOCK1_ERODEACTIVE) != BLOCK1_NONE))
        {
            Erode_Block_FUN_0040ed80(&blockPos, TRUE);

            if ((blocks[blockIndex].flags1 & BLOCK1_NOTHOT) == BLOCK1_NONE)
            {
                if (blocks[blockIndex].erodeLevel == 4)
                {
                    blocks[blockIndex].texture = TEXTURE_FLOOR_LAVA;
                    Map3D_SetBlockUVWobbles(level->map, bx, by, TRUE);
                    if ((blocks[blockIndex].flags1 & BLOCK1_SMOKE) == BLOCK1_NONE)
                    {
                        // TODO: Implement Level_BlockUpdateSurface
                    }
                    else
                    {
                        Smoke_Hide(blocks[blockIndex].smoke, FALSE);
                    }
                }
                else
                {
                    Map3D_SetBlockUVWobbles(level->map, bx, by, FALSE);
                    switch (blocks[blockIndex].erodeLevel)
                    {
                        case 0:
                            blocks[blockIndex].texture = TEXTURE_FLOOR_ERODE_LOW;
                        break;
                        case 1:
                            blocks[blockIndex].texture = TEXTURE_FLOOR_ERODE_MED;
                        break;
                        case 2:
                            blocks[blockIndex].texture = TEXTURE_FLOOR_ERODE_HIGH;
                        break;
                        case 3:
                            blocks[blockIndex].texture = TEXTURE_FLOOR_ERODE_MAX;
                        break;
                    }
                }
            }
            else
            {
                Map3D_SetBlockUVWobbles(level->map, bx, by, FALSE);
                blocks[blockIndex].texture = TEXTURE_FLOOR_LAVA_NOTHOT;

                if ((blocks[blockIndex].flags1 & BLOCK1_SMOKE) != BLOCK1_NONE)
                {
                    Smoke_Hide(blocks[blockIndex].smoke, TRUE);
                }
            }
        }
        else if (blocks[blockIndex].terrain == Lego_SurfaceType8_Lake)
        {
            blocks[blockIndex].texture = TEXTURE_FLOOR_WATER;
            Map3D_SetBlockUVWobbles(level->map, bx, by, TRUE);
        }
        else if ((ogFlags & BLOCK1_FOUNDATION) == BLOCK1_NONE)
        {
            if ((ogFlags & BLOCK1_PATH) == BLOCK1_NONE)
            {
                if (((blocks[blockIndex].flags2 & 0xFF) & BLOCK2_SLUGHOLE_EXPOSED) == 0)
                {
                    if ((ogFlags & BLOCK1_CLEARED) == BLOCK1_NONE)
                    {
                        switch (ogFlags & BLOCK1_RUBBLE_FULL)
                        {
                            case BLOCK1_NONE:
                            {
                                blocks[blockIndex].texture = TEXTURE_FLOOR_RUBBLE_LOW;
                                break;
                            }
                            case BLOCK1_RUBBLE_LOW:
                            {
                                blocks[blockIndex].texture = TEXTURE_FLOOR_RUBBLE_MED;
                                break;
                            }
                            case BLOCK1_RUBBLE_MEDIUM:
                            {
                                blocks[blockIndex].texture = TEXTURE_FLOOR_RUBBLE_HIGH;
                                break;
                            }
                            case BLOCK1_RUBBLE_FULL:
                            {
                                blocks[blockIndex].texture = TEXTURE_FLOOR_RUBBLE_MAX;
                                break;
                            }
                        }
                    }
                    else if ((ogFlags & BLOCK1_LAYEDPATH) == BLOCK1_NONE)
                    {
                        blocks[blockIndex].texture = TEXTURE_FLOOR_STD;
                    }
                    else
                    {
                        blocks[blockIndex].texture = TEXTURE_FLOOR_PATH_LAYED;
                    }
                }
                else
                {
                    blocks[blockIndex].texture = TEXTURE_FLOOR_SLUGHOLE;
                }
            }
            else
            {
                blocks[blockIndex].texture = Level_Block_ChoosePathTexture(bx, by, &blocks[blockIndex].direction, blocks[blockIndex].flags2 & BLOCK2_POWERED);
            }
        }
        else if ((blocks[blockIndex].flags2 & BLOCK2_POWERED) == BLOCK2_NONE)
        {
            blocks[blockIndex].texture = TEXTURE_FLOOR_PATH_FOUNDATION;
        }
        else
        {
            blocks[blockIndex].texture = TEXTURE_FLOOR_POWERED_FOUNDATION;
        }

        goto somelabel2;
    }

    if (exposedCaverns != 2)
    {
        if (exposedCaverns == 1 || exposedCaverns == 3)
        {
            blocks[blockIndex].flags1 = ogFlags & ~(BLOCK1_FLOOR | BLOCK1_WALL | BLOCK1_REINFORCED | BLOCK1_INCORNER | BLOCK1_OUTCORNER | BLOCK1_DIAGONAL) | (BLOCK1_SURVEYED | BLOCK1_WALL);

            if (exposedCaverns == 1)
            {
                switch (blocks[blockIndex].terrain)
                {
                    case Lego_SurfaceType8_Immovable:
                    case Lego_SurfaceType8_Water:
                        blocks[blockIndex].texture = TEXTURE_INCORNER_IMM;
                    break;
                    case Lego_SurfaceType8_Hard:
                        blocks[blockIndex].texture = TEXTURE_INCORNER_HARD;
                    break;
                    case Lego_SurfaceType8_Medium:
                        blocks[blockIndex].texture = TEXTURE_INCORNER_MED;
                    break;
                    case Lego_SurfaceType8_Loose:
                        blocks[blockIndex].texture = TEXTURE_INCORNER_LOOSE;
                    break;
                    case Lego_SurfaceType8_Soil:
                        blocks[blockIndex].texture = TEXTURE_INCORNER_SOIL;
                    break;
                }

                blocks[blockIndex].flags1 = ogFlags & ~(BLOCK1_FLOOR | BLOCK1_WALL | BLOCK1_REINFORCED | BLOCK1_INCORNER | BLOCK1_OUTCORNER | BLOCK1_DIAGONAL) | (BLOCK1_SURVEYED | BLOCK1_WALL | BLOCK1_INCORNER);

                blocks[blockIndex].direction = DIRECTION_OPPOSITE(lastExposedCavernDirection);

                // TODO: Implement Level_BlockUpdateSurface
            }
            else if (exposedCaverns == 3)
            {
                switch (blocks[blockIndex].terrain)
                {
                    case Lego_SurfaceType8_Immovable:
                    case Lego_SurfaceType8_Water:
                        blocks[blockIndex].texture = TEXTURE_OUTCORNER_IMM;
                    break;
                    case Lego_SurfaceType8_Hard:
                        blocks[blockIndex].texture = TEXTURE_OUTCORNER_HARD;
                    break;
                    case Lego_SurfaceType8_Medium:
                        blocks[blockIndex].texture = TEXTURE_OUTCORNER_MED;
                    break;
                    case Lego_SurfaceType8_Loose:
                        blocks[blockIndex].texture = TEXTURE_OUTCORNER_LOOSE;
                    break;
                    case Lego_SurfaceType8_Soil:
                        blocks[blockIndex].texture = TEXTURE_OUTCORNER_SOIL;
                    break;
                }
                // TODO: Implement Level_BlockUpdateSurface

                blocks[blockIndex].direction = nonExposedCavernsDirections[0];
                blocks[blockIndex].flags1 |= BLOCK1_OUTCORNER;
            }

            F32 xPos, yPos, zPos, zPos2;
            Map3D_BlockVertexToWorldPos(map, bx, by, &xPos, &yPos, &zPos);
            Map3D_BlockVertexToWorldPos(map, bx + 1U, by + 1U, &xPos, &yPos, &zPos2);
            F32 diff = zPos - zPos2;

            Map3D_BlockVertexToWorldPos(map, bx + 1U, by, &xPos, &yPos, &zPos);
            Map3D_BlockVertexToWorldPos(map, bx, by + 1U, &xPos, &yPos, &zPos2);

            Map3D_SetBlockRotated(map, bx, by, fabs(zPos - zPos2) < fabs(diff));

            // TODO: Implement Level_BlockUpdateSurface

            goto somelabel2;
        }
        else
        {
            if ((ogFlags & BLOCK1_SURVEYED) != BLOCK1_NONE)
            {
                switch (blocks[blockIndex].terrain)
                {
                    case Lego_SurfaceType8_Immovable:
                    case Lego_SurfaceType8_Hard:
                    case Lego_SurfaceType8_Medium:
                    case Lego_SurfaceType8_Loose:
                    case Lego_SurfaceType8_Soil:
                        break;
                    default:
                        blocks[blockIndex].direction = DIRECTION_UP;
                        goto somelabel2;
                }
            }

            blocks[blockIndex].texture = TEXTURE_ROOF_STD;
            blocks[blockIndex].direction = DIRECTION_UP;
            goto somelabel2;
        }
    }

    //if (exposedCaverns == 2)

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

    U32 i = 0;
    while (DIRECTIONS2[i].x != nonExposedCavernsDirections[0] || DIRECTIONS2[i].y != nonExposedCavernsDirections[1])
    {
        i++;

        if (i >= 6)
        {
            dir = bx;
            goto somelabel;
        }
    }

    dir = i;

somelabel:
    if (dir > 3)
    {
        switch (blocks[blockIndex].terrain)
        {
            case Lego_SurfaceType8_Immovable:
            case Lego_SurfaceType8_Hard:
            case Lego_SurfaceType8_Medium:
            case Lego_SurfaceType8_Loose:
            case Lego_SurfaceType8_Soil:
                blocks[blockIndex].texture = TEXTURE_DIAGONAL_STD;
        }

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

void Level_SetPointer_FromSurfaceType(Lego_SurfaceType8 surfaceType)
{
    if (surfaceType == Lego_SurfaceType8_Immovable)
    {
        Pointer_SetCurrent_IfTimerFinished(Pointer_SurfaceType_Immovable);
        return;
    }

    if (surfaceType == Lego_SurfaceType8_Hard)
    {
        Pointer_SetCurrent_IfTimerFinished(Pointer_SurfaceType_Hard);
        return;
    }

    if (surfaceType == Lego_SurfaceType8_Medium)
    {
        Pointer_SetCurrent_IfTimerFinished(Pointer_SurfaceType_Medium);
        return;
    }

    if (surfaceType == Lego_SurfaceType8_Loose)
    {
        Pointer_SetCurrent_IfTimerFinished(Pointer_SurfaceType_Loose);
        return;
    }

    if (surfaceType == Lego_SurfaceType8_Soil)
    {
        Pointer_SetCurrent_IfTimerFinished(Pointer_SurfaceType_Soil);
        return;
    }

    if (surfaceType == Lego_SurfaceType8_OreSeam)
    {
        Pointer_SetCurrent_IfTimerFinished(Pointer_SurfaceType_OreSeam);
        return;
    }

    if (surfaceType == Lego_SurfaceType8_CrystalSeam)
    {
        Pointer_SetCurrent_IfTimerFinished(Pointer_SurfaceType_CrystalSeam);
        return;
    }

    if (surfaceType == Lego_SurfaceType8_RechargeSeam)
    {
        Pointer_SetCurrent_IfTimerFinished(Pointer_SurfaceType_RechargeSeam);
        return;
    }
}

B32 Level_BlockPointerCheck(Point2I* blockPos)
{
    // TODO: Implement Level_BlockPointerCheck

    return FALSE;
}

B32 Level_FindSelectedUnit_BlockCheck_FUN_00431960(U32 bx, U32 by, B32 param3)
{
    // TODO: Implement Level_FindSelectedUnit_BlockCheck_FUN_00431960

    return FALSE;
}

B32 Level_Block_IsImmovable(Point2I* blockPos)
{
    return legoGlobs.currLevel->blocks[blockPos->y * legoGlobs.currLevel->width + blockPos->x].terrain == Lego_SurfaceType8_Immovable;
}

B32 Level_Block_IsLava(Point2I* blockPos)
{
    return legoGlobs.currLevel->blocks[blockPos->y * legoGlobs.currLevel->width + blockPos->x].terrain == Lego_SurfaceType8_Lava;
}

B32 Level_Block_IsPath(Point2I* blockPos)
{
    // TODO: Cleanup decompiled code
    S32 x;
    S32 y;

    x = blockPos->x;
    if (((x > -1 && (y = blockPos->y, y > -1)) && (x < (S32)legoGlobs.currLevel->width)) &&
        (y < (S32)legoGlobs.currLevel->height))
    {
        return (legoGlobs.currLevel->blocks[legoGlobs.currLevel->width * y + x].flags1 & BLOCK1_PATH);
    }

    return FALSE;
}

// Generally used to tell Rock Monsters where they can't go.
B32 Level_Block_IsFoundationOrBusyFloor(Point2I* blockPos)
{
    if ((legoGlobs.currLevel->blocks[blockPos->y * legoGlobs.currLevel->width + blockPos->x].flags1 & (BLOCK1_FOUNDATION | BLOCK1_BUSY_FLOOR)) == BLOCK1_NONE)
        return FALSE;

    return TRUE;
}

B32 Level_Block_IsSurveyed(U32 bx, U32 by)
{
    if (bx < legoGlobs.currLevel->width - 1 && by < legoGlobs.currLevel->height - 1)
        return legoGlobs.currLevel->blocks[by * legoGlobs.currLevel->width + bx].flags1 & BLOCK1_SURVEYED;

    return FALSE;
}

B32 Level_Block_IsWall(U32 bx, U32 by)
{
    if (bx < legoGlobs.currLevel->width - 1 && by < legoGlobs.currLevel->height - 1)
    {
        return (legoGlobs.currLevel->blocks[by * legoGlobs.currLevel->width + bx].flags1 & BLOCK1_WALL) != BLOCK1_NONE &&
               (legoGlobs.currLevel->blocks[by * legoGlobs.currLevel->width + bx].flags1 & BLOCK1_FLOOR) == BLOCK1_NONE;
    }

    return FALSE;
}

void Level_Block_SetBusy(Point2I* blockPos, B32 state)
{
    // TODO: Implement Level_Block_SetBusy
}

void Level_BlockActivity_UpdateAll(lpLego_Level level, F32 elapsedGame)
{
    // TODO: Implement Level_BlockActivity_UpdateAll
}

// NOTE: This does not DIRECTLY remove the block activity. But it will subsequently mark it for
// removal in the next UpdateAll loop.
void Level_BlockActivity_Destroy(lpLego_Level level, Point2I blockPos, B32 wallDestroyed)
{
    // TODO: Implement Level_BlockActivity_Destroy
}

void Level_UpdateEffects(lpLego_Level level, F32 elapsedGame)
{
    // TODO: Implement Level_UpdateEffects
}

// 0 = Can't Cross, 1 = Can Cross (floor), 2 = diagonal, 3 = wall
CrossTerrainType Lego_GetCrossTerrainType(lpLegoObject liveObj, S32 bx1, S32 by1, S32 bx2, S32 by2, B32 param6)
{
    B32 checkWater = FALSE;
    B32 checkLand = TRUE;
    B32 checkLava = FALSE;
    B32 checkEnterToolStore = FALSE;

    Lego_Block block = legoGlobs.currLevel->blocks[by1 * legoGlobs.currLevel->width + bx1];

    const Point2I DIRECTIONS[4] = {
        {0, -1},
        {1, 0},
        {0, 1},
        {-1, 0},
    };

    B32 result = CrossTerrainType_CanCross;

    StatsFlags1 sflags1;
    if ((liveObj != NULL &&
        (sflags1 = StatsObject_GetStatsFlags1(liveObj),
        (sflags1 & STATS1_CANBEDRIVEN) != STATS1_NONE)) && liveObj->driveObject == NULL)
    {
        return CrossTerrainType_CantCross;
    }


    S32 i;
    for (i = 0; i < 4; i++)
    {
        if (DIRECTIONS[i].x + bx2 == bx1 && DIRECTIONS[i].y + by2 == by1)
            break;
    }

    B32 checkSingleWidthDig;
    if (liveObj == NULL)
    {
        checkSingleWidthDig = FALSE;
    }
    else
    {
        if (liveObj->type == LegoObject_ElectricFence)
        {
            return CrossTerrainType_CantCross;
        }

        checkEnterToolStore = StatsObject_GetStatsFlags3(liveObj) & STATS3_ENTERTOOLSTORE;
        checkSingleWidthDig = StatsObject_GetStatsFlags1(liveObj) & STATS1_SINGLEWIDTHDIG;
        checkWater = StatsObject_GetStatsFlags1(liveObj) & STATS1_CROSSWATER;
        checkLand = StatsObject_GetStatsFlags1(liveObj) & STATS1_CROSSLAND;
        checkLava = StatsObject_GetStatsFlags2(liveObj) & STATS2_CROSSLAVA;

        Point2I blockPos;
        if ((!param6 &&
            LegoObject_GetBlockPos(liveObj, &blockPos.x, &blockPos.y)) &&
            (blockPos.x == bx1 && blockPos.y == by1))
        {
            return CrossTerrainType_CanCross;
        }
    }

    if (block.terrain == Lego_SurfaceType8_Lake)
    {
        if (!checkWater)
        {
            return CrossTerrainType_CantCross;
        }
    }
    else if (!checkLand)
    {
        return CrossTerrainType_CantCross;
    }

    if ((block.terrain == Lego_SurfaceType8_Lava && !checkLava) &&
        (block.flags1 & BLOCK1_NOTHOT) == BLOCK1_NONE)
    {
        return CrossTerrainType_CantCross;
    }

    if ((block.flags1 & BLOCK1_BUILDINGSOLID) != BLOCK1_NONE)
    {
        return CrossTerrainType_CantCross;
    }

    if (param6 && (block.flags1 & BLOCK1_INCORNER) != BLOCK1_NONE)
    {
        return CrossTerrainType_CantCross;
    }

    if (!checkEnterToolStore && (block.flags2 & BLOCK2_TOOLSTORE) != BLOCK2_NONE)
    {
        return CrossTerrainType_CantCross;
    }

    if (!checkSingleWidthDig)
    {
        if ((block.flags1 & BLOCK1_FLOOR) == BLOCK1_NONE)
        {
            return CrossTerrainType_CantCross;
        }
    }
    else
    {
        if ((block.flags1 & BLOCK1_WALL) != BLOCK1_NONE)
        {
            result = CrossTerrainType_Wall;
        }

        if (block.predug == Lego_PredugType_Wall)
        {
            return CrossTerrainType_CantCross;
        }

        S32 index = by1 * legoGlobs.currLevel->width + bx1;

        B32 isWall = legoGlobs.currLevel->blocks[index].flags1 & BLOCK1_WALL;
        if (isWall)
        {
            if (legoGlobs.currLevel->blocks[index - 1].terrain == Lego_SurfaceType8_Lava)
            {
                return CrossTerrainType_CantCross;
            }

            if (legoGlobs.currLevel->blocks[(by1 - 1) * legoGlobs.currLevel->width + bx1].terrain == Lego_SurfaceType8_Lava)
            {
                return CrossTerrainType_CantCross;
            }
        }

        if (isWall)
        {
            if (legoGlobs.currLevel->blocks[index - 1].terrain == Lego_SurfaceType8_Water)
            {
                return CrossTerrainType_CantCross;
            }

            if (legoGlobs.currLevel->blocks[(by1 - 1) * legoGlobs.currLevel->width + bx1].terrain == Lego_SurfaceType8_Water)
            {
                return CrossTerrainType_CantCross;
            }
        }

        if (i == 0)
        {
            if ((block.flags1 & BLOCK1_DIAGONAL) != BLOCK1_NONE)
            {
                result = CrossTerrainType_Diagonal;
            }

            if ((legoGlobs.currLevel->blocks[by2 * legoGlobs.currLevel->width + bx2].flags1 & BLOCK1_DIAGONAL) != BLOCK1_NONE)
            {
                result = CrossTerrainType_Diagonal;
            }
        }
    }

    if (bx2 != bx1 && by2 != by1)
    {
        if ((legoGlobs.currLevel->blocks[by2 * legoGlobs.currLevel->width + bx1].flags1 & BLOCK1_FLOOR) != BLOCK1_NONE &&
            (legoGlobs.currLevel->blocks[by1 * legoGlobs.currLevel->width + bx2].flags1 & BLOCK1_FLOOR) != BLOCK1_NONE)
        {
            return CrossTerrainType_Diagonal;
        }
    }

    return result;
}
