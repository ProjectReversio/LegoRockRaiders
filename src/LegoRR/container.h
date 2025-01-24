#pragma once

#include "platform.h"
#include "3DSound.h"
#include "main.h"
#include "common.h"
#include <d3drm.h>

#define CONTAINER_ULONG_NULL (-1)

#define CONTAINER_ACTIVITYFRAMEPREFIX "ActFrame"
#define CONTAINER_SCALESTRING "SCALE"

#define CONTAINER_TRASHVALUE 0xdeaddead

#define CONTAINER_TEXTURE_NOLOAD 0x00000001

#define CONTAINER_MAXLISTS 20
#define CONTAINER_MAXTEXTURES 1000
#define CONTAINER_MESHGROUPBLOCKSIZE 20
#define CONTAINER_MAXVISUALS 4

#ifdef _DEBUG
#define CONTAINER_DEBUG_NOTREQUIRED	1
#else
#define CONTAINER_DEBUG_NOTREQUIRED	NULL
#endif

// TODO: Implement these for debug mode
#define Container_DebugCheckOK(c)      (c)
#define Container_Mesh_DebugCheckOK(c, g)
#define Container_NoteCreation(o)

struct Container_CloneData;

typedef enum Container_Type
{
    Container_Invalid = -1,
    Container_Null,
    Container_Mesh,
    Container_Frame,
    Container_Anim,
    Container_FromActivity,
    Container_Light,
    Container_Reference,
    Container_LWS,
    Container_LWO,

    Container_TypeCount
} Container_Type, *lpContainer_Type;

typedef enum Container_MeshType
{
    Container_MeshType_Normal         = 0,
    Container_MeshType_SeperateMeshes = 1,
    Container_MeshType_Immediate      = 2,
    Container_MeshType_Transparent    = 3,
    Container_MeshType_Additive       = 4,
    Container_MeshType_Subtractive    = 5,
} Container_MeshType;

typedef enum ContainerFlags
{
    CONTAINER_FLAG_NONE = 0,
    CONTAINER_FLAG_TRIGGERSAMPLE = 0x2,
    CONTAINER_FLAG_MESHSWAPPED = 0x4,
    CONTAINER_FLAG_HIDDEN = 0x8,
    CONTAINER_FLAG_DEADREFERENCE = 0x10,
    CONTAINER_FLAG_ANIMATIONSKIPPED = 0x20,
    CONTAINER_FLAG_HIDDEN2 = 0x80,
} ContainerFlags;

typedef enum Container_GlobFlags
{
    CONTAINER_GLOB_FLAG_NONE = 0,
    CONTAINER_GLOB_FLAG_INITIALIZED = 1,
    CONTAINER_GLOB_FLAG_TRIGGERENABLED = 0x40,
} Container_GlobFlags;

typedef enum Container_Combine
{
    Container_Combine_Replace = 0,
    Container_Combine_Before  = 1,
    Container_Combine_After   = 2,
} Container_Combine;

typedef struct Container_TypeData
{
    char* name;
    LPDIRECT3DRMLIGHT light;
    LPDIRECT3DRMMESH mesh;
    struct Mesh *transMesh;
} Container_TypeData, *lpContainer_TypeData;

typedef struct Container_Texture
{
    LPDIRECTDRAWSURFACE4 surface;
    LPDIRECT3DRMTEXTURE3 texture;

    B32 colourKey;
} Container_Texture, *lpContainer_Texture;

typedef struct Container_TextureRef
{
    const char* fileName;
    LPDIRECT3DRMTEXTURE3 texture;
} Container_TextureRef, *lpContainer_TextureRef;

typedef struct Container_TextureData
{
    const char* xFileName;
    U32 flags;
} Container_TextureData, *lpContainer_TextureData;

typedef enum Container_SearchMode
{
    Container_SearchMode_FirstMatch,
    Container_SearchMode_MatchCount,
    Container_SearchMode_NthMatch
} Container_SearchMode;

typedef struct Container
{
    LPDIRECT3DRMFRAME3 masterFrame, activityFrame, hiddenFrame;
    lpContainer_TypeData typeData;
    Container_Type type;
    ContainerFlags flags;
    void (*activityCallback)(struct Container* cont, void* data);
    void* activityCallbackData;
    void* userData;

    struct Container_CloneData *cloneData; // Only used by animsets.
    struct Container* nextFree;
} Container, *lpContainer;

typedef struct Container_CloneData
{
    lpContainer clonedFrom; // NULL if this is the original
    lpContainer* cloneTable; // List of clones (NULL for unused elements)
    U32 cloneCount, cloneNumber; // Total number of clones made and clone's number in the list.
    B32 used;
} Container_CloneData, *lpContainer_CloneData;

typedef struct Container_AppData
{
    lpContainer ownerContainer;

    char* animSetFileName; // For the dodgy Animation Set clone stuff...
    char* frameName; // For freeing the allocation for SetName...

    U32 frameCount;
    F32 currTime;
    F32 transCo; // Standard translation during amimset loop.
    char* activitySample; // Sample to play when activity is called...
    struct AnimClone* animClone;
    U32 trigger;

    lpSound3D_SoundFrameRecord soundList;
} Container_AppData, *lpContainer_AppData;

typedef struct Container_MeshAppData
{
    LPDIRECT3DRMMESH *meshList;
    U32 usedCount, listSize;
    B32 groupZeroHidden, firstAddGroup;
} Container_MeshAppData, *lpContainer_MeshAppData;

typedef struct Container_SearchData
{
    const char* string;
    U32 stringLen;
    B32 caseSensitive;
    LPDIRECT3DRMFRAME3 resultFrame;
    U32 count, mode, matchNumber;
} Container_SearchData, *lpContainer_SearchData;

typedef B32 (*Container_SearchCallback)(LPDIRECT3DRMFRAME3 frame, void* data);
typedef void (*ContainerSoundTriggerCallback)(const char* sampleName, lpContainer cont, void* data);
typedef void (*ContainerTriggerFrameCallback)(lpContainer cont, void* data);

typedef struct Container_Globs
{
    lpContainer listSet[CONTAINER_MAXLISTS];
    lpContainer freeList, rootContainer;

    const char* typeName[Container_TypeCount];
    const char* extensionNames[Container_TypeCount];
    const char* gameName;

    LPDIRECT3DRMVISUAL visualArray[CONTAINER_MAXVISUALS];

    Container_TextureRef textureSet[CONTAINER_MAXTEXTURES];
    U32 textureCount;

    ContainerSoundTriggerCallback soundTriggerCallback;
    void* soundTriggerData;

    ContainerTriggerFrameCallback triggerFrameCallback;
    void *triggerFrameData;

    const char* sharedDir;
    U32 fogColor;
    U32 listCount;
    Container_GlobFlags flags;
} Container_Globs, *lpContainer_Globs;

extern Container_Globs containerGlobs;

extern lpContainer Container_Initialize(const char* gameName);
extern void Container_Shutdown();

extern lpContainer Container_Create(lpContainer parent);
extern void Container_Remove(lpContainer dead);
extern void Container_Remove2(lpContainer dead, B32 kill);

extern void Container_Frame_FreeName(LPDIRECT3DRMFRAME3 frame);

extern void Container_AddList();

extern lpContainer Container_GetParent(lpContainer child);

extern lpContainer Container_Clone(lpContainer orig);

extern U32 Container_GetActivities(lpContainer cont, LPDIRECT3DRMFRAME3* frameList, struct AnimClone** acList, char** nameList);

extern void Container_SetTypeData(lpContainer cont, const char* name, LPDIRECT3DRMLIGHT light, LPDIRECT3DRMMESH mesh, struct Mesh* transMesh);

extern void Container_FreeTypeData(lpContainer cont);

extern void Container_Frame_SetAppData(LPDIRECT3DRMFRAME3 frame, lpContainer owner, struct AnimClone* animClone, const char* asfname, U32* frameCount, const char* frameName, F32* currTime, F32* transCo, const char* actSample, void* soundRecord, U32* trigger);

extern void Container_SetSharedTextureDirectory(const char* path);

extern void Container_SetTriggerFrameCallback(void (*Callback)(lpContainer cont, void* data), void* data);

extern void Container_Frame_ReferenceDestroyCallback(LPDIRECT3DRMOBJECT lpD3DRMobj, void* lpArg);

extern lpContainer Container_Frame_GetContainer(LPDIRECT3DRMFRAME3 frame);

extern void Container_GetFrames(lpContainer cont, lpContainer ref, LPDIRECT3DRMFRAME3 *contFrame, LPDIRECT3DRMFRAME3 *refFrame);

extern void Container_SetPosition(lpContainer cont, lpContainer ref, F32 x, F32 y, F32 z);
extern void Container_SetOrientation(lpContainer cont, lpContainer ref, F32 dirx, F32 diry, F32 dirz, F32 upx, F32 upy, F32 upz);
extern void Container_AddRotation(lpContainer cont, Container_Combine combine, F32 x, F32 y, F32 z, F32 angle);
extern void Container_AddScale(lpContainer cont, Container_Combine combine, F32 x, F32 y, F32 z);
extern void Container_AddTranslation(lpContainer cont, Container_Combine combine, F32 x, F32 y, F32 z);
extern void Container_SetParent(lpContainer child, lpContainer parent);

extern B32 Container_Light_SetSpotPenumbra(lpContainer spotlight, F32 angle);
extern B32 Container_Light_SetSpotUmbra(lpContainer spotlight, F32 angle);
extern B32 Container_Light_SetSpotRange(lpContainer spotlight, F32 dist);

extern void Container_Frame_SafeAddChild(LPDIRECT3DRMFRAME3 parent, LPDIRECT3DRMFRAME3 child);

extern void Container_GetPosition(lpContainer cont, lpContainer ref, lpPoint3F pos);
extern void Container_GetOrientation(lpContainer cont, lpContainer ref, lpPoint3F dir, lpPoint3F up);

extern B32 Container_AddActivity2(lpContainer cont, const char* fname, const char* actname, F32 transCo, U32 trigger, const char* sample, struct AnimClone* origClone, B32 lws, B32 looping);
inline B32 Container_AddActivity(lpContainer cont, const char* fname, const char* actname, F32 transCo, U32 trigger, const char* sample, struct AnimClone* animClone, B32 lws, B32 looping)
{
    return Container_AddActivity2(cont, fname, actname, transCo, trigger, sample, animClone, lws, looping);
}
extern B32 Container_SetActivity(lpContainer cont, const char* actname);

extern F32 Container_GetAnimationTime(lpContainer cont);
extern F32 Container_SetAnimationTime(lpContainer cont, F32 time);

extern U32 Container_GetAnimFileFrameCount(const char* fileData);
extern U32 Container_GetAnimationFrames(lpContainer cont);

extern LPDIRECT3DRMFRAME3 Container_Frame_Find(lpContainer cont, const char* findName, U32 hidden);

extern void Container_Frame_RemoveAppData(LPDIRECT3DRMFRAME3 frame);

extern const char* Container_Frame_GetName(LPDIRECT3DRMFRAME3 frame);
extern F32 Container_Frame_GetCurrTime(LPDIRECT3DRMFRAME3 frame);
extern U32 Container_Frame_GetFrameCount(LPDIRECT3DRMFRAME3 frame);
extern lpContainer Container_Frame_GetOwner(LPDIRECT3DRMFRAME3 frame);
extern const char* Container_Frame_GetAnimSetFileName(LPDIRECT3DRMFRAME3 frame);
extern struct AnimClone* Container_Frame_GetAnimClone(LPDIRECT3DRMFRAME3 frame);
extern F32 Container_Frame_GetTransCo(LPDIRECT3DRMFRAME3 frame);
extern const char* Container_Frame_GetSample(LPDIRECT3DRMFRAME3 frame);
extern U32 Container_Frame_GetTrigger(LPDIRECT3DRMFRAME3 frame);

extern U32 Container_Mesh_GetVertices(lpContainer cont, U32 groupID, U32 index, U32 count, lpVertex retArray);
extern U32 Container_Mesh_SetVertices(lpContainer cont, U32 groupID, U32 index, U32 count, lpVertex values);

extern lpContainer Container_SearchTree(lpContainer root, const char* name, Container_SearchMode mode, U32* count);
extern B32 Container_Frame_WalkTree(LPDIRECT3DRMFRAME3 frame, U32 level, Container_SearchCallback callback, void* data);
extern B32 Container_Frame_SearchCallback(LPDIRECT3DRMFRAME3 frame, void* data);

extern Container_Type Container_ParseTypeString(const char* str, B32* noTexture);

extern const char* Container_FormatPartName(lpContainer cont, const char* partName, U32* instance);

extern lpContainer Container_Load(lpContainer parent, const char* filename, const char* typestr, B32 looping);
extern lpContainer Container_MakeLight(lpContainer parent, U32 type, F32 r, F32 g, F32 b);
extern lpContainer Container_MakeMesh2(lpContainer parent, Container_MeshType type);

inline U32 Container_GetRGBAColour(F32 r, F32 g, F32 b, F32 a)
{
    if (r > 1.0f)
        r = 1.0f;
    if (g > 1.0f)
        g = 1.0f;
    if (b > 1.0f)
        b = 1.0f;
    if (a > 1.0f)
        a = 1.0f;

    if (r < 0.0f)
        r = 0.0f;
    if (g < 0.0f)
        g = 0.0f;
    if (b < 0.0f)
        b = 0.0f;
    if (a < 0.0f)
        a = 0.0f;

    return D3DRGBA(r, g, b, a);
}

inline U32 Container_GetRGBColour(F32 r, F32 g, F32 b)
{
    return Container_GetRGBAColour(r, g, b, 1.0f);
}

extern void Container_SetColourAlpha(lpContainer cont, F32 r, F32 g, F32 b, F32 a);

#define Container_SetColour(c, r, g, b)             Container_SetColourAlpha((c), (r), (g), (b), 1.0f)

#define Container_MakeMesh(r)                       Container_MakeMesh2((r), Container_MeshType_Normal)
#define Container_MakeMeshSeparateGroups(r)         Container_MakeMesh2((r), Container_MeshType_SeperateMeshes)
#define Container_MakeMeshImmediate(r)              Container_MakeMesh2((r), Container_MeshType_Immediate)
#define Container_MakeMeshTrans(r)                  Container_MakeMesh2((r), Container_MeshType_Transparent)
#define Container_MakeMeshAdditive(r)               Container_MakeMesh2((r), Container_MeshType_Additive)
#define Container_MakeMeshSubtractive(r)            Container_MakeMesh2((r), Container_MeshType_Subtractive)

#define Container_LoadTexture(f, w, h)              Container_LoadTexture2((f), FALSE, (w), (h))
#define Container_LoadTextureImmediate(f, w, h)     Container_LoadTexture2((f), TRUE, (w), (h))

extern lpContainer_Texture Container_LoadTexture2(const char* fname, B32 immediate, U32* width, U32* height);

extern void Container_Mesh_SetTexture(lpContainer cont, U32 groupID, lpContainer_Texture itext);

extern U32 Container_Mesh_AddGroup(lpContainer cont, U32 vertexCount, U32 faceCount, U32 vPerFace, U32* faceData);
extern void Container_Mesh_HideGroup(lpContainer cont, U32 group, B32 hide);
extern void Container_Mesh_SetQuality(lpContainer cont, U32 group, Graphics_Quality quality);
extern void Container_Mesh_SetEmissive(lpContainer cont, U32 groupID, F32 r, F32 g, F32 b);

extern void Container_Mesh_SetColourAlpha(lpContainer cont, U32 groupID, F32 r, F32 g, F32 b, F32 a);

extern void Container_Mesh_SetPerspectiveCorrection(lpContainer cont, U32 group, B32 on);

extern void Container_Mesh_Swap(lpContainer target, lpContainer origin, B32 restore);

extern void Container_EnableFog(B32 on);
extern void Container_SetFogColour(F32 r, F32 g, F32 b);
extern void Container_GetFogColour(F32* r, F32* g, F32* b);
extern void Container_SetFogMode(U32 mode);
extern void Container_SetFogParams(F32 start, F32 end, F32 density);

extern B32 Container_Mesh_HandleSeparateMeshGroups(LPDIRECT3DRMMESH *mesh, U32* group);

extern void Container_Hide2(lpContainer cont, B32 hide);
extern void Container_Hide(lpContainer cont, B32 hide);

extern void Container_SetSoundTriggerCallback(ContainerSoundTriggerCallback callback, void* data);

extern void Container_YFlipTexture(LPDIRECT3DRMTEXTURE3 texture);

extern S32 __cdecl Container_TextureSetSort(const void* a, const void* b);
extern void Container_TextureDestroyCallback(LPDIRECT3DRMOBJECT lpD3DRMobj, void* lpArg);
extern HRESULT Container_TextureLoadCallback(const char* name, void* data, LPDIRECT3DRMTEXTURE3 *texture);

extern B32 Container_FrameLoad(const char* fname, LPDIRECT3DRMFRAME3 frame);
extern LPDIRECT3DRMMESH Container_MeshLoad(void* file_data, U32 file_size, const char* file_name, LPDIRECT3DRMFRAME3 frame, B32 noTexture);

extern inline LPDIRECT3DRMFRAME3 Container_GetMasterFrame(lpContainer cont);

extern void Container_Frame_FormatName(LPDIRECT3DRMFRAME3 frame, const char* msg, ...);

extern B32 Container_GetDecalColour(const char* fname, U32* colour);

extern struct AnimClone* Container_LoadAnimSet(const char* fname, LPDIRECT3DRMFRAME3 frame, U32* frameCount, B32 lws, B32 looping);

extern LPDIRECTDRAWSURFACE4 Container_LoadTextureSurface(const char* fname, B32 managed, U32* width, U32* height, B32* trans);

extern lpContainer Container_GetRoot();

extern void Container_SetUserData(lpContainer cont, void* data);

extern void Container_EnableSoundTriggers(B32 on);
