#include "lego_camera.h"
#include "mem.h"
#include "lego.h"

Camera_Globs cameraGlobs = { 0 };

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
    cameraGlobs.maxSpeed = cameraSpeed;
    cameraGlobs.deceleration = cameraDropOff;
    cameraGlobs.acceleration = cameraAcceleration;
    cameraGlobs.mouseScrollIndent = mouseScrollIndent;
}

void Camera_SetZoomRange(lpLegoCamera cam, F32 minDist, F32 maxDist)
{
    cam->flags |= CAMERA_FLAG_ZOOMRANGE;
    cam->zoomRange.min = minDist;
    cam->zoomRange.max = maxDist;
}

void Camera_SetTiltRange(lpLegoCamera cam, F32 minTilt, F32 maxTilt)
{
    cam->flags |= CAMERA_FLAG_TILTRANGE;
    cam->tiltRange.min = minTilt;
    cam->tiltRange.max = maxTilt;
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

void Camera_Update(lpLegoCamera cam, lpLego_Level level, F32 elapsedAbs, F32 elapsedGame)
{
    Container_AddTranslation(cam->contCam, Container_Combine_Before, -cam->shakeVector.x, -cam->shakeVector.y, -cam->shakeVector.z);

    if (cam->type == LegoCamera_Top)
    {
        // Decelerate camera movement
        Maths_Vector3DScale(&cam->moveVector, &cam->moveVector, cameraGlobs.deceleration);

        Container_AddTranslation(cam->cont2, Container_Combine_After, cam->moveVector.x, cam->moveVector.y, cam->moveVector.z);

        Point3F top_vecPos;
        Container_GetPosition(cam->cont2, NULL, &top_vecPos);
        Point3F top_vecDir;
        Point3F top_vecUp;
        Container_GetOrientation(cam->cont2, NULL, &top_vecDir, &top_vecUp);

        S32 blockX, blockY;
        if (Map3D_WorldToBlockPos_NoZ(level->map, top_vecPos.x, top_vecPos.y, &blockX, &blockY))
        {
            if (blockX < cameraGlobs.mouseScrollIndent || level->map->blockWidth - cameraGlobs.mouseScrollIndent <= blockX)
                Container_AddTranslation(cam->cont2, Container_Combine_After, -cam->moveVector.x, 0.0f, 0.0f);
            if (blockY < cameraGlobs.mouseScrollIndent || level->map->blockHeight - cameraGlobs.mouseScrollIndent <= blockY)
                Container_AddTranslation(cam->cont2, Container_Combine_After, 0.0f, -cam->moveVector.y, 0.0f);

            F32 worldZ = Map3D_GetWorldZ(level->map, top_vecPos.x, top_vecPos.y);
            Container_SetPosition(cam->cont3, NULL, top_vecPos.x, top_vecPos.y, worldZ);
            Container_SetOrientation(cam->cont3, NULL, top_vecUp.x, top_vecUp.y, 0.0f, 0.0f, 0.0f, -1.0f);

            F32 unknownThing = Map3D_UnkCameraXYFunc_RetZunk(Lego_GetMap(), top_vecPos.x, top_vecPos.y);
            Container_SetPosition(cam->contCam, cam->cont4, 0.0f, 0.0f, -(cam->zoom + -unknownThing));
        }
        else
        {
            // Cancel out moveVector ??
            Container_AddTranslation(cam->cont2, Container_Combine_After, -cam->moveVector.x, -cam->moveVector.y, -cam->moveVector.z);
        }

        if (cam->shakeDuration <= cam->shakeTimer)
        {
            cam->shakeVector.x = 0.0f;
            cam->shakeVector.y = 0.0f;
            cam->shakeVector.z = 0.0f;
        }
        else
        {
            // TODO: Implement Camera_Update
        }
    }
    else if (cam->type == LegoCamera_FP)
    {
        // TODO: Implement Camera_Update
    }
    else if (cam->type == LegoCamera_Radar)
    {
        // TODO: Implement Camera_Update
    }
}
