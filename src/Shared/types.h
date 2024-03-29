#pragma once

#include <stdint.h>

typedef uint8_t     U8;
typedef int8_t      S8;

typedef uint16_t    U16;
typedef int16_t     S16;

typedef uint32_t    U32;
typedef int32_t     S32;

typedef uint64_t    U64;
typedef int64_t     S64;

typedef float       F32;
typedef double      F64;

typedef U8          B8;
typedef U32         B32;

#define TRUE 1
#define FALSE 0

typedef enum Bool3
{
    BOOL3_FALSE = 0,
    BOOL3_TRUE = 1,
    BOOL3_ERROR = 2,
} Bool3;

typedef struct ColourRGBF
{
    F32 red;
    F32 green;
    F32 blue;
} ColourRGBF;
