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
    // TODO: Implement Flic_FindChunk
    return 0;
}
