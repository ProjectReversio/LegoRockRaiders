#include <stdio.h>
#include "flic.h"
#include "file.h"
#include "mem.h"
#include "directdraw.h"
#include "error.h"

B32 Flic_Setup(const char* filename, lpFlic* fsp, FlicUserFlags flags)
{
    S32 count, xsize, ysize;
    char drive[_MAX_DRIVE];
    char dir[_MAX_DIR];
    char fname[_MAX_FNAME];
    char ext[_MAX_EXT];
    char buff[128];
    DDSURFACEDESC2 ddsd;
    HRESULT hr = 0;
    LPDIRECTDRAW4 dDraw = DirectDraw();
    LPDIRECTDRAW4 *lpDD = &dDraw;
    DDCOLORKEY ColourKey;

    // Allocate flic structure memory
    *fsp = (lpFlic)Mem_Alloc(sizeof(Flic));
    ZeroMemory(*fsp, sizeof(Flic));

    (*fsp)->fsDisplayMode = FLICMODE_HICOLOR;

    _splitpath(filename, drive, dir, fname, ext);
    if (!Flic_LoadHeader(filename, fsp))
        return FALSE;

    // create a DirectDrawSurface for this bitmap
    ZeroMemory(&ddsd, sizeof(ddsd));
    ddsd.dwSize = sizeof(ddsd);
    ddsd.dwFlags = DDSD_CAPS | DDSD_HEIGHT | DDSD_WIDTH;
    ddsd.ddsCaps.dwCaps = DDSCAPS_OFFSCREENPLAIN | DDSCAPS_SYSTEMMEMORY;
    xsize = (*fsp)->fsHeader.width;
    ysize = (*fsp)->fsHeader.height;
    ddsd.dwWidth = xsize;
    ddsd.dwHeight = ysize;
    (*fsp)->fsXsize = xsize;
    (*fsp)->fsYsize = ysize;

    if ((hr = (*lpDD)->lpVtbl->CreateSurface(*lpDD, &ddsd, &(*fsp)->fsSurface, NULL)))
    {
        Error_Fatal(TRUE, "Failed to create surface for flic");
    }

    ColourKey.dwColorSpaceLowValue = 0;
    ColourKey.dwColorSpaceHighValue = 0;
    (*fsp)->fsSurface->lpVtbl->SetColorKey((*fsp)->fsSurface, DDCKEY_SRCBLT, &ColourKey);

    count = 0;
    (*fsp)->userFlags = flags;
    strcpy((*fsp)->filename, filename);
    (*fsp)->fsXc = 0;
    (*fsp)->fsYc = 0;
    (*fsp)->currentframe = 0;
    (*fsp)->overallframe = 0;
    (*fsp)->framerate = 25 << 16;

    if ((((*fsp)->userFlags) & FLICMEMORY) == FLICMEMORY)
    {
        (*fsp)->pointerposition = 0;
        (*fsp)->rambufferhandle = Mem_Alloc((*fsp)->fsHeader.size);
        if (!(*fsp)->rambufferhandle)
            Error_Warn(TRUE, "No flic buffer handle");
        if (((*fsp)->filehandle = File_Open(filename, "rb")) == NULL)
            Error_Fatal(TRUE, "Could not open the flic");

        File_Seek((*fsp)->filehandle, 128, SEEK_SET);
        File_Read((*fsp)->rambufferhandle, (*fsp)->fsHeader.size, 1, (*fsp)->filehandle);
        File_Close((*fsp)->filehandle);
    } else
    {
        (*fsp)->pointerposition = 128;
        if (((*fsp)->filehandle = File_Open(filename, "rb")) == NULL)
        {
            sprintf(buff, "Not Enough Memory > %s", filename);
            Error_Warn(TRUE, buff);
            return FALSE;
        }
        File_Seek((*fsp)->filehandle, 128, SEEK_SET);
    }
    (*fsp)->fsLoadBuffer = Mem_Alloc(20000);
    return TRUE;
}

B32 Flic_LoadHeader(const char* filename, lpFlic* fsp)
{
    lpFile headerhandle;
    char buff[128];

    if ((headerhandle = File_Open(filename, "rb")) == NULL)
    {
        sprintf(buff, "Flic File Not Found %s", filename);
        Error_Warn(TRUE, buff);
        return FALSE;
    }

    File_Seek(headerhandle, 0, SEEK_SET);
    File_Read((char*)&(*fsp)->fsHeader, sizeof(FLICHEADERSTRUCT), 1, headerhandle);

    if (((*fsp)->fsHeader.size) != File_Length(headerhandle))
    {
        File_Close(headerhandle);
        sprintf(buff, "Flic File Invalid %s", filename);
        Error_Warn(TRUE, buff);
        return FALSE;
    }

    switch((*fsp)->fsHeader.magic)
    {
        case 0xaf11:
        case 0xaf12:
        case 0x9119:
        case 0xaf43:
        case 0x1234:
            break;
        default:
        {
            File_Close(headerhandle);
            sprintf(buff, "Flic File Invalid %s", filename);
            Error_Warn(TRUE, buff);
            return FALSE;
        }
    }
    File_Close(headerhandle);
    return TRUE;
}

B32 Flic_Close(lpFlic fsp)
{
    if (!fsp)
        return FALSE;

    if ((fsp->userFlags & FLICMEMORY) == FLICMEMORY)
        Mem_Free(fsp->rambufferhandle);
    else
        File_Close(fsp->filehandle);

    if (fsp->fsLoadBuffer)
    {
        Mem_Free(fsp->fsLoadBuffer);
        fsp->fsLoadBuffer = NULL;
    }

    fsp = NULL;

    return TRUE;
}

U32 Flic_GetWidth(lpFlic fsp)
{
    return fsp->fsXsize;
}

U32 Flic_GetHeight(lpFlic fsp)
{
    return fsp->fsYsize;
}

B32 Flic_Animate(lpFlic fsp, Area2F* destArea, B32 advance, B32 trans)
{
    DDSURFACEDESC2 dds;
    HRESULT hr;
    DDBLTFX ddBltFx;
    LPDIRECTDRAWSURFACE4 BackBuffer = DirectDraw_bSurf();
    LPDIRECTDRAWSURFACE4 *lpBB = &BackBuffer;
    RECT destRect = {
        (U32) destArea->x,
        (U32) destArea->y,
        (U32) destArea->x + (U32) destArea->width,
        (U32) destArea->y + (U32) destArea->height
    };
    RECT *dest = &destRect;
    S32 flicRetVal = FLICNOERROR;

    ZeroMemory(&dds, sizeof(dds));
    dds.dwSize = sizeof(dds);
    hr = fsp->fsSurface->lpVtbl->Lock(fsp->fsSurface, NULL, &dds, DDLOCK_WAIT, NULL);
    fsp->fsSPtr = dds.lpSurface;
    fsp->fsPitch = dds.lPitch;
    fsp->is15bit = dds.ddpfPixelFormat.dwGBitMask == 0x3E0;

    if (fsp->currentframe == 0)
        advance = TRUE;

    if (advance)
    {
        if (((fsp->userFlags) & FLICMEMORY) == FLICMEMORY)
            flicRetVal = Flic_Memory(fsp);
        else
            flicRetVal = Flic_Load(fsp);
    }

    hr = fsp->fsSurface->lpVtbl->Unlock(fsp->fsSurface, NULL);

    ZeroMemory(&ddBltFx, sizeof(DDBLTFX));
    ddBltFx.dwSize = sizeof(DDBLTFX);
    ddBltFx.dwFillColor = 0xFF00;

    hr = (*lpBB)->lpVtbl->Blt(*lpBB, dest, fsp->fsSurface, NULL, DDBLT_WAIT | (trans ? DDBLT_KEYSRC : 0), &ddBltFx);

    return (flicRetVal == FLICNOERROR);
}

S32 Flic_Memory(lpFlic fsp)
{
    S32 flicRetVal = FLICNOERROR;

    Flic_FindChunk(fsp);

    if (fsp->currentframe == 0)
        fsp->ringframe = fsp->pointerposition;

    fsp->currentframe++;
    fsp->overallframe++;

    if ((fsp->userFlags & FLICLOOPINGON) == FLICLOOPINGON)
    {
        if (fsp->currentframe == fsp->fsHeader.frames + 1)
        {
            fsp->pointerposition = fsp->ringframe;
            fsp->currentframe = 1;
        }
    }
    else
    {
        if (fsp->currentframe == fsp->fsHeader.frames + 1)
            flicRetVal = FLICFINISHED;
    }

    return flicRetVal;
}

S32 Flic_Load(lpFlic fsp)
{
    S32 flicRetVal = FLICNOERROR;
    S16* source;

    source = fsp->fsLoadBuffer;

    if (fsp->currentframe == 0)
        fsp->ringframe = fsp->pointerposition;

    File_Read(source, 16, 1, fsp->filehandle);
    Flic_FindChunk(fsp);

    fsp->currentframe++;
    fsp->overallframe++;

    if ((fsp->userFlags & FLICLOOPINGON) == FLICLOOPINGON)
    {
        if (fsp->currentframe > fsp->fsHeader.frames)
        {
            File_Seek(fsp->filehandle, fsp->ringframe, SEEK_SET);
            fsp->pointerposition = fsp->ringframe;
            fsp->currentframe = 0;
        }
    }
    else
    {
        if (fsp->currentframe > fsp->fsHeader.frames)
            flicRetVal = FLICFINISHED;
    }
    return flicRetVal;
}

S32 Flic_FindChunk(lpFlic fsp)
{
    U16 chunktype;
    S32 flag;
    void* source;
    S32 storepointer, originalpointer;

    if (((fsp->userFlags) & FLICMEMORY) == FLICMEMORY)
        source = fsp->rambufferhandle;
    else
        source = fsp->fsLoadBuffer;

    originalpointer = fsp->pointerposition;

    if (((fsp->userFlags) & FLICMEMORY) == FLICMEMORY)
    {
        source = (char*)source + fsp->pointerposition;
        storepointer = *(S32*)(char*)source;
        flag = 0;
        while((flag == 0) && ((fsp->pointerposition) < (fsp->fsHeader.size)))
        {
            chunktype = (*((U16*)source + 2));
            switch (chunktype)
            {
                case (U16)0xf1fa:
                {
                    Flic_FrameChunk(fsp);
                    flag = 1;
                    break;
                }
                case 0x4b4c:
                {
                    Flic_LoadPointers(fsp);
                    fsp->pointerposition += *(S32*)source;
                    source = (char*) source + (*(S32*)source);
                    storepointer += (*(S32*)source);
                    break;
                }
                case 0x000b:
                {
                    Flic_LoadPalette64(fsp);
                    fsp->pointerposition += *(S32*)source;
                    source = (char*) source + (*(S32*)source);
                    storepointer += (*(S32*)source);
                    break;
                }
                default:
                {
                    fsp->pointerposition += *(S32*)source;
                    source = (char*) source + (*(S32*)source);
                    storepointer += (*(S32*)source);
                    break;
                }
            }
        }
    }
    else
    {
        storepointer = (*(S32 *) source);
        flag = 0;
        while ((flag == 0) && ((fsp->pointerposition) < (fsp->fsHeader.size)))
        {
            chunktype = (*((U16*) source + 2));
            switch (chunktype)
            {
                case (U16)0xf1fa:
                {
                    Flic_FrameChunk(fsp);
                    flag = 1;
                    break;
                }
                case 0x4b4c:
                    Flic_LoadPointers(fsp);
                    fsp->pointerposition += *(S32*)source;
                    File_Seek(fsp->filehandle, fsp->pointerposition, SEEK_SET);
                    source = fsp->fsLoadBuffer;
                    File_Read((char*)source, 16, 1, fsp->filehandle);
                    storepointer += (*(S32*)source);
                    break;
                case 0x000b:
                    Flic_LoadPalette64(fsp);
                    fsp->pointerposition += *(S32*)source;
                    File_Seek(fsp->filehandle, fsp->pointerposition, SEEK_SET);
                    source = fsp->fsLoadBuffer;
                    File_Read((char*)source, 16, 1, fsp->filehandle);
                    storepointer += (*(S32*)source);
                    break;
                    break;
                default:
                    fsp->pointerposition += *(S32*)source; // this bit must load in the next
                    File_Seek(fsp->filehandle, fsp->pointerposition, SEEK_SET);
                    source = fsp->fsLoadBuffer;
                    File_Read((char*)source, 16, 1, fsp->filehandle);
                    storepointer += (*(S32*)source);
                    break;
            }
        }
    }

    fsp->pointerposition = storepointer + originalpointer;
    return FLICNOERROR;
}

B32 Flic_FrameChunk(lpFlic fsp)
{
    S32 flicmaxchunk;
    char chunkcount;
    char *source;
    char buff[128];

    if (((fsp->userFlags) & FLICMEMORY) == FLICMEMORY)
    {
        source = fsp->rambufferhandle;
        source += (fsp->pointerposition);
        chunkcount = *(source + 6);
        source += 16;
        fsp->pointerposition += 16;
        while ((chunkcount > 0) && ((fsp->pointerposition) < (fsp->fsHeader.size)))
        {
            fsp->fsSource = source;
            Flic_DoChunk(fsp);
            fsp->pointerposition += *(S32*)source;
            source = (char*) source + (*(S32*)source);
            chunkcount--;
        }
    }
    else
    {
        source = fsp->fsLoadBuffer;
        chunkcount = (*((char*)source + 6));
        flicmaxchunk = (*(S32*)source) - 16;
        if (flicmaxchunk > fsp->fsLoadBufferSize)
        {
            if (fsp->fsLoadBuffer)
                Mem_Free(fsp->fsLoadBuffer);
            fsp->fsLoadBufferSize = flicmaxchunk;
            fsp->fsLoadBuffer = Mem_Alloc(fsp->fsLoadBufferSize);
            if (!fsp->fsLoadBuffer)
            {
                sprintf(buff, "Flic Not Enough Memory");
                Error_Warn(TRUE, buff);
                return FALSE;
            }
            source = (fsp->fsLoadBuffer);
        }

        File_Read(source, flicmaxchunk, 1, fsp->filehandle);
        fsp->pointerposition += 16;
        while ((chunkcount > 0) && ((fsp->pointerposition) < (fsp->fsHeader.size)))
        {
            fsp->fsSource = source;
            Flic_DoChunk(fsp);
            fsp->pointerposition += *(S32*)source;
            source = (char*)source + (*(S32*)source);
            chunkcount--;
        }
    }

    return TRUE;
}

// Function to load pointers for each frame of flic
S32 Flic_LoadPointers(lpFlic fsp)
{
    S32 flicmaxchunk;
    char chunkcount;
    void* source;
    char buff[256];

    if (((fsp->userFlags) & FLICMEMORY) == FLICMEMORY)
    {
        source = fsp->rambufferhandle;
        source = (char*)source + fsp->pointerposition;
    }
    else
    {
        source = fsp->fsLoadBuffer;
        flicmaxchunk = (*(S32*)source);
        if (flicmaxchunk > fsp->fsLoadBufferSize)
        {
            if (fsp->fsLoadBuffer)
                Mem_Free(fsp->fsLoadBuffer);
            fsp->fsLoadBufferSize = flicmaxchunk;
            fsp->fsLoadBuffer = Mem_Alloc(fsp->fsLoadBufferSize);
            if (!fsp->fsLoadBuffer)
            {
                sprintf(buff, "Flic Not Enough Memory");
                Error_Warn(TRUE, buff);
                return FALSE;
            }
            source = fsp->fsLoadBuffer;
        }

        File_Seek(fsp->filehandle, fsp->pointerposition, SEEK_SET);
        File_Read(source, flicmaxchunk, 1, fsp->filehandle);
    }

    chunkcount = flicmaxchunk / 8;
    source = (char*) source + 6;

    return FLICNOERROR;
}

// Function to load an 8 bit palette
S32 Flic_LoadPalette64(lpFlic fsp)
{
    S32 flicmaxchunk;
    char *source;
    char buff[128];

    if (((fsp->userFlags) & FLICMEMORY) == FLICMEMORY)
    {
        source = fsp->rambufferhandle;
        source = (char*)source + fsp->pointerposition;
    }
    else
    {
        source = fsp->fsLoadBuffer;
        flicmaxchunk = (*(S32*)source);
        if (flicmaxchunk > fsp->fsLoadBufferSize)
        {
            if (fsp->fsLoadBuffer)
                Mem_Free(fsp->fsLoadBuffer);
            fsp->fsLoadBufferSize = flicmaxchunk;
            fsp->fsLoadBuffer = Mem_Alloc(fsp->fsLoadBufferSize);
            if (!fsp->fsLoadBuffer)
            {
                sprintf(buff, "Flic Not Enough Memory");
                Error_Warn(TRUE, buff);
                return TRUE;
            }
            source = fsp->fsLoadBuffer;
        }

        File_Seek(fsp->filehandle, fsp->pointerposition, SEEK_SET);
        File_Read(source, flicmaxchunk, 1, fsp->filehandle);
    }
    return FLICNOERROR;
}

S32 Flic_DoChunk(lpFlic fsp)
{
    S16 chunktype;
    S32 flag = 0;
    chunktype = (*((S16*)(fsp->fsSource) + 2));
    switch (chunktype)
    {
        case 0x0004:
            Flic_Palette256(fsp);
            flag = 1;
            break;
        case 0x0007:
            Flic_DeltaWord(fsp);
            flag = 1;
            break;
        case 0x000b:
            Flic_Palette64(fsp);
            flag = 1;
            break;
        case 0x000c:
            Flic_DeltaByte(fsp);
            flag = 1;
            break;
        case 0x000d:
            Flic_Black(fsp);
            flag = 1;
            break;
        case 0x000f:
            Flic_BrunDepack(fsp);
            flag = 1;
            break;
        case 0x0010:
            Flic_Copy(fsp);
            flag = 1;
            break;
        case 0x0019:
            Flic_BrunDepack(fsp);
            flag = 1;
            break;
        case 0x001b:
            Flic_DeltaWord(fsp);
            flag = 1;
            break;
        case 0x5344:
            //Flic_SoundChunk(fsp);
            flag = 1;
            break;
        case 0x5555:
            Flic_Unpack(fsp);
            flag = 1;
            break;
        default:
            break;
    }

    return flag;
}

B32 Flic_Palette256(lpFlic fsp)
{
    U8* src;
    U8* dst, col;
    U16 cnt, indx, ccnt;

    src = fsp->fsSource;
    src += 6;

    cnt = *(U16*)src;
    src += 2;

    dst = (U16*)&fsp->fsPalette256;

    while (cnt)
    {
        indx = *src++;
        dst += (indx * 3);
        ccnt = *src++;

        if (!ccnt)
        {
            cnt = 1;
            ccnt = 256;
            dst = (U8*)&fsp->fsPalette256;
        }
        ccnt *= 3;
        while (ccnt)
        {
            col = *src++;
            col >>= 2;

            *dst++ = col;
            ccnt--;
        }
        cnt--;
    }

    if (fsp->fsDisplayMode == FLICMODE_HICOLOR)
    {
        FlicCreateHiColorTable(fsp);
    }
    return TRUE;
}

B32 Flic_DeltaWord(lpFlic fsp)
{
    if (fsp->fsHeader.depth == 8)
    {
        if (fsp->fsDisplayMode == FLICMODE_BYTEPERPIXEL)
            FlicDeltaWordBytePerPixel(fsp);
        if (fsp->fsDisplayMode == FLICMODE_HICOLOR)
            FlicDeltaWordHiColor(fsp);
    }
    else
    {
        if (fsp->fsHeader.depth == 16)
        {
            if (fsp->fsHeader.magic == 0x1234)
                FlicDeltaWordHiColorDZ(fsp);
            else if (fsp->fsHeader.magic == 0xaf43)
                FlicDeltaWordHiColorFlic32k(fsp);
            else
                FlicDeltaWordHiColorFlic(fsp);
        }
    }
    return TRUE;
}

B32 Flic_Palette64(lpFlic fsp)
{
    return TRUE;
}

B32 Flic_DeltaByte(lpFlic fsp)
{
    return TRUE;
}

B32 Flic_Black(lpFlic fsp)
{
    return TRUE;
}

B32 Flic_BrunDepack(lpFlic fsp)
{
    if (fsp->fsHeader.depth == 8)
    {
        if (fsp->fsDisplayMode == FLICMODE_BYTEPERPIXEL)
            FlicBRunDepackBytePerPixel(fsp);
        if (fsp->fsDisplayMode == FLICMODE_HICOLOR)
            FlicBRunDepackHiColor(fsp);
    }
    else if (fsp->fsHeader.depth == 16)
    {
        if (fsp->fsHeader.magic == 0xaf43)
            FlicBRunDepackHiColorFlic32k(fsp);
        else
            FlicBRunDepackHiColorFlic(fsp);
    }
    return TRUE;
}

B32 Flic_Copy(lpFlic fsp)
{
    if (fsp->fsBitPlanes == 16)
    {
        FlicCopyHiColorFlic(fsp);
        return TRUE;
    }
    if (fsp->fsDisplayMode == FLICMODE_BYTEPERPIXEL)
        FlicCopyBytePerPixel(fsp);
    if (fsp->fsDisplayMode == FLICMODE_HICOLOR)
        FlicCopyHiColor(fsp);

    return TRUE;
}

S32 Flic_Unpack(lpFlic fsp)
{
    return 0;
}

void FlicCreateHiColorTable(lpFlic fsp)
{
    U8* src;
    U16 *dst, wrd, col;
    S32 i;

    src = (U8*)&fsp->fsPalette256;
    dst = (U16*)&fsp->fsPalette64k;

    for (i = 0; i < 256; i++)
    {
        col = src[0];
        col >>= 1;
        col <<= 11;

        wrd = src[1];
        wrd <<= 5;

        col |= wrd;

        wrd = src[2];
        wrd >>= 1;

        col |= wrd;

        *dst++ = col;

        src += 3;
    }
    fsp->fsPalette64k[0] = 0x00;
}

void FlicDeltaWordBytePerPixel(lpFlic fsp)
{
    // Empty on purpose
}

void FlicDeltaWordHiColor(lpFlic fsp)
{
    U8 *dst;
    U8 *src, w1, w2, mfval;
    U16 wrd, cnt, ctrl, cpy, mcnt, mflag, line, height, offs;

    dst = fsp->fsSPtr;
    height = fsp->fsYsize;

    src = fsp->fsSource;
    src += 6;

    line = 0;
    cnt = *(U16*)src;
    src += 2;

    while (cnt)
    {
        dst = (U8*)(fsp->fsSPtr) + ((fsp->fsPitch) * line);

        ctrl = *(U16*)src;
        ctrl >>= 12;
        ctrl &= 0xc;
        if (ctrl == 0xc)
        {
            wrd = *(U16*)src;
            src += 2;
            wrd = 65536 - wrd;

            line += wrd;
        }
        else if (ctrl == 0x4)
        {
            return;
        }
        else
        {
            mflag = 0;
            if (ctrl == 0x8)
            {
                mfval = *src;
                src += 2;
                mflag = 1;
            }
            mcnt = *(U16*)src;
            src += 2;
            while(mcnt)
            {
                offs = *src++;
                dst += 2 * offs;
                cpy = *src++;
                if ((cpy & 0x80))
                {
                    cpy = 256 - cpy;
                    w1 = *src++;
                    w2 = *src++;
                    while (cpy)
                    {
                        *(U16*)dst = FHCOL(w1);
                        dst += 2;
                        *(U16*)dst = FHCOL(w2);
                        dst += 2;

                        cpy--;
                    }
                }
                else
                {
                    while (cpy)
                    {
                        w1 = *src++;
                        w2 = *src++;
                        *(U16*)dst = FHCOL(w1);
                        dst += 2;
                        *(U16*)dst = FHCOL(w2);
                        dst += 2;

                        cpy--;
                    }
                }
                mcnt--;
            }
            if (mflag)
                *(U16*)dst = FHCOL(mfval);
            cnt--;
            line++;
        }
    }
}

void FlicDeltaWordHiColorDZ(lpFlic fsp)
{
    // Empty on purpose
}

void FlicDeltaWordHiColorFlic32k(lpFlic fsp)
{
    U8* dst;
    U8* src;
    U16 wrd, cnt, ctrl, cpy, mcnt, tmp, mflag, mfval, line, height;

    dst = fsp->fsSPtr;
    height = fsp->fsYsize;

    src = fsp->fsSource;
    src += 6;

    line = 0;
    cnt = *(U16*)src;
    src += 2;
    while (cnt)
    {
        dst = (U8*)(fsp->fsSPtr) + ((fsp->fsPitch) * line);

        ctrl = *(U16*)src;
        ctrl >>= 12;
        ctrl &= 0xc;
        if (ctrl == 0xc)
        {
            wrd = *(U16*)src;
            src += 2;
            wrd = 65536 - wrd;

            line += wrd;
        }
        else if (ctrl == 0x4)
        {
            return;
        }
        else
        {
            mflag = 0;
            if (ctrl == 0x8)
            {
                mfval = DoScreenConversion(fsp, *(U16*)src);
                src += 2;
                mflag = 1;
            }
            mcnt = *(U16*)src;
            src += 2;
            while (mcnt)
            {
                dst += 2 * (*src++);
                cpy = *src++;
                if ((cpy & 0x80))
                {
                    tmp = *(U16*)src;
                    src += 2;
                    wrd = ((tmp & 0xffe0) << 1);
                    wrd |= tmp & 0x1f;
                    cpy = 256 - cpy;
                    while (cpy)
                    {
                        *(U16*)dst = DoScreenConversion(fsp, wrd);
                        dst += 2;
                        cpy--;
                    }
                }
                else
                {
                    while (cpy)
                    {
                        tmp = *(U16*)src;
                        src += 2;
                        wrd = ((tmp & 0xffe0) << 1);
                        wrd |= tmp & 0x1f;
                        *(U16*) dst = DoScreenConversion(fsp, wrd);
                        dst += 2;

                        cpy--;
                    }
                }
                mcnt--;
            }
            if (mflag)
                *dst = (char)mfval;
            line++;
            cnt--;
        }
    }
}

void FlicDeltaWordHiColorFlic(lpFlic fsp)
{
    // Empty on purpose
}

B32 FlicBRunDepackBytePerPixel(lpFlic fsp)
{
    return TRUE;
}

B32 FlicBRunDepackHiColor(lpFlic fsp)
{
    U8* src, wrd;
    U16 *dst;
    U16 cnt, width, height, line;

    // 421 6259 karl

    src = fsp->fsSource;
    src += 6;

    height = fsp->fsYsize;
    line = 0;
    src++;

    while (line < height)
    {
        width = fsp->fsXsize;
        dst = (U16*)(fsp->fsSPtr) + (fsp->fsPitch / 2) * line;

        while (width > 0)
        {
            cnt = *src++;

            if (cnt < 128)
            {
                width -= cnt;
                wrd = *src++;
                while (cnt)
                {
                    *dst++ = FHCOL(wrd);
                    cnt--;
                }
            }
            else
            {
                cnt = 256 - cnt;
                width -= cnt;
                while (cnt)
                {
                    wrd = *src++;
                    *dst++ = FHCOL(wrd);
                    cnt--;
                }
            }
        }

        line++;
        src++;
    }
    return TRUE;
}

B32 FlicBRunDepackHiColorFlic32k(lpFlic fsp)
{
    S16 height, width;
    U8* src;
    U16 wrd, cnt, tmp, line;
    U16 *dst;

    dst = fsp->fsSPtr;
    src = fsp->fsSource;
    src += 6;
    src++;

    line = 0;
    height = fsp->fsYsize;
    while (line < height)
    {
        width = fsp->fsXsize;
        dst = (U16*)(fsp->fsSPtr) + ((fsp->fsPitch / 2) * line);

        while (width > 0)
        {
            cnt = *src++;
            if (cnt < 128)
            {
                width -= cnt;
                wrd = *(U16*)src;
                src += 2;
                tmp = ((wrd & 0xffe0) << 1);
                tmp |= wrd & 0x1f;
                wrd = tmp;
                while (cnt)
                {
                    *dst++ = DoScreenConversion(fsp, wrd);
                    cnt--;
                }
            }
            else
            {
                cnt = 256 - cnt;
                width -= cnt;
                while (cnt)
                {
                    tmp = *(U16*)src;
                    src += 2;
                    wrd = ((tmp & 0xffe0) << 1);
                    wrd |= tmp & 0x1f;
                    *dst++ = DoScreenConversion(fsp, wrd);

                    cnt--;
                }
            }
        }

        src++;
        line++;
    }
    return TRUE;
}

B32 FlicBRunDepackHiColorFlic(lpFlic fsp)
{
    return TRUE;
}

void FlicCopyHiColorFlic(lpFlic fsp)
{
    // Empty on purpose
}

void FlicCopyBytePerPixel(lpFlic fsp)
{
    // Empty on purpose
}

void FlicCopyHiColor(lpFlic fsp)
{
    // Empty on purpose
}

U16 getFlicCol(U8 n, lpFlic fsp)
{
    U16 ret;
    U16 *ctab;

    ctab = fsp->fsPalette64k;

    ret = ctab[n];

    return ret;
}
