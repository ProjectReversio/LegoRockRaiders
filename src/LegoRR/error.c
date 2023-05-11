#include "error.h"
#include <windows.h>

Error_Globs errorGlobs = { NULL };

void Error_Initialize()
{
    errorGlobs.dumpFile = NULL;
    errorGlobs.loadLogFile = NULL;
    errorGlobs.loadErrorLogFile = NULL;
    errorGlobs.redundantLogFile = NULL;
    errorGlobs.fullScreen = FALSE;
}
