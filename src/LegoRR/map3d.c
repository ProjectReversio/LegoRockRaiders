#include "map3d.h"
#include "file.h"
#include "mem.h"
#include "material.h"

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

    // THIS MATERIAL IS NEVER USED, AND IS NEVER RELEASED!!!
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
            // TODO: Implement Map3D_Create

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
