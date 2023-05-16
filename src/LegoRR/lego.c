#include "lego.h"
#include <string.h>
#include "types.h"
#include "main.h"

Lego_Globs legoGlobs;

B32 Gods_Go(const char* programName)
{
    memset(&legoGlobs, NULL, sizeof(legoGlobs));
    legoGlobs.gameName = programName;
    Main_SetTitle(programName);

    Main_State mainState;
    mainState.Initialize = Lego_Initialize;
    mainState.MainLoop = Lego_MainLoop;
    mainState.Shutdown = Lego_Shutdown_Quick;

    if (Main_ProgrammerMode() == PROGRAMMER_MODE_10)
        mainState.Shutdown = Lego_Shutdown_Full; // Full shutdown with proper cleanup.

    return Main_SetState(&mainState);
}

B32 Lego_Initialize()
{
    // TODO: Implement Lego_Initialize

    return TRUE;
}

B32 Lego_Shutdown_Full()
{
    // TODO: Implement Lego_Shutdown_Full

    return TRUE;
}

B32 Lego_Shutdown_Quick()
{
    // TODO: Implement Lego_Shutdown_Quick
    return TRUE;
}

B32 Lego_MainLoop(F32 time)
{
    // TODO: Implement Lego_MainLoop

    return TRUE;
}
