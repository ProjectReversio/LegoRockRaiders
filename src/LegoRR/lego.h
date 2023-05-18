#pragma once

#include "types.h"
#include "config.h"
#include "main.h"
#include "maths.h"
#include "container.h"
#include "viewport.h"
#include "lego_camera.h"
#include "fonts.h"
#include "text_window.h"
#include "lego_object.h"

typedef struct Lego_Level
{
    void* temp;
    // TODO: Implement Lego_Level
} Lego_Level, *lpLego_Level;

typedef struct Lego_Globs
{
    lpConfig config;
    const char* gameName;
    U32 field_8;
    U32 field_c;
    Graphics_Quality quality;
    lpLego_Level currLevel;
    lpContainer rootCont;
    lpViewport viewMain;
    lpViewport viewTrack;
    lpLegoCamera cameraMain;
    lpLegoCamera cameraTrack;
    lpLegoCamera cameraFP;
    lpContainer spotlightTop;
    lpContainer spotlightTrack;
    lpContainer pointLightFP;
    lpContainer dirLightFP;
    lpContainer ambientLight;
    lpContainer rootSpotlight;
    lpContainer rootLight;
    lpContainer dirLightCallToArms;
    F32 FPClipBlocks;
    F32 TVClipDist;
    U32 field_58;
    U32 field_5c;
    U32 field_60;
    lpFont bmpFONT5_HI;
    lpFont bmpToolTipFont;
    lpFont bmpDesktopFont;
    lpFont bmpfont5_HI;
    lpFont bmpMbriefFONT;
    lpFont bmpMbriefFONT2;
    lpFont bmpRSFont;
    lpTextWindow textWnd_80;
    lpTextWindow textWnd_84;
    Point2F gotoNewPos;
    Point2F gotoTargetPos;
    B32 gotoSmooth;
    F32 gameSpeed;
    lpLegoObject objectFP;

    // TODO: Implement Lego_Globs

} Lego_Globs, *lpLego_Globs;

extern Lego_Globs legoGlobs;

extern B32 Gods_Go(const char* programName);

extern B32 Lego_Initialize();
extern B32 Lego_Shutdown_Full();
extern B32 Lego_Shutdown_Quick();
extern B32 Lego_MainLoop(F32 time);
