#pragma once

#include "platform.h"
#include "container.h"

typedef struct Detail_TextureSet
{
    Size2I gridSize;
    Container_Texture** gridSurfaces;
} Detail_TextureSet, *lpDetail_TextureSet;

extern void Dust_Setup(lpContainer cont, const char* path);
