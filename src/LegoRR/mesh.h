#pragma once

#include "platform.h"
#include "viewport.h"

typedef struct Mesh
{
    U32 groupCount, listSize;
    // TODO: Implement Mesh
} Mesh, *lpMesh;

extern B32 Mesh_Initialize(const char* sharedTextureDir);

extern void Mesh_PostRenderAll(lpViewport vp);
