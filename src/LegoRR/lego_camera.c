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

void Camera_InitCameraMovements(F32 cameraSpeed, F32 cameraDropOff, F32 cameraAcceleration, S32 mouseScrollIndent)
{
    // TODO: Implement Camera_InitCameraMovements
}

void Camera_SetZoomRange(lpLegoCamera cam, F32 minDist, F32 maxDist)
{
    // TODO: Implement Camera_SetZoomRange
}

void Camera_SetTiltRange(lpLegoCamera cam, F32 minTilt, F32 maxTilt)
{
    // TODO: Implement Camera_SetTiltRange
}
