#pragma once

#include "platform.h"
#include "3DSound.h"
#include <d3drm.h>

#define CONTAINER_MAXLISTS 20
#define CONTAINER_MAXTEXTURES 1000
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

extern void Container_AddList();

extern void Container_SetTypeData(lpContainer cont, const char* name, LPDIRECT3DRMLIGHT light, LPDIRECT3DRMMESH mesh, struct Mesh* transMesh);

extern void Container_Frame_SetAppData(LPDIRECT3DRMFRAME3 frame, lpContainer owner, struct AnimClone* animClone, const char* asfname, U32* frameCount, const char* frameName, F32* currTime, F32* transCo, const char* actSample, void* soundRecord, U32* trigger);

extern void Container_SetSharedTextureDirectory(const char* path);

extern void Container_SetTriggerFrameCallback(void (*Callback)(lpContainer cont, void* data), void* data);

extern void Container_GetFrames(lpContainer cont, lpContainer ref, LPDIRECT3DRMFRAME3 *contFrame, LPDIRECT3DRMFRAME3 *refFrame);

extern void Container_SetPosition(lpContainer cont, lpContainer ref, F32 x, F32 y, F32 z);
extern void Container_SetOrientation(lpContainer cont, lpContainer ref, F32 dirx, F32 diry, F32 dirz, F32 upx, F32 upy, F32 upz);
extern void Container_SetParent(lpContainer child, lpContainer parent);

extern void Container_Frame_SafeAddChild(LPDIRECT3DRMFRAME3 parent, LPDIRECT3DRMFRAME3 child);

extern void Container_GetPosition(lpContainer cont, lpContainer ref, lpPoint3F pos);
extern void Container_GetOrientation(lpContainer cont, lpContainer ref, lpPoint3F dir, lpPoint3F up);

extern F32 Container_GetAnimationTime(lpContainer cont);
extern F32 Container_SetAnimationTime(lpContainer cont, F32 time);

extern U32 Container_GetAnimationFrames(lpContainer cont);

extern LPDIRECT3DRMFRAME3 Container_Frame_Find(lpContainer cont, const char* findName, U32 hidden);

extern F32 Container_Frame_GetCurrTime(LPDIRECT3DRMFRAME3 frame);
extern U32 Container_Frame_GetFrameCount(LPDIRECT3DRMFRAME3 frame);
extern struct AnimClone* Container_Frame_GetAnimClone(LPDIRECT3DRMFRAME3 frame);
extern U32 Container_Frame_GetTrigger(LPDIRECT3DRMFRAME3 frame);

extern Container_Type Container_ParseTypeString(const char* str, B32* noTexture);

extern lpContainer Container_Load(lpContainer parent, const char* filename, const char* typestr, B32 looping);
extern lpContainer Container_MakeLight(lpContainer parent, U32 type, F32 r, F32 g, F32 b);

extern void Container_Hide2(lpContainer cont, B32 hide);
extern void Container_Hide(lpContainer cont, B32 hide);

extern void Container_SetSoundTriggerCallback(ContainerSoundTriggerCallback callback, void* data);

extern inline LPDIRECT3DRMFRAME3 Container_GetMasterFrame(lpContainer cont);

extern void Container_Frame_FormatName(LPDIRECT3DRMFRAME3 frame, const char* msg, ...);

extern B32 Container_GetDecalColour(const char* fname, U32* colour);

extern struct AnimClone* Container_LoadAnimSet(const char* fname, LPDIRECT3DRMFRAME3 frame, U32* frameCount, B32 lws, B32 looping);

extern LPDIRECTDRAWSURFACE4 Container_LoadTextureSurface(const char* fname, B32 managed, U32* width, U32* height, B32* trans);

extern lpContainer Container_GetRoot();
