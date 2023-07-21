#pragma once

#include "platform.h"
#include "lws.h"
#include <d3drm.h>

#define ANIMCLONE_MAXVISUALS 4

typedef struct AnimClone
{
    struct AnimClone *clonedFrom;

    struct Lws_Info *scene;
    LPDIRECT3DRMANIMATIONSET2 animSet;
    B32 lws;

    LPDIRECT3DRMFRAME3 root;
    LPDIRECT3DRMFRAME3 *partArray;
    U32 partCount;
    U32 frameCount;
}AnimClone, *lpAnimClone;

extern lpAnimClone AnimClone_RegisterLws(lpLws_Info scene, LPDIRECT3DRMFRAME3 root, U32 frameCount);
