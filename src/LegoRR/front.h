#pragma once

#include "types.h"
#include "movie.h"

extern B32 Front_IsFrontEndEnabled();
extern B32 Front_IsIntrosEnabled();

extern void Front_PlayIntroMovie(const char* aviKey, B32 skippable);
extern void Front_PlayIntroSplash(const char* imageKey, B32 skippable, const char* timeKey);

///    Plays an already-loaded Movie_t from the G98CMovie C wrapper API.
///    Allows terminating the movie playback during runtime with isSkippable.
extern void Front_PlayMovie(Movie_t* mov, B32 skippable);
