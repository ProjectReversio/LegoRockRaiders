#pragma once

#include "platform.h"
#include <amstream.h>

#ifdef __cplusplus
extern "C"
{
#endif
    typedef struct G98CMovie Movie_t;

    extern Movie_t *Movie_Load(const char *fName);
    extern void Movie_Free(Movie_t *mov);

    extern B32 Movie_Update(Movie_t *mov, F32 speed, RECT *destRect);

    extern U64 Movie_GetDuration(Movie_t *mov);
#ifdef __cplusplus
}
#endif
