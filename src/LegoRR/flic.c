#include "flic.h"
#include "file.h"
#include "mem.h"

B32 Flic_Setup(const char* filename, lpFlic* fsp, FlicUserFlags flags)
{
    // TODO: Implement Flic_Setup
    return FALSE;
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
    // TODO: Implement Flic_Animate
    return FALSE;
}
