#include "map3d.h"
#include "file.h"
#include "mem.h"
#include "material.h"
#include "main.h"

lpMap3D Map3D_Create(lpContainer root, const char* filename, F32 blockSize, F32 roughLevel)
{
    lpMapFileInfo mapFileInfo = File_LoadBinary(filename, NULL);
    if (mapFileInfo == NULL)
        return NULL;

    lpMap3D map = Mem_Alloc(sizeof(Map3D));
    if (map == NULL)
    {
        Mem_Free(mapFileInfo);
        return NULL;
    }

    map->blockSize = blockSize;
    map->roughLevel = roughLevel;
    Map3D_MapFileGetSpecs(mapFileInfo, &map->gridWidth, &map->gridHeight);
    map->blockWidth = map->gridWidth - 1;
    map->blockHeight = map->gridHeight - 1;
    map->worldDimensions_fnegx.width = -((F32)(U64)(map->blockWidth) * blockSize * 0.5);
    map->worldDimensions_fnegx.height = ((F32)(U64)(map->blockHeight) * blockSize * 0.5);
    map->textureSet = NULL;

    memset(map->texsNum, 0, MAP3D_MAXTEXTURES);

    map->flagsMap = MAP3D_FLAG_NONE;
    map->visibleBlocksNum = 0;
    map->uvBlocksNum = 0;
    map->float_20 = 0.0f;

    // TODO: THIS MATERIAL IS NEVER USED, AND IS NEVER RELEASED!!!
    map->material = Material_Create(1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f);

    memset(map->transBlocks, 0, sizeof(Map3D_TransitionBlock) * 10);
    memset(map->texsNoFade, 0, sizeof(B32) * MAP3D_MAXTEXTURES);

    for (U32 i = 0; i < 10; i++)
        map->transBlocks[i].groupID = D3DRMGROUP_ALLGROUPS; // (not created yet)

    map->transMesh = Container_MakeMeshTrans(root);
    map->mesh = Container_MakeMeshImmediate(root);

    if (map->mesh != NULL)
    {
        map->blocks3D = Mem_Alloc(sizeof(Map3D_Block) * map->gridWidth * map->gridHeight);
        if (map->blocks3D != NULL)
        {
            U32 faceData[6];
            faceData[0] = 0;
            faceData[1] = 1;
            faceData[2] = 3;
            faceData[3] = 1;
            faceData[4] = 2;
            faceData[5] = 3;

            if (map->blockWidth * map->blockHeight != 0)
            {
                U32 iter = 0;
                do
                {
                    U32 group = Container_Mesh_AddGroup(map->mesh, 4, 2, 3, faceData);
                    Container_Mesh_HideGroup(map->mesh, group, TRUE);
                    Container_Mesh_SetQuality(map->mesh, group, Gouraud);

                    iter++;
                } while (iter < map->blockWidth * map->blockHeight);
            }

            if (map->gridHeight != 0)
            {
                U32 yIter = 0;
                do
                {
                    if (map->gridWidth != 0)
                    {
                        U32 xIter = 0;
                        do
                        {
                            map->blocks3D[yIter * map->gridWidth + xIter].flags3D = MAP3DBLOCK_FLAG_UNK_20 | MAP3DBLOCK_FLAG_UNK_40;
                            map->blocks3D[yIter * map->gridWidth + xIter].texture = TEXTURE_FLOOR_STD;
                            map->blocks3D[yIter * map->gridWidth + xIter].heightValue = Map3D_MapFileBlockValue(mapFileInfo, xIter, yIter, map->gridWidth);

                            F32 fVar8 = Maths_RandRange(0.0f, 1.0f);
                            F32 fVar8_2 = Maths_RandRange(0.0f, 1.0f);
                            F32 fVar9 = 1.0f / (F32)sqrt(fVar8 * fVar8 + fVar8_2 * fVar8_2);
                            map->blocks3D[yIter * map->gridWidth + xIter].uvCoord.x = fVar9 * fVar8 * 0.3f;
                            map->blocks3D[yIter * map->gridWidth + xIter].uvCoord.y = fVar9 * fVar8_2 * 0.3f;

                            map->blocks3D[yIter * map->gridWidth + xIter].highlight = 0;

                            xIter++;
                        } while (xIter < map->gridWidth);
                    }
                    yIter++;
                } while(yIter < map->gridHeight);
            }

            Map3D_InitRoughness(map);

            if (map->blockHeight != 0)
            {
                U32 yIter = 0;
                do
                {
                    U32 xIter = 0;
                    if (map->blockWidth != 0)
                    {
                        do
                        {
                            U32 iter = yIter * map->gridWidth + xIter;
                            U32 iterNext = (yIter + 1) * map->gridWidth + xIter;

                            // Compare crossed height differences
                            // Condition is true when abs(x0y0 - x1y1) < diff(x1y0 - x0y1).
                            S32 diffxy00_11 = map->blocks3D[iter].heightValue - map->blocks3D[iterNext + 1].heightValue;
                            S32 diffxy10_01 = map->blocks3D[iter + 1].heightValue - map->blocks3D[iterNext].heightValue;
                            // diffxy00_11 = (blocks3D[y][x].heightValue - blocks3D[y+1][x+1].heightValue)
                            // diffxy10_01 = (blocks3D[y][x+1].heightValue - blocks3D[y+1][x].heightValue)

                            if (abs(diffxy00_11) < abs(diffxy10_01))
                                Map3D_SetBlockRotated(map, xIter, yIter, TRUE);
                            else
                                Map3D_GenerateBlockPlaneNormals(map, xIter, yIter);

                            xIter++;
                        } while (xIter < map->blockWidth);
                    }

                    yIter++;
                } while(yIter < map->blockHeight);
            }

            Mem_Free(mapFileInfo);
            return map;
        }
        Container_Remove(map->mesh);
    }
    Mem_Free(map);
    Mem_Free(mapFileInfo);

    return NULL;
}

void Map3D_MapFileGetSpecs(lpMapFileInfo mapFileInfo, U32* width, U32* height)
{
    *width = mapFileInfo->dimensions.width;
    *height = mapFileInfo->dimensions.height;
}

U16 Map3D_MapFileBlockValue(lpMapFileInfo mapFile, U32 bx, U32 by, U32 gridWidth)
{
    return mapFile->blocks[gridWidth * by + bx];
}

B32 Map3D_BlockToWorldPos(lpMap3D map, U32 bx, U32 by, F32* outX, F32* outY)
{
    if (bx < map->blockWidth && by < map->blockHeight)
    {
        *outX = map->blockSize * 0.5f + (F32)bx * map->blockSize + map->worldDimensions_fnegx.width;
        *outY = map->worldDimensions_fnegx.height - (map->blockSize * 0.5f + (F32)by * map->blockSize);
        return TRUE;
    }
    return FALSE;
}

B32 Map3D_WorldToBlockPos(lpMap3D map, F32 x, F32 y, S32* outBx, S32* outBy, F32* outZ)
{
    // decompiled code
    /*
        longlong lVar4 = __ftol(((float10)in_x - (float10)(map->worldDimensions_fnegx).width) / (float10)map->blockSize);
        int xPos = (int)lVar4;
        lVar4 = __ftol(((float10)(map->worldDimensions_fnegx).height - (float10)in_y) / (float10)map->blockSize);
        int yPos = (int)lVar4;
    */

    // TODO: Verify that this is correct
    S32 xPos = (S32)(x - map->worldDimensions_fnegx.width / map->blockSize);
    S32 yPos = (S32)((map->worldDimensions_fnegx.height - y) / map->blockSize);

    if (outBx != NULL)
        *outBx = xPos;
    if (outBy != NULL)
        *outBy = yPos;

    if (-1 < xPos && xPos < (S32)map->blockWidth && -1 < yPos && yPos < (S32)map->blockHeight)
    {
        if (outZ != NULL)
        {
            F32 fVar1 = map->blockSize * 0.5f;
            x = (x - ((F32)xPos * map->blockSize + map->worldDimensions_fnegx.width) / fVar1);
            fVar1 = -((y - (map->worldDimensions_fnegx.height - (F32)yPos * map->blockSize)) / fVar1);
            if (1.0f < x)
                x = 2.0f - x;
            if (1.0f < fVar1)
                fVar1 = 2.0f - fVar1;
            if (x < fVar1)
                fVar1 = x;

            *outZ = fVar1;
        }
        return TRUE;
    }

    return FALSE;
}

B32 Map3D_WorldToBlockPos_NoZ(lpMap3D map, F32 x, F32 y, S32* outBx, S32* outBy)
{
    return Map3D_WorldToBlockPos(map, x, y, outBx, outBy, NULL);
}

F32 Map3D_GetWorldZ(lpMap3D map, F32 xPos, F32 yPos)
{
    // TODO: Implement Map3D_GetWorldZ
    return 0.0f;
}

F32 Map3D_UnkCameraXYFunc_RetZunk(lpMap3D map, F32 xPos, F32 yPos)
{
    // TODO: Implement Map3D_UnkCameraXYFunc_RetZunk

    return 0.0f;
}

void Map3D_InitRoughness(lpMap3D map)
{
    Vertex vertices[4];

    Point2F OFFSETS[4];

    OFFSETS[0].x = 0.0f;
    OFFSETS[0].y = 0.0f;
    OFFSETS[1].x = 1.0f;
    OFFSETS[1].y = 0.0f;
    OFFSETS[2].x = 1.0f;
    OFFSETS[2].y = 1.0f;
    OFFSETS[3].x = 0.0f;
    OFFSETS[3].y = 1.0f;

    U32 y = 0;
    if (map->blockHeight != 0)
    {
        do
        {
            U32 x = 0;
            if (map->blockWidth != 0)
            {
                do
                {
                    Point2F* pOffset = OFFSETS;

                    S32 i = 3;
                    do
                    {
                        vertices[i].position.x = ((F32)x + pOffset->x) * map->blockSize + map->worldDimensions_fnegx.width;
                        vertices[i].position.y = map->worldDimensions_fnegx.height - (pOffset->y + (F32)y) * map->blockSize;

                        vertices[i].position.z = -((F32)(U32)map->blocks3D[(y + (S32)pOffset->y) * map->gridWidth + x + (S32)pOffset->x].heightValue * map->roughLevel);
                        if (vertices[i].position.z < map->float_20)
                            map->float_20 = vertices[i].position.z;

                        pOffset++;
                        i--;
                    } while (i >= 0);

                    Container_Mesh_SetVertices(map->mesh, y * map->blockWidth + x, 0, 4, vertices);

                    x++;
                } while (x < map->blockWidth);
            }
            y++;
        } while (y < map->blockHeight);
    }
}

// This seems to be used when the height difference of diagonal vertex points differs.
//
// diffxy00_11 = (blocks3D[y][x].heightValue - blocks3D[y+1][x+1].heightValue)
// diffxy10_01 = (blocks3D[y][x+1].heightValue - blocks3D[y+1][x].heightValue)
// rotated = (abs(diffxy00_11) < std::abs(diffxy10_01));
void Map3D_SetBlockRotated(lpMap3D map, U32 bx, U32 by, B32 on)
{
    if ((on != 0) != Map3D_IsBlockRotated(map, bx, by))
    {
        Vertex vertices[4];
        Vertex vertexSwap;

        U32 groupID = map->blockWidth * by + bx;
        Container_Mesh_GetVertices(map->mesh, groupID, 0, 4, vertices);

        if ((map->blocks3D[by * map->gridWidth + bx].flags3D & MAP3DBLOCK_FLAG_ROTATED) == MAP3DBLOCK_FLAG_NONE)
        {
            map->blocks3D[by * map->gridWidth + bx].flags3D |= MAP3DBLOCK_FLAG_ROTATED;

            // Shift vertices up one
            memcpy(&vertexSwap,  &vertices[3], sizeof(D3DRMVERTEX));
            memcpy(&vertices[3], &vertices[2], sizeof(D3DRMVERTEX));
            memcpy(&vertices[2], &vertices[1], sizeof(D3DRMVERTEX));
            memcpy(&vertices[1], &vertices[0], sizeof(D3DRMVERTEX));
            memcpy(&vertices[0], &vertexSwap,  sizeof(D3DRMVERTEX));
        }
        else
        {
            map->blocks3D[by * map->gridWidth + bx].flags3D &= ~MAP3DBLOCK_FLAG_ROTATED;

            // Shift vertices down one
            memcpy(&vertexSwap,  &vertices[0], sizeof(D3DRMVERTEX));
            memcpy(&vertices[0], &vertices[1], sizeof(D3DRMVERTEX));
            memcpy(&vertices[1], &vertices[2], sizeof(D3DRMVERTEX));
            memcpy(&vertices[2], &vertices[3], sizeof(D3DRMVERTEX));
            memcpy(&vertices[3], &vertexSwap,  sizeof(D3DRMVERTEX));
        }

        Container_Mesh_SetVertices(map->mesh, groupID, 0, 4, vertices);

        Map3D_GenerateBlockPlaneNormals(map, bx, by);
        // Perform action in a square: { { 0, 0 }, { 1, 0 },
        //                               { 0, 1 }, { 1, 1 } }
        Map3D_SetBlockVertexModified(map, bx, by);
        Map3D_SetBlockVertexModified(map, bx + 1, by);
        Map3D_SetBlockVertexModified(map, bx + 1, by + 1);
        Map3D_SetBlockVertexModified(map, bx, by + 1);
    }
}

B32 Map3D_IsBlockRotated(lpMap3D map, U32 bx, U32 by)
{
    return (map->blocks3D[by * map->gridWidth + bx].flags3D & MAP3DBLOCK_FLAG_ROTATED) != MAP3DBLOCK_FLAG_NONE;
}

void Map3D_SetBlockVertexModified(lpMap3D map, U32 vx, U32 vy)
{
    map->blocks3D[map->gridWidth * vy + vx].flags3D |= MAP3DBLOCK_FLAG_VERTEXMODIFIED;
    map->flagsMap |= MAP3D_FLAG_VERTEXMODIFIED;
}

void Map3D_GenerateBlockPlaneNormals(lpMap3D map, U32 bx, U32 by)
{
    Vertex vertices[4];

    U32 width = map->blockWidth;

    if (bx < width && by < map->blockHeight)
    {
        Container_Mesh_GetVertices(map->mesh, by * width + bx, 0, 4, vertices);
        // (&surfMap->blocks3D[by * surfMap->gridWidth + bx].vector_0,
        //   &vertices[0].position, &vertices[1].position, &vertices[2].position)

        Maths_PlaneNormal(&map->blocks3D[by * map->gridWidth + bx].normalA, &vertices[0].position, &vertices[1].position, &vertices[3].position);
        // (&surfMap->blocks3D[by * surfMap-gridWidth + bx].vector_c,
        //   &vertices[1].position, &vertices[2].position, &vertices[3].position)

        // NOTE: index is shifted one after the previous call
        Maths_PlaneNormal(&map->blocks3D[by * map->gridWidth + bx].normalB, &vertices[1].position, &vertices[2].position, &vertices[3].position);
    }
}

void Map3D_SetEmissive(lpMap3D map, B32 on)
{
    // TODO: Implement Map3D_SetEmissive
}
