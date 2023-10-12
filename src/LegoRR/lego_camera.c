#include "lego_camera.h"
#include "mem.h"

lpLegoCamera Camera_Create(lpContainer root, LegoCamera_Type camType)
{
    lpLegoCamera cam = Mem_Alloc(sizeof(LegoCamera));
    if (!cam)
        return NULL;

    cam->type = camType;
    cam->trackObject = NULL;
    cam->trackRadarZoom = 0.0f;
    cam->trackRadarZoomSpeed = 0.0f;
    cam->flags = CAMERA_FLAG_NONE;
    cam->rotation = 0.0f;
    cam->tilt = 0.0f;
    cam->zoom = 200.0f;
    cam->trackFPCameraFrame = 0;
    cam->moveVector.x = 0.0f;
    cam->moveVector.y = 0.0f;
    cam->moveVector.z = 0.0f;
    cam->moveSpeed = 0.0f;
    cam->shakeTimer = 0.0f;
    cam->shakeDuration = 0.0f;
    cam->shakeIntensity = 0.0f;
    cam->shakeVector.x = 0.0f;
    cam->shakeVector.y = 0.0f;
    cam->shakeVector.z = 0.0f;

    cam->contCam = Container_Create(NULL);

    if (!cam->contCam)
    {
        Mem_Free(cam);
        return NULL;
    }

    Container_SetPosition(cam->contCam, NULL, 0.0f, 0.0f, -cam->zoom);
    Container_SetOrientation(cam->contCam, NULL, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f);

    if (camType == LegoCamera_FP)
    {
        Container_SetParent(cam->contCam, root);
        return cam;
    }

    cam->cont2 = Container_Create(root);
    if (!cam->cont2)
    {
        Container_Remove(cam->contCam);
        Mem_Free(cam);
        return NULL;
    }

    cam->cont3 = Container_Create(root);
    if (!cam->cont3)
    {
        Container_Remove(cam->cont2);
        Container_Remove(cam->contCam);
        Mem_Free(cam);
        return NULL;
    }

    cam->cont4 = Container_Create(cam->cont2);
    if (!cam->cont4)
    {
        Container_Remove(cam->cont3);
        Container_Remove(cam->cont2);
        Container_Remove(cam->contCam);
        Mem_Free(cam);
        return NULL;
    }

    if (camType == LegoCamera_Top)
    {
        cam->contListener = Container_Create(cam->cont4);
        if (cam->contListener)
            Container_SetPosition(cam->contListener, NULL, 0.0f, 0.0f, -80.0f);
    }

    Container_SetParent(cam->contCam, cam->cont4);

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

void Camera_SetTopdownPosition(lpLegoCamera cam, F32 x, F32 y)
{
    if (cam->type == LegoCamera_Top)
        Container_SetPosition(cam->cont2, NULL, x, y, 0.0f);
}

void Camera_SetRotation(lpLegoCamera cam, F32 yaw)
{
    if (((cam->flags & CAMERA_FLAG_ROTATIONRANGE) == CAMERA_FLAG_NONE ||
        (cam->flags & CAMERA_FLAG_FREEMOVEMENT) != CAMERA_FLAG_NONE) ||
        ((cam->rotationRange.min <= yaw && yaw <= cam->rotationRange.max)))
    {
        cam->rotation = yaw;
        Container_SetOrientation(cam->cont2, NULL, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f);
        Container_AddRotation(cam->cont2, Container_Combine_Before, 0.0f, 0.0f, 1.0f, -yaw);
    }
}

void Camera_SetTilt(lpLegoCamera cam, F32 tilt)
{
    if (((cam->flags & CAMERA_FLAG_TILTRANGE) == CAMERA_FLAG_NONE ||
        (cam->flags & CAMERA_FLAG_FREEMOVEMENT) != CAMERA_FLAG_NONE) ||
        ((cam->tiltRange.min <= tilt && tilt <= cam->tiltRange.max)))
    {
        cam->tilt = tilt;
        Container_SetOrientation(cam->cont4, cam->cont2, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f);
        Container_AddRotation(cam->cont4, Container_Combine_Before, 1.0f, 0.0f, 0.0f, -tilt);
    }
}

void Camera_TrackObject(lpLegoCamera cam, lpLegoObject liveObj, F32 trackZoomSpeed, F32 trackDist, F32 trackTilt, F32 trackRotationSpeed)
{
    cam->trackObject = liveObj;
    cam->trackRadarZoomSpeed = trackZoomSpeed;
    cam->trackRadarZoom = trackDist;
    Camera_SetTilt(cam, trackTilt);
    cam->trackRadarRotationSpeed = trackRotationSpeed;
}
