#pragma once

#include "platform.h"
#include "text_window.h"
#include "lego.h"

typedef enum Objective_GlobFlags
{
    OBJECTIVE_GLOB_FLAG_NONE                = 0,
    OBJECTIVE_GLOB_FLAG_BRIEFING            = 0x1,
    OBJECTIVE_GLOB_FLAG_COMPLETED           = 0x2,
    OBJECTIVE_GLOB_FLAG_FAILED              = 0x4,
    OBJECTIVE_GLOB_FLAG_STATUSREADY         = 0x8,
    OBJECTIVE_GLOB_FLAG_SHOWBRIEFINGADVISOR = 0x10,
    OBJECTIVE_GLOB_FLAG_SHOWACHIEVEDADVISOR = 0x20,
    OBJECTIVE_GLOB_FLAG_HITTIMEFAIL         = 0x40,
    OBJECTIVE_GLOB_FLAG_SHOWFAILEDADVISOR   = 0x80,
    OBJECTIVE_GLOB_FLAG_CRYSTAL             = 0x100,
    OBJECTIVE_GLOB_FLAG_ORE                 = 0x200,
    OBJECTIVE_GLOB_FLAG_BLOCK               = 0x400,
    OBJECTIVE_GLOB_FLAG_TIMER               = 0x800,
    OBJECTIVE_GLOB_FLAG_CONSTRUCTION        = 0x1000,
} Objective_GlobFlags;

typedef struct Objective_Globs
{
    Objective_GlobFlags flags;
    File* file; // PTRFileStream_00500bc4
    char filename[128]; // CHAR_ARRAY_00500bc8
    char* messages[4]; // [Briefing,Completion,Failure,CrystalFailure] Strings containing text of entire status message (pages are separated with '\a').
    U32 reserved1[384]; // (possibly unused array of char[3][128])
    U32 currentPages[4]; // (1-indexed) Current page number of the displayed status type. (g_Objective_StatusUnkCounts)
    U32 currentPageStates[4]; // (1-indexed) State tracking for page to switch to (this is only used to check if the above field needs to trigger an update). (g_Objective_StatusUnkCounts2)
    U32 pageCounts[4]; // Number of pages for the specific status. (g_Objective_StatusBellCounts)
    lpTextWindow textWindows[4]; // Text windows for the specific status. (g_Objective_StatusTextWindows)
    lpTextWindow pageTextWindows[3]; // PTRTextWindow_00500e18, PTRTextWindow_00500e1c, PTRTextWindow_00500e20
    lpTextWindow beginTextWindow; // Unknown usage, only worked with when line "[BEGIN]" is found (PTRTextWindow_00500e24)
    U32 reserved2;
    B32 hasBeginText; // True when text has been assigned to beginTextWindow (BOOL_00500e2c)
    B32 achieved; // True if the level was has ended successfully. (g_LevelIsComplete)
    B32 objectiveSwitch; // (see: NERPFunc__SetObjectiveSwitch)
    char* soundName;
    S32 soundHandle; // (init: -1 when unused) INT_00500e3c
    F32 soundTimer; // (init: (playTime - 1.5f) * 25.0f) FLOAT_00500e40
    B32 showing; // True when an objective is currently being shown or changed to(?)
    B32 endTeleportEnabled; // (cfg: ! DisableEndTeleport, default: false (enabled))
} Objective_Globs;

extern Objective_Globs objectiveGlobs;

extern void Objective_SetStatus(LevelStatus status);

extern void Objective_Update(lpTextWindow textWnd, lpLego_Level level, F32 elapsedGamed, F32 elapsedAbs);

extern B32 Objective_HandleKeys(B32 spaceKeyHeld, B32 lastLeftMouseButtonDown, B32 *outGotoNextLevel);

extern void Objective_ProgrammerModeGT3_FUN_00458ba0();

// Returns true if the level has ended.
extern B32 Objective_IsEnded();

extern void Objective_SetEndTeleportEnabled(B32 on);

extern void Objective_LoadLevel(lpConfig config, const char* gameName, const char* levelName, struct Lego_Level* level, U32 screenWidth, U32 screenHeight);
