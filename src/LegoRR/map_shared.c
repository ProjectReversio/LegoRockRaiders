#include "map_shared.h"

#include "map3d.h"
#include "mem.h"

void MapShared_GetDimensions(U32 memHandle, U32* outWidth, U32* outHeight)
{
    MapFileInfo* mapFile = (MapFileInfo*)Mem_AddressHandle(memHandle);

    *outWidth = mapFile->dimensions.width;
    *outHeight = mapFile->dimensions.height;
}

U32 MapShared_GetBlock(U32 memHandle, S32 x, S32 y)
{
    U32 width;
    U32 height;

    MapFileInfo* mapFile = (MapFileInfo*)Mem_AddressHandle(memHandle);
    MapShared_GetDimensions(memHandle, &width, &height);

    return (U32)mapFile->blocks[width * y + x];
}
