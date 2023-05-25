#include "bmp.h"
#include "mem.h"

void BMP_Parse(void* data, U32 size, LPD3DRMIMAGE istruct)
{
    S32 i;
    U8 swap;
    LPD3DRMPALETTEENTRY pal;
    lpBMP_Header header = data;
    U32 colourCount = 0, bpp;

    if (header->bits_per_pixel == 8)
    {
        if (!(colourCount = header->colours))
            colourCount = 256;

        pal = Mem_Alloc(sizeof(D3DRMPALETTEENTRY) * 256); // Always create a table 256 long.
        memset(pal, 0, sizeof(D3DRMPALETTEENTRY) * 256);
        memcpy(pal, ((U8*)data) + sizeof(BMP_Header), colourCount * sizeof(D3DRMPALETTEENTRY));

        istruct->rgb = FALSE;
        istruct->red_mask = 0xfc;
        istruct->green_mask = 0xfc;
        istruct->blue_mask = 0xfc;
        istruct->alpha_mask = 0xfc;
        istruct->palette_size = colourCount;

        for (i = 0; i < (S32)colourCount; i++)
        {
            swap = pal[i].red; // convert palette to RGB from BGR
            pal[i].red = pal[i].blue;
            pal[i].blue = swap;
        }
        istruct->palette = pal; // place in image structure
    } else {
        istruct->rgb = TRUE;
        istruct->palette_size = 0;
        istruct->palette = NULL;
    }

    istruct->width = (S32)header->wid;
    istruct->height = header->hgt;
    istruct->depth = header->bits_per_pixel;

    switch (istruct->depth)
    {
        case 8:
            bpp = 1;
            break;
        case 15:
        case 16:
            bpp = 2;
            break;
        case 24:
            bpp = 3;
            break;
        case 32:
            bpp = 4;
            break;
        default:
            bpp = 0;
    }
    istruct->bytes_per_line = ((istruct->width * bpp) + 3) & ~3;

    istruct->buffer1 = &((U8*)data)[header->img_offset];
    istruct->buffer2 = NULL;

    istruct->aspectx = istruct->aspecty = 1;
}

void BMP_Cleanup(LPD3DRMIMAGE istruct)
{
    if (istruct->palette)
        Mem_Free(istruct->palette);
}
