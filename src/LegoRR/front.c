#include "front.h"
#include "lego.h"

B32 Front_IsFrontEndEnabled()
{
    B32 result = Config_GetBoolValue(legoGlobs.config, Config_BuildStringID(legoGlobs.gameName, "Main", "FrontEnd", 0));

    return result == 1 && Main_ProgrammerMode() < PROGRAMMER_MODE_3;
}

B32 Front_IsIntrosEnabled()
{
    B32 result = Config_GetBoolValue(legoGlobs.config, Config_BuildStringID(legoGlobs.gameName, "Main", "DontPlayAvis", 0));

    return result != 1 && Main_ProgrammerMode() < PROGRAMMER_MODE_3;
}

void Front_PlayIntroMovie(const char* aviKey, B32 skippable)
{
    if (aviKey == NULL)
        return;

    const char* fName = Config_GetStringValue(legoGlobs.config, Config_BuildStringID(legoGlobs.gameName, "Main", aviKey, 0));
    if (fName == NULL)
        return;

    // TODO: Implement Front_PlayIntroMovie
}

void Front_PlayIntroSplash(const char* imageKey, B32 skippable, const char* timeKey)
{
    F32 time = Config_GetFloatValue(legoGlobs.config, Config_BuildStringID(legoGlobs.gameName, "Main", timeKey, 0));

    // TODO: Implement Front_PlayIntroSplash
}
