#pragma once

#include "platform.h"
#include "maths.h"
#include "container.h"
#include "lego_object.h"
#include "map3d.h"

typedef enum LegoCamera_Type
{
    LegoCamera_None,
    LegoCamera_Top,
    LegoCamera_Radar,
    LegoCamera_FP,
    LegoCamera_Type_Count,
} LegoCamera_Type;

typedef enum CameraFlags
{
    CAMERA_FLAG_NONE          = 0,
    CAMERA_FLAG_TILTRANGE     = 0x1,
    CAMERA_FLAG_ROTATIONRANGE = 0x2,
    CAMERA_FLAG_ZOOMRANGE     = 0x4,
    CAMERA_FLAG_FREEMOVEMENT  = 0x8,
    CAMERA_FLAG_FPSETUP       = 0x10,
} CameraFlags;

typedef struct LegoCamera
{
    LegoCamera_Type type;
    lpLegoObject trackObject;
    F32 trackRadarZoomSpeed;
    F32 trackRadarZoom;
    F32 trackRadarRotationSpeed; // A constant yaw H rotation speed applied to tracked radar objects
    S32 trackFPCameraFrame; // CameraFrameIndex for FP object SwapPolyFP
    Point3F moveVector; // current 3D (really 2D) movement and directional speed
    lpContainer contCam; // only cont parented for FP type (child of contRoot if FP)
    lpContainer cont2; // child of contRoot
    lpContainer cont3; // child of contRoot
    lpContainer cont4; // child of cont2
    lpContainer contListener; // child of cont4 (topdown type only)
    U8 reserved1[64];
    F32 tilt; // (init: 0.0f) current tilt (Pitch, V Rotation)
    Range2F tiltRange; // min/max tilt (Pitch, V Rotation)
    F32 rotation; // (init: 0.0f) current yaw (Angle, H Rotation)
    Range2F rotationRange; // min/max yaw (Angle, H Rotation)
    F32 zoom; // (init: 200.0f) current dist (Zoom)
    Range2F zoomRange; // min/max dist (Zoom)
    F32 moveSpeed; // current 3D (really 2D) movement speed
    F32 shakeIntensity;
    F32 shakeDuration;
    F32 shakeTimer; // count-up timer until shakeDuration
    Point3F shakeVector;
    CameraFlags flags;
} LegoCamera, *lpLegoCamera;

typedef struct Camera_Globs
{
    F32 maxSpeed; // (cfg: CameraSpeed) maximum move speed of camera
    F32 deceleration; // (cfg: CameraDropOff) movement deceleration
    F32 acceleration; // (cfg: CameraAcceleration) movement acceleration
    S32 mouseScrollIndent; // (cfg: MouseScrollIndent) area around window borders for mouse movement
} Camera_Globs;

extern Camera_Globs cameraGlobs;

extern lpLegoCamera Camera_Create(lpContainer root, LegoCamera_Type camType);

extern void Camera_InitCameraMovements(F32 cameraSpeed, F32 cameraDropOff, F32 cameraAcceleration, S32 mouseScrollIndent);

extern void Camera_SetZoomRange(lpLegoCamera cam, F32 minDist, F32 maxDist);
extern void Camera_SetTiltRange(lpLegoCamera cam, F32 minTilt, F32 maxTilt);

extern void Camera_SetTopdownPosition(lpLegoCamera cam, F32 x, F32 y);
extern void Camera_SetRotation(lpLegoCamera cam, F32 yaw);
extern void Camera_SetTilt(lpLegoCamera cam, F32 tilt);

extern B32 Camera_GetTopdownWorldPos(lpLegoCamera cam, lpMap3D surfMap, Point3F* outWorldPos);

extern void Camera_TrackObject(lpLegoCamera cam, lpLegoObject liveObj, F32 trackZoomSpeed, F32 trackDist, F32 trackTilt, F32 trackRotationSpeed);

struct Lego_Level;
extern void Camera_Update(lpLegoCamera cam, struct Lego_Level* level, F32 elapsedAbs, F32 elapsedGame);

extern void Camera_Move(lpLegoCamera cam, Point3F *refDir, F32 elapsedAbs);

extern void Camera_StopMovement(lpLegoCamera cam);
