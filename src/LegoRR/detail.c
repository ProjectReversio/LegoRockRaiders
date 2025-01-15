#include "detail.h"

#include "mem.h"

void Dust_Setup(lpContainer cont, const char* path)
{
    // Empty function, address in rockfall beta is 0x00410190
}

Detail_TextureSet* Detail_LoadTextureSet(const char* textureBaseName, U32 width, U32 height)
{
    Detail_TextureSet* textureSet = Mem_Alloc(sizeof(Detail_TextureSet));

    // TODO: Implement Detail_LoadTextureSet

    return textureSet;
}
