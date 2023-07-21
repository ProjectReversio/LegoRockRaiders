#pragma once

#include "platform.h"
#include "maths.h"
#include "sfx.h"
#include "mesh.h"

#define LWS_FILESUFFIX              "lws"
#define LWS_HEADER1                 "LWSC"
#define LWS_HEADER2                 "1"
#define LWS_SOUNDTRIGGERPREFIX      "SFX"
#define LWS_SOUNDTRIGGERSEPERATOR   ","

#define LWS_MAXLINELEN              1024
#define LWS_MAXARGS                 20
#define LWS_NODELISTBLOCKSIZE       10
#define LWS_MAXMESHFILES            1000
#define LWS_MAXSTATICDISSOLVES      100
#define LWS_MAXTRIGGERKEYS          25

#define LWS_FLAG_LOOPING            0x01

#define LWSNODE_FLAG_NULL           0x01
#define LWSNODE_FLAG_SOUNDTRIGGER   0x02
#define LWSNODE_FLAG_FACECAMERA     0x04

typedef struct Lws_SoundTrigger
{
    U32 sfxID;
    U16 frameStartList[LWS_MAXTRIGGERKEYS];
    U16 frameEndList[LWS_MAXTRIGGERKEYS];
    U32 loopUID[LWS_MAXTRIGGERKEYS];
    U16 count;
} Lws_SoundTrigger, *lpLws_SoundTrigger;

typedef struct Lws_KeyInfo
{
    Point3F position;
    Point3F hpb;
    Point3F scale;
    U16 frame;
} Lws_KeyInfo, *lpLws_KeyInfo;

typedef struct Lws_Node
{
    char* name;
    U16 reference, frameIndex;
    U8 triggerIndex;

    Point3F pivotVector;
    Lws_KeyInfo *keyList;

    F32* dissolveLevel;
    U16* dissolveFrame;

    U16 keyCount, dissolveCount;

    U8 flags;

    struct Lws_Node *childList;
    struct Lws_Node *next;
} Lws_Node, *lpLws_Node;

typedef struct Lws_Info
{
    U16 firstFrame, lastFrame;
    F32 fps;
    char* filePath;

    F32 lastTime, time;

    LPDIRECT3DRMFRAME3 *frameList;
    lpLws_SoundTrigger triggerList;

    lpLws_Node masterNode;
    lpLws_Node nodeList;

    U16 nodeCount, nodeListSize, triggerCount;

    struct Lws_Info* clonedFrom;
    U32 referenceCount;

    U8 flags;
} Lws_Info, *lpLws_Info;

typedef struct Lws_MeshPath
{
    char* path;
    lpMesh mesh;
} Lws_MeshPath, *lpLws_MeshPath;

typedef struct Lws_Globs
{
    Lws_MeshPath meshPathList[LWS_MAXMESHFILES]; // For full path to files...
    Lws_MeshPath meshPathListShared[LWS_MAXMESHFILES]; // For shared files...
    char* sharedDir;
    U32 meshPathCount, meshPathCountShared;
    F32 staticDissolveLevel[LWS_MAXSTATICDISSOLVES];
    U32 staticDissolveCount;

    B32 (*FindSFXIDFunc)(const char* name, U32* sfxID);
    B32 (*SoundEnabledFunc)();
    S32 (*PlaySample3DFunc)(void* frame, U32 type, B32 loop, B32 onCont, lpPoint3F wPos);

#ifdef _DEBUG
    B32 initialized;
#endif

} Lws_Globs, *lpLws_Globs;

extern Lws_Globs lwsGlobs;

extern void Lws_Initialize(const char* sharedDir, B32 (*FindSFXID)(const char* name, SFX_ID* sfxID), S32 (*PlaySample3D)(lpContainer cont, SFX_ID type, B32 loop, B32 onCont, Point3F* wPos), B32 (*SoundEnabled)());

extern lpLws_Info Lws_Parse(const char* fname, B32 looping);
extern void Lws_LoadMeshes(lpLws_Info scene, LPDIRECT3DRMFRAME3 parent);
extern void Lws_SetTime(lpLws_Info scene, F32 time);
extern U32 Lws_GetFrameCount(lpLws_Info scene);

