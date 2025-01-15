#pragma once

#include "platform.h"
#include "container.h"

typedef struct Detail_TextureSet
{
    Size2I gridSize;
    Container_Texture** gridSurfaces;
} Detail_TextureSet, *lpDetail_TextureSet;

extern void Dust_Setup(lpContainer cont, const char* path);

extern Detail_TextureSet* Detail_LoadTextureSet(const char* textureBaseName, U32 width, U32 height);

extern lpContainer_Texture Detail_GetTexture(lpDetail_TextureSet textureSet, enum SurfaceTexture texture);
