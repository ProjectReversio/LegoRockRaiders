#include "animation.h"
#include <windows.h>
#include <ddraw.h>
#include <vfw.h>

LPDIRECTDRAW4 ddraw = NULL;

void Animation_Initialize(LPDIRECTDRAW4 directDraw)
{
    ddraw = directDraw;
    AVIFileInit();
}
