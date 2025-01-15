#include "detail.h"

#include <stdio.h>

#include "map3d.h"
#include "mem.h"

void Dust_Setup(lpContainer cont, const char* path)
{
    // Empty function, address in rockfall beta is 0x00410190
}

Detail_TextureSet* Detail_LoadTextureSet(const char* textureBaseName, U32 width, U32 height)
{
    Detail_TextureSet* textureSet = Mem_Alloc(sizeof(Detail_TextureSet));

    if (textureSet == NULL)
        return NULL;

    textureSet->gridSurfaces = Mem_Alloc(height * width * 4);
    if (textureSet->gridSurfaces == NULL)
    {
        Mem_Free(textureSet);
        return NULL;
    }

    textureSet->gridSize.width = width;
    textureSet->gridSize.height = height;

    char buff[1024];

    for (U32 y = 0; y < height; y++)
    {
        for (U32 x = 0; x < width; x++)
        {
            sprintf(buff, "%s%0.1x%0.1x.bmp", textureBaseName, x, y);
            textureSet->gridSurfaces[y * width + x] = Container_LoadTextureImmediate(buff, 0, 0);
        }
    }

    return textureSet;
}

lpContainer_Texture Detail_GetTexture(lpDetail_TextureSet textureSet, SurfaceTexture texture)
{
    if (textureSet != NULL)
        return textureSet->gridSurfaces[(texture & 0xF) * textureSet->gridSize.width + (U32)(texture >> 4)];

    return NULL;
}
