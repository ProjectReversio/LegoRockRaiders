#pragma once

#include "types.h"
#include "movie.h"
#include "config.h"

typedef enum Menu_ScreenType
{
    Menu_Screen_Title = 0,
    Menu_Screen_Missions = 1,
    Menu_Screen_Training = 2,
    Menu_Screen_Load_unused = 3,
    Menu_Screen_Save = 4,
    Menu_Screen_Count = 5,
} Menu_ScreenType;

extern B32 Front_IsFrontEndEnabled();
extern B32 Front_IsIntrosEnabled();

extern void Front_PlayIntroMovie(const char* aviKey, B32 skippable);
extern void Front_PlayIntroSplash(const char* imageKey, B32 skippable, const char* timeKey);

///    Plays an already-loaded Movie_t from the G98CMovie C wrapper API.
///    Allows terminating the movie playback during runtime with isSkippable.
extern void Front_PlayMovie(Movie_t* mov, B32 skippable);

extern void Front_Initialize(lpConfig config);

extern void Front_RunScreenMenuType(Menu_ScreenType screenType);
extern B32 Front_IsTriggerAppQuit();

extern const char* Front_GetSelectedLevel();
extern void Front_LoadOptionParameters(B32 loadOptions, B32 resetFront);
