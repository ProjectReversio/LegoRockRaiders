#pragma once

#include "platform.h"
#include "file.h"

#define LBUF_LEN 256

#define LWVALUE(n) (((n>>24)&0xff) + ((n>>8)&0xff00) + ((n<<8)&0xff0000) + ((n<<24)&0xff000000))
#define LWSVALUE(n) (((n>>8)&0xff) + ((n<<8)&0xff00))
#define LWSPVALUE(n) ((((*n)>>8)&0xff) + (((*n)<<8)&0xff00))

typedef struct
{
    U32 srflCount;
    char **srflName;
} LWSURFLIST;

typedef struct
{
    U32 lwVertCount;
    U32 lwPolyCount;
    U32 lwSurfaceCount;
} LWSIZE;

typedef struct
{
    U32 plyCount;
    U32 plySurface;
    U16 *plyData;
} LWPOLY;

typedef struct
{
    U8 colRed;
    U8 colGreen;
    U8 colBlue;
    U8 colAlpha;
} LWRGB;

typedef struct
{
    F32 tdX;
    F32 tdY;
    F32 tdZ;
} TEXDATA;

typedef struct _LWSURFACE
{
    char* srfName;
    struct _LWSURFACE *srfNextSurf;
    char* srfPath;
    LWRGB srfCol;
    char srfTCLR[4];
    U32 srfTexFlags;
    U32 srfFlags;
    U32 srfTexType;
    U32 srfTexWrap;
    TEXDATA srfTexSize;
    TEXDATA srfTexCentre;
    F32 srfLuminous;
    F32 srfTransparent;
    F32 srfDiffuse;
    F32 srfReflect;
    F32 srfSpecular;
    F32 srfSpecPower;
} LWSURFACE;

typedef struct APPOBJTAG
{
    char* aoPath;
    LWSIZE aoSize;
    F32 *aoVerts;
    LWPOLY *aoPoly;
    LWSURFACE *aoSurface;

    lpFile aoFileUV;

} APPOBJ;

// srfTexFlags bit enumeration
enum
{
    TF_AXIS_X = 0,
    TF_AXIS_Y,
    TF_AXIS_Z,
    TF_WORLD_COORD,
    TF_NEGATIVE_IMAGE,
    TF_PIXEL_BLENDING,
    TF_ANTIALIASING,
    TF_SEQUENCE,
    TF_MAX,
};

#define	TFM_AXIS_X              (1<<TF_AXIS_X)
#define	TFM_AXIS_Y              (1<<TF_AXIS_Y)
#define	TFM_AXIS_Z              (1<<TF_AXIS_Z)
#define	TFM_WORLD_COORD         (1<<TF_WORLD_COORD)
#define	TFM_NEGATIVE_IMAGE      (1<<TF_NEGATIVE_IMAGE)
#define	TFM_PIXEL_BLENDING      (1<<TF_PIXEL_BLENDING)
#define	TFM_ANTIALIASING        (1<<TF_ANTIALIASING)
#define	TFM_SEQUENCE            (1<<TF_SEQUENCE)

// srfFlags bit enumeration
enum
{
    SF_LUMINOUS = 0,
    SF_OUTLINE,
    SF_SMOOTHING,
    SF_COLORHIGHLIGHTS,
    SF_COLORFILTER,
    SF_OPAQUEEDGE,			//\ These two should not be set together
    SF_TRANSPARENTEDGE,		///
    SF_SHARPTERMINATOR,
    SF_DOUBLESIDED,
    SF_ADDITIVE,
    SF_MAX,
};

#define SFM_LUMINOUS                (1<<SF_LUMINOUS)
#define SFM_OUTLINE                 (1<<SF_OUTLINE)
#define SFM_SMOOTHING               (1<<SF_SMOOTHING)
#define SFM_COLORHIGHLIGHTS         (1<<SF_COLORHIGHLIGHTS)
#define SFM_COLORFILTER             (1<<SF_COLORFILTER)
#define SFM_OPAQUEEDGE              (1<<SF_OPAQUEEDGE)
#define SFM_TRANSPARENTEDGE         (1<<SF_TRANSPARENTEDGE)
#define SFM_SHARPTERMINATOR         (1<<SF_SHARPTERMINATOR)
#define SFM_DOUBLESIDED             (1<<SF_DOUBLESIDED)
#define SFM_ADDITIVE                (1<<SF_ADDITIVE)

enum
{
    MT_PLANAR = 0,
    MT_CYLINDRICAL,
    MT_SPHERICAL,
    MT_MAX
};

B32 LoadLWOB(char* fn, LWSIZE *size, F32 **verts, LWPOLY **polys, LWSURFACE **surf, lpFile *fileUV, B32 dflag);
B32 FreeLWOB(APPOBJ *ao);

B32 LoadAppObj(char* fn, APPOBJ **ao, B32 flag);
