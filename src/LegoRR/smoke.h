#pragma once

#include "platform.h"
#include "mesh.h"

typedef struct Smoke
{
    // TODO: Implement Smoke
    lpMesh mesh;
    // TODO: Implement Smoke
} Smoke, *lpSmoke;

extern B32 Smoke_Initialize();

extern void Smoke_LoadTextures(const char* dirname, const char* basename, U32 textureCount);

extern void Smoke_Hide(lpSmoke smoke, B32 hide);
