#include "input.h"
#include "main.h"
#include <windows.h>

Input_Globs inputGlobs = { NULL };

B32 Input_InitKeysAndDI()
{
    // TODO: Implement Input_InitKeysAndDI
    return FALSE;
}

void Input_ReleaseKeysAndDI()
{
    // TODO: Implement Input_ReleaseKeysAndDI
}

void Input_ReadKeys()
{
    // TODO: Implement Input_ReadKeys
}

void Input_ReadMouse2()
{
    RECT rect;
    POINT clientPos, cursorPos;

    if (GetClientRect(Main_hWnd(), &rect))
    {
        clientPos.x = clientPos.y = 0;
        ClientToScreen(Main_hWnd(), &clientPos);
        GetCursorPos(&cursorPos);

        cursorPos.x -= clientPos.x;
        cursorPos.y -= clientPos.y;

        if (cursorPos.x >= appWidth())
            cursorPos.x = appWidth() - 1;
        if (cursorPos.y >= appHeight())
            cursorPos.y = appHeight() - 1;
        if (cursorPos.x < 0)
            cursorPos.x = 0;
        if (cursorPos.y < 0)
            cursorPos.y = 0;

        inputGlobs.diffx = cursorPos.x - inputGlobs.msx;
        inputGlobs.diffy = cursorPos.y - inputGlobs.msy;
        inputGlobs.msx = cursorPos.x;
        inputGlobs.msy = cursorPos.y;
    }
}

U32 Input_AnyKeyPressed()
{
    // TODO: Implement Input_AnyKeyPressed
    return 0;
}
