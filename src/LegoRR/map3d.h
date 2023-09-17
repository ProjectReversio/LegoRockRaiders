#pragma once

#include "common.h"
#include "container.h"
#include "detail.h"
#include "material.h"

#define MAP3D_MAXTEXTURES 128

typedef enum SurfaceTexture
{
    TEXTURE_FLOOR_STD = 0x0,
    TEXTURE_WALL_SOIL = 0x1,
    TEXTURE_WALL_LOOSE = 0x2,
    TEXTURE_WALL_MED = 0x3,
    TEXTURE_WALL_HARD = 0x4,
    TEXTURE_WALL_IMM = 0x5,
    TEXTURE_FLOOR_ERODE_LOW = 0x6,
    TEXTURE_FLOOR_07 = 0x7,
    TEXTURE_FLOOR_RUBBLE_MAX = 0x10,
    TEXTURE_FLOOR_RUBBLE_HIGH = 0x11,
    TEXTURE_FLOOR_RUBBLE_MED = 0x12,
    TEXTURE_FLOOR_RUBBLE_LOW = 0x13,
    TEXTURE_FLOOR_ERODE_MED = 0x16,
    TEXTURE_WALL_CRYSTALSEAM = 0x20,
    TEXTURE_REINWALL_SOIL = 0x21,
    TEXTURE_REINWALL_LOOSE = 0x22,
    TEXTURE_REINWALL_MED = 0x23,
    TEXTURE_REINWALL_HARD = 0x24,
    TEXTURE_REINWALL_IMM = 0x25,
    TEXTURE_FLOOR_ERODE_HIGH = 0x26,
    TEXTURE_FLOOR_SLUGHOLE = 0x30,
    TEXTURE_INCORNER_SOIL = 0x31,
    TEXTURE_INCORNER_LOOSE = 0x32,
    TEXTURE_INCORNER_MED = 0x33,
    TEXTURE_INCORNER_HARD = 0x34,
    TEXTURE_INCORNER_IMM = 0x35,
    TEXTURE_FLOOR_ERODE_MAX = 0x36,
    TEXTURE_WALL_ORESEAM = 0x40,
    TEXTURE_FLOOR_WATER = 0x45,
    TEXTURE_FLOOR_LAVA = 0x46,
    TEXTURE_FLOOR_LAVA_NOTHOT = 0x47,
    TEXTURE_OUTCORNER_SOIL = 0x51,
    TEXTURE_OUTCORNER_LOOSE = 0x52,
    TEXTURE_OUTCORNER_MED = 0x53,
    TEXTURE_OUTCORNER_HARD = 0x54,
    TEXTURE_OUTCORNER_IMM = 0x55,
    TEXTURE_FLOOR_PATH_4SIDES = 0x60,
    TEXTURE_FLOOR_PATH_LAYED = 0x61,
    TEXTURE_FLOOR_PATH_2SIDES = 0x62,
    TEXTURE_FLOOR_PATH_CORNER = 0x63,
    TEXTURE_FLOOR_PATH_3SIDES = 0x64,
    TEXTURE_FLOOR_PATH_1SIDES = 0x65,
    TEXTURE_FLOOR_POWERED_FOUNDATION = 0x66,
    TEXTURE_WALL_RECHARGESEAM = 0x67,
    TEXTURE_ROOF_STD = 0x70,
    TEXTURE_FLOOR_POWERED_4SIDES = 0x71,
    TEXTURE_FLOOR_POWERED_2SIDES = 0x72,
    TEXTURE_FLOOR_POWERED_CORNER = 0x73,
    TEXTURE_FLOOR_POWERED_3SIDES = 0x74,
    TEXTURE_FLOOR_POWERED_1SIDES = 0x75,
    TEXTURE_FLOOR_PATH_FOUNDATION = 0x76,
    TEXTURE_DIAGONAL_STD = 0x77,
    TEXTURE__INVALID = 0xff,
} SurfaceTexture;

typedef enum Map3DFlags
{
    MAP3D_FLAG_NONE           = 0,
    MAP3D_FLAG_VERTEXMODIFIED = 0x2,
    MAP3D_FLAG_EMISSIVE_4     = 0x4,
} Map3DFlags;

typedef enum Map3D_BlockFlags
{
    MAP3DBLOCK_FLAG_NONE           = 0,
    MAP3DBLOCK_FLAG_ROTATED        = 0x1,
    MAP3DBLOCK_FLAG_VERTEXMODIFIED = 0x4,
    MAP3DBLOCK_FLAG_VERTEXLEVELED  = 0x8,
    MAP3DBLOCK_FLAG_UVWOBBLES      = 0x10,
    MAP3DBLOCK_FLAG_UNK_20         = 0x20,
    MAP3DBLOCK_FLAG_UNK_40         = 0x40,
} Map3D_BlockFlags;

typedef enum Map3D_TransitionFlags
{
    MAP3DTRANS_FLAG_NONE = 0,
    MAP3DTRANS_FLAG_USED = 0x1,
} Map3D_TransitionFlags;

#pragma pack(push, 2)
typedef struct MapFileInfo
{
    char Signature[4]; // "MAP "
    U32 fileSize;
    Size2I dimensions;
    U16 blocks[1];
} MapFileInfo, *lpMapFileInfo;
#pragma pack(pop)

typedef struct Map3D_Block
{
    Point3F normalA; // Maths_PlaneNormal of vertex positions 0, 1, 3
    Point3F normalB; // Maths_PlaneNormal of vertex positions 1, 2, 3
    SurfaceTexture texture;
    Map3D_BlockFlags flags3D;
    U8 heightValue; // height block value taken from high.map file
    U8 padding1;
    Point2F uvCoord;
    U8 highlight; // (WallHighlightType)
    U8 padding2[3];
} Map3D_Block, *lpMap3D_Block;

typedef struct Map3D_TransitionBlock
{
    D3DRMGROUPINDEX groupID; // (init -1, no group created yet)
    F32 timer; // count-up timer [0.05f-1.0f]
    Point2I blockPos;
    Point2F uvCoords[4];
    U32 reserved1;
    Map3D_TransitionFlags flags; // (0x1 = visible, only flag)
    U32 padding1[3];
} Map3D_TransitionBlock, *lpMap3D_TransitionBlock;

typedef struct Map3D
{
    U32 blockWidth; // gridWidth - 1
    U32 blockHeight; // gridHeight - 1
    U32 gridWidth; // full map width
    U32 gridHeight; // full map height
    F32 blockSize; // (cfg: BlockSize)
    F32 roughLevel; // (cfg: RoughLevel)
    Size2F worldDimensions_fnegx; // (-width, +height) * BlockSize / 2.0f (smallDimensions)
    F32 float_20; // (some sort of maximum)
    lpContainer mesh; // Mesh for Map3D_Blocks
    lpMap3D_Block blocks3D; // [*:dimensions]
    Detail_TextureSet* textureSet;
    lpMaterial material; // (UNUSED: and never released!!!)
    U8 texsGrid[MAP3D_MAXTEXTURES][MAP3D_MAXTEXTURES];
    U8 texsNum[MAP3D_MAXTEXTURES];
    B32 texsNoFade[MAP3D_MAXTEXTURES];
    Coord2U visibleBlocksTable[500]; // (related to FP detail)
    U32 visibleBlocksNum;
    Coord2U uvBlocksTable[2500]; // Blocks that require UV updates (EMISSIVE or UVWOBBLES)
    U32 uvBlocksNum;
    Map3D_TransitionBlock transBlocks[10]; // [BASEMAPTEXTURES??]
    lpContainer transMesh; // Mesh for Map3D_TransitionBlocks
    Map3DFlags flagsMap;
} Map3D, *lpMap3D;

extern lpMap3D Map3D_Create(lpContainer root, const char* filename, F32 blockSize, F32 roughLevel);

extern void Map3D_MapFileGetSpecs(lpMapFileInfo mapFileInfo, U32* width, U32* height);

extern U16 Map3D_MapFileBlockValue(lpMapFileInfo mapFile, U32 bx, U32 by, U32 gridWidth);

extern void Map3D_InitRoughness(lpMap3D map);
