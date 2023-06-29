#include "flic.h"

B32 Flic_Setup(const char* filename, lpFlic* fsp, FlicUserFlags flags)
{
    // TODO: Implement Flic_Setup
    return FALSE;
}

B32 Flic_Close(lpFlic fsp)
{
    // TODO: Implement Flic_Close
    return FALSE;
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
