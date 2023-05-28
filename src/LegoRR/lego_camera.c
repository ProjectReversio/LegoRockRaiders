#include "lego_camera.h"
#include "mem.h"

lpLegoCamera Camera_Create(lpContainer root, LegoCamera_Type camType)
{
    lpLegoCamera cam = Mem_Alloc(sizeof(LegoCamera));
    if (!cam)
        return NULL;

    // TODO: Implement Camera_Create
    return cam;
}
