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

    B32 (*FindSFXIDFunc)(const char* name, SFX_ID* sfxID);
    B32 (*SoundEnabledFunc)();
    S32 (*PlaySample3DFunc)(lpContainer frame, SFX_ID type, B32 loop, B32 onCont, lpPoint3F wPos);

#ifdef _DEBUG
    B32 initialized;
#endif

} Lws_Globs, *lpLws_Globs;

extern Lws_Globs lwsGlobs;

extern void Lws_Initialize(const char* sharedDir, B32 (*FindSFXID)(const char* name, SFX_ID* sfxID), S32 (*PlaySample3D)(lpContainer cont, SFX_ID type, B32 loop, B32 onCont, Point3F* wPos), B32 (*SoundEnabled)());

extern lpLws_Info Lws_Parse(const char* fname, B32 looping);
extern void Lws_LoadMeshes(lpLws_Info scene, LPDIRECT3DRMFRAME3 parent);
extern void Lws_CreateFrames(lpLws_Info scene, lpLws_Node node, LPDIRECT3DRMFRAME3 parent, U16* frameCount);
extern void Lws_LoadNodes(lpLws_Info scene, lpLws_Node node);
extern lpMesh Lws_LoadMesh(const char* baseDir, const char* fname, LPDIRECT3DRMFRAME3 frame, B32 noTextures);

extern lpMesh Lws_SearchMeshPathList(lpLws_MeshPath list, U32 count, const char* path);
extern void Lws_AddMeshPathEntry(lpLws_MeshPath list, U32* count, const char* path, lpMesh mesh);

extern void Lws_SetupNodeTransform(lpLws_Info scene, lpLws_Node node, lpPoint3F pos, lpPoint3F hpb, lpPoint3F scale);
extern void Lws_SetAbsoluteKey(lpLws_Info scene, lpLws_Node node, U16 key);
extern void Lws_SetDissolveLevel(lpLws_Info scene, lpLws_Node node, F32 level);

extern void Lws_SetTime(lpLws_Info scene, F32 time);
extern U32 Lws_GetFrameCount(lpLws_Info scene);

extern void Lws_AnimateTextures(lpLws_Info scene, lpLws_Node node);

extern void Lws_SetupSoundTriggers(lpLws_Info scene);

extern F32 Lws_FindPrevKey(lpLws_Node node, F32 time, U16* prev);
extern F32 Lws_FindPrevDissolve(lpLws_Node node, F32 time, U16* prev);

extern void Lws_HandleTrigger(lpLws_Info scene, lpLws_Node node);
extern B32 Lws_KeyPassed(lpLws_Info scene, U32 key);

extern void Lws_InterpolateKeys(lpLws_Info scene, lpLws_Node node, U16 key, F32 delta);
extern void Lws_InterpolateDissolve(lpLws_Info scene, lpLws_Node node, U16 prev, F32 delta);

extern inline lpMesh Lws_GetNodeMesh(lpLws_Info scene, lpLws_Node node);

