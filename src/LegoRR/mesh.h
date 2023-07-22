#pragma once

#include "platform.h"
#include "viewport.h"
#include "lwt.h"

#define MESH_MAXTEXTURESEQENCE			100

//MESH RENDER FLAGS FOR RENDER DESC
//#define MESH_FLAG_ZB_ENABLE				0x00000100
//#define MESH_FLAG_ZB_WRITE				0x00000200
#define MESH_FLAG_RENDER_ALPHA11		0x00000400
#define MESH_FLAG_RENDER_ALPHASA1		0x00000800
#define MESH_FLAG_RENDER_ALPHATRANS		0x00001000
#define MESH_FLAG_RENDER_ALPHASA0		0x20000000
#define MESH_FLAG_RENDER_ALLALPHA		(MESH_FLAG_RENDER_ALPHASA0|MESH_FLAG_RENDER_ALPHA11|MESH_FLAG_RENDER_ALPHASA1|MESH_FLAG_RENDER_ALPHATRANS)
#define MESH_FLAG_TRANSFORM_PARENTPOS	0x00002000
#define MESH_FLAG_TRANSFORM_IDENTITY	0x00004000
#define MESH_FLAG_STIPPLE				0x00008000
#define MESH_FLAG_RENDER_ALPHAMOD		0x00010000
#define MESH_FLAG_RENDER_ALPHATEX		0x00020000
#define MESH_FLAG_RENDER_ALPHADIFFUSE	0x00040000
#define MESH_FLAG_ALPHAENABLE			0x00080000
#define MESH_FLAG_TEXTURECOLOURONLY		0x00100000
#define MESH_FLAG_HASBEENCLONED			0x00200000
#define MESH_FLAG_TRANSTEXTURE			0x00400000
#define MESH_FLAG_RENDER_FILTERNEAREST	0x00800000
#define MESH_FLAG_FACECAMERA			0x01000000
#define MESH_FLAG_FACECAMERADONE		0x02000000
#define MESH_FLAG_RENDER_DOUBLESIDED	0x04000000
#define MESH_FLAG_HIGHZBIAS				0x08000000
#define MESH_FLAG_ALPHAHIDDEN			0x10000000

#define MESH_RENDERFLAGS_LWONORM		( MESH_FLAG_TRANSFORM_PARENTPOS )
#define MESH_RENDERFLAGS_LWOALPHA		( MESH_FLAG_RENDER_ALPHATRANS | MESH_FLAG_TRANSFORM_PARENTPOS )

#define MESH_MAXLISTS			20

#define MESH_TEXTURELISTSIZE		2048

typedef enum Mesh_Colour
{
    Mesh_Colour_Diffuse,
    Mesh_Colour_Ambient,
    Mesh_Colour_Specular,
    Mesh_Colour_Emissive,

    Mesh_Colour_Alpha,
    Mesh_Colour_Power
} Mesh_Colour;


typedef enum Mesh_Type
{
    Mesh_Type_Norm,
    Mesh_Type_PostEffect,
    Mesh_Type_LightWaveObject,
} Mesh_Type;


typedef enum Mesh_WrapType
{
    Mesh_WrapType_XAxis,
    Mesh_WrapType_YAxis,
    Mesh_WrapType_ZAxis
} Mesh_WrapType;

#define MESH_DEFLISTSIZE			10
#define MESH_LISTINCREASE			150			// Increase list size by 150% when it is full...


#define MESH_FLAG_HIDDEN			0x00000001
#define MESH_FLAG_POSTEFFECT		0x00000002
#define MESH_FLAG_LWO				0x00000004


#define MESH_MAXTEXTURESTAGESTATES	50


//#define MESH_DEFAULTRENDERFLAGS	( D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_RESERVED1 | D3DFVF_TEX1 )
#define MESH_DEFAULTRENDERFLAGS		D3DFVF_VERTEX

typedef struct Container_Texture *lpMesh_Texture;

typedef struct Mesh_LightWave_SurfaceTAG
{
    lpMesh_Texture *textureSeq;
    lpMesh_Texture texture;
    U32 numInTexSeq;
    Point4F colour;
    F32 diffuse;
    F32 emissive;
    F32 specular;
    F32 power;
    F32 transparency;
    U32 flags;
    U32 texFlags;

    S32 texSeqOffset;

} Mesh_LightWave_Surface, *lpMesh_LightWave_Surface;

typedef struct Mesh_TextureStateChangeDataTAG
{
    U32 origValue;
    B32 changed;
} Mesh_TextureStateChangeData, *lpMesh_TextureStateChangeData;

typedef struct Mesh_RenderDescTAG
{
    void (*renderCallback)(struct Mesh *mesh, void* data, lpViewport vp);
    void* renderCallbackData;

    U32 renderFlags;
} Mesh_RenderDesc, *lpMesh_RenderDesc;

typedef struct Mesh_VertexTAG
{
    Point3F position;
    Point3F normal;

    F32 tu, tv;
} Mesh_Vertex, *lpMesh_Vertex;

typedef struct Mesh_GroupTAG
{
    U32 faceDataSize, vertexCount;
    U16* faceData;
    lpMesh_Vertex vertices;

    D3DMATERIAL material;
    LPDIRECT3DTEXTURE2 imText;

    U32 renderFlags;

    lpMesh_LightWave_Surface lightWaveSurfaceInfo;

    U32 flags;
} Mesh_Group, *lpMesh_Group;

typedef struct Mesh
{
    U32 groupCount, listSize;
    lpMesh_Group groupList;

    LPDIRECT3DRMUSERVISUAL uv;

    Mesh_RenderDesc renderDesc;
    lpMesh_LightWave_Surface lightWaveSurf;

    U32 numOfRefs;
    struct Mesh *clonedFrom;
    LPDIRECT3DRMFRAME3 frameCreatedOn;

    U32 flags;

    struct Mesh *nextFree;

    S32 (*textureRenderCallback)(void* data);
    void* textureRenderCallbackData;

} Mesh, *lpMesh;

typedef struct Mesh_PostRenderInfoTAG
{
    lpMesh mesh;
    D3DMATRIX matWorld;

    struct Mesh_PostRenderInfoTAG *next;
} Mesh_PostRenderInfo, *lpMesh_PostRenderInfo;

typedef struct Mesh_TextureReference
{
    LPDIRECTDRAWSURFACE4 surface;
    char* path;

    B32 trans;
} Mesh_TextureReference, *lpMesh_TextureReference;

typedef struct Mesh_GlobsTAG
{
    lpMesh postRenderList;

    lpMesh_PostRenderInfo postRenderMeshList;

    Mesh_TextureStateChangeData stateData[MESH_MAXTEXTURESTAGESTATES];

    char* sharedTextureDir;
    struct Mesh_TextureReference textureList[MESH_TEXTURELISTSIZE];
    struct Mesh_TextureReference textureListShared[MESH_TEXTURELISTSIZE];
    U32 textureCount, textureCountShared;

    D3DTEXTUREHANDLE oldTextureRM;
    D3DMATERIALHANDLE oldMatIM;
    LPDIRECT3DTEXTURE2 oldTextureIM;

    D3DTEXTUREHANDLE currTextureRM;
    D3DMATERIALHANDLE currMatIM;
    LPDIRECT3DTEXTURE2 currTextureIM;

    D3DMATERIALHANDLE matHandle;
    LPDIRECT3DMATERIAL3 imMat;

    U32 ambientLight;

    struct Mesh* listSet[MESH_MAXLISTS];
    struct Mesh* freeList;

    U32 listCount;
} Mesh_Globs, *lpMesh_Globs;

extern Mesh_Globs meshGlobs;

extern B32 Mesh_Initialize(const char* sharedTextureDir);

extern void Mesh_ClearPostRenderList();
extern B32 Mesh_CreateGlobalMaterial();

extern void Mesh_PostRenderAll(lpViewport vp);

extern lpMesh Mesh_Load(const char* fname, LPDIRECT3DRMFRAME3 frame, B32 noTextures);
extern B32 Mesh_ParseLWO(const char* basePath, lpMesh mesh, APPOBJ *lightWaveObject, B32 noTextures);
extern lpMesh Mesh_CreateOnFrame(LPDIRECT3DRMFRAME3 frame, void(*renderFunc)(lpMesh mesh, void* data, lpViewport vp), U32 renderFlags, void* data, U32 type);

extern lpMesh Mesh_Clone(lpMesh mesh, LPDIRECT3DRMFRAME3 frame);
