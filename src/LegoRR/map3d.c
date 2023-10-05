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
                // TODO: Implement Map3D_Create
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

void Map3D_InitRoughness(lpMap3D map)
{
    // TODO: Implement Map3D_InitRoughness
}
