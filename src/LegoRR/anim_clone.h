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

extern lpAnimClone AnimClone_Register(LPDIRECT3DRMANIMATIONSET2 animSet, LPDIRECT3DRMFRAME3 root, U32 frameCount);
extern lpAnimClone AnimClone_RegisterLws(lpLws_Info scene, LPDIRECT3DRMFRAME3 root, U32 frameCount);

extern B32 AnimClone_WalkTree(LPDIRECT3DRMFRAME3 frame, U32 level, B32 (*Callback)(LPDIRECT3DRMFRAME3 frame, void* data), void* data);

extern B32 AnimClone_FrameCountCallback(LPDIRECT3DRMFRAME3 frame, void* data);
extern B32 AnimClone_SetupFrameArrayCallback(LPDIRECT3DRMFRAME3 frame, void* p);

extern void AnimClone_SetTime(lpAnimClone clone, F32 time, F32* oldTime);

extern void AnimClone_CreateCopy(LPDIRECT3DRMFRAME3 orig, LPDIRECT3DRMFRAME3 clone, B32 lws);
extern void AnimClone_CloneLwsMesh(LPDIRECT3DRMFRAME3 orig, LPDIRECT3DRMFRAME3 clone);
extern void AnimClone_ReferenceVisuals(LPDIRECT3DRMFRAME3 orig, LPDIRECT3DRMFRAME3 clone);

extern B32 AnimClone_IsLws(lpAnimClone clone);

extern lpAnimClone AnimClone_Make(lpAnimClone orig, LPDIRECT3DRMFRAME3 parent, U32* frameCount);

extern void AnimClone_Remove(lpAnimClone dead);
