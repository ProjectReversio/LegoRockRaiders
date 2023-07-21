#include "lws.h"

Lws_Globs lwsGlobs = { NULL };

void Lws_Initialize(const char* sharedDir, B32 (*FindSFXID)(const char* name, SFX_ID* sfxID), S32 (*PlaySample3D)(lpContainer cont, SFX_ID type, B32 loop, B32 onCont, Point3F* wPos), B32 (*SoundEnabled)())
{
    // TODO: Implement Lws_Initialize
}

lpLws_Info Lws_Parse(const char* fname, B32 looping)
{
    // TODO: Implement Lws_Parse
    return NULL;
}

void Lws_LoadMeshes(lpLws_Info scene, LPDIRECT3DRMFRAME3 parent)
{
    // TODO: Implement Lws_LoadMeshes
}

void Lws_SetTime(lpLws_Info scene, F32 time)
{
    // TODO: Implement Lws_SetTime
}

U32 Lws_GetFrameCount(lpLws_Info scene)
{
    return scene->lastFrame;
}
