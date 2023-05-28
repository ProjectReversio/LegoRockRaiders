#include "front.h"
#include "lego.h"
#include "input.h"
#include "images.h"
#include "mem.h"
#include "movie.h"
#include "file.h"

B32 Front_IsFrontEndEnabled()
{
    Bool3 result = Config_GetBoolValue(legoGlobs.config, Config_BuildStringID(legoGlobs.gameName, "Main", "FrontEnd", 0));

    return result == BOOL3_TRUE && Main_ProgrammerMode() < PROGRAMMER_MODE_3;
}

B32 Front_IsIntrosEnabled()
{
    Bool3 result = Config_GetBoolValue(legoGlobs.config, Config_BuildStringID(legoGlobs.gameName, "Main", "DontPlayAvis", 0));

    return result != BOOL3_TRUE && Main_ProgrammerMode() < PROGRAMMER_MODE_3;
}

void Front_PlayIntroMovie(const char* aviKey, B32 skippable)
{
    if (aviKey == NULL)
        return;

    const char* fName = Config_GetStringValue(legoGlobs.config, Config_BuildStringID(legoGlobs.gameName, "Main", aviKey, 0));
    if (fName == NULL)
        return;

    if (File_Exists(fName))
    {
        Movie_t* mov = Movie_Load(fName);
        Front_PlayMovie(mov, skippable);
        Movie_Free(mov);
        Mem_Free(fName);
    }
}

void Front_PlayIntroSplash(const char* imageKey, B32 skippable, const char* timeKey)
{
    F32 time = Config_GetFloatValue(legoGlobs.config, Config_BuildStringID(legoGlobs.gameName, "Main", timeKey, 0));

    if (time == 0.0f)
        time = 3.0f;

    S32 startTime = timeGetTime() - (S32)(time * -1000.0f);

    if (imageKey == NULL)
        return;

    const char* bmpFile = Config_GetStringValue(legoGlobs.config, Config_BuildStringID(legoGlobs.gameName, "Main", imageKey, 0));

    if (bmpFile == NULL)
        return;

    lpImage img = Image_LoadBMP(bmpFile);
    if (img == NULL)
        return;

    do
    {
        Image_Display(img, 0);
        Main_LoopUpdate(FALSE);
        Sleep(100);
    } while (timeGetTime() < startTime && (!skippable || !Input_AnyKeyPressed() && !inputGlobs.msrb && !inputGlobs.mslb));
    Image_Remove(img);
}

void Front_PlayMovie(Movie_t* mov, B32 skippable)
{
    // HARDCODED SCREEN RESOLUTION
    RECT rect;
    rect.left = 0;
    rect.top = 0;
    rect.right = 640;
    rect.bottom = 480;

    F32 num = 0.0f;
    if (Movie_Update(mov, 1.0f, &rect))
    {
        do
        {
            Main_LoopUpdate(FALSE);
            num -= -1.0f;
        } while (Movie_GetDuration(mov) >= num
            && (!skippable || !Input_AnyKeyPressed() && !inputGlobs.msrb && !inputGlobs.mslb) &&
            Movie_Update(mov, 1.0f, &rect));
    }
}

void Front_Initialize(lpConfig config)
{
    // TODO: Implement Front_Initialize
}

void Front_RunScreenMenuType(Menu_ScreenType screenType)
{
    // TODO: Implement Front_RunScreenMenuType
}

B32 Front_IsTriggerAppQuit()
{
    // TODO: Implement Front_IsTriggerAppQuit
    return FALSE;
}

const char* Front_GetSelectedLevel()
{
    // TODO: Implement Front_GetSelectedLevel
    return NULL;
}

void Front_LoadOptionParameters(B32 loadOptions, B32 resetFront)
{
    // TODO: Implement Front_LoadOptionParameters
}
