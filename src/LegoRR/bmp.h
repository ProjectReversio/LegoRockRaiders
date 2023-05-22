#pragma once

#include "types.h"
#include <d3drm.h>

#pragma pack(push)
#pragma pack(1)

typedef struct BMP_Header
{
    char bmp_str[2];
    U32 file_size;
    U16 res1;
    U16 res2;
    U32 img_offset;

    U32 img_header_size;
    U32 wid;
    U32 hgt;
    U16 planes; // Always 1
    U16 bits_per_pixel;
    U32 compression;
    U32 comp_img_size;
    U32 horz_res; // pels/m
    U32 vert_res; // pels/m
    U32 colours;
    U32 important_colours;

} BMP_Header, *lpBMP_Header;

#pragma pack(pop)

extern void BMP_Parse(void* data, U32 size, LPD3DRMIMAGE istruct);
extern void BMP_Cleanup(LPD3DRMIMAGE istruct);
