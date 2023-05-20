#pragma once

#include "types.h"
#include "anim_clone.h"
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

typedef struct Container_TextureRef
{
    const char* fileName;
    LPDIRECT3DRMTEXTURE3 texture;
} Container_TextureRef, *lpContainer_TextureRef;

typedef struct Container
{
    LPDIRECT3DRMFRAME3 masterFrame, activityFrame, hiddenFrame;
    lpContainer_Type typeData;
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

    void (*soundTriggerCallback)(const char* sampleName, lpContainer cont, void* data);
    void* soundTriggerData;

    void (*triggerFrameCallback)(lpContainer cont, void* data);
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

extern void Container_Frame_SetAppData(LPDIRECT3DRMFRAME3 frame, lpContainer owner, lpAnimClone animClone, const char* asfname, U32* frameCount, const char* frameName, F32* currTime, F32* transCo, const char* actSample, void* soundRecord, U32* trigger);
