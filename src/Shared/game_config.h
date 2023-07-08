#pragma once

// Define me to fix memory leak bugs from the original.
#define LEGORR_FIX_MEMORY_LEAKS

// Define me to fix the bug from the original game where you can't exit while in menus or cutscenes.
#define LEGORR_FIX_CLOSE_DOESNT_EXIT

// Define me to center window on launch.
#define LEGORR_CENTER_WINDOW

// Define me to make windowed mode the default.
//#define LEGORR_WINDOWED_DEFAULT

// Define me to force dev mode on.
//#define LEGORR_FORCE_DEV_MODE

// Define me to force display of the version number
//#define LEGORR_FORCE_SHOW_VERSION

// Define me to log file access.
//#define LEGORR_LOG_FILE_ACCESS

// Define me to force a specific program name globally.
//#define LEGORR_FORCE_PROGRAM_NAME_GLOBAL

// Define me to force a specific program name only for wad files.
//#define LEGORR_FORCE_PROGRAM_NAME_WAD_ONLY

// Define the program name to use when forcing a specific program name.
#define LEGORR_FORCED_PROGRAM_NAME "LegoRR"

// Define me to fix the console output in CLion.
//#define LEGORR_FIX_CLION_CONSOLE_OUTPUT

// Define me to display debug info in the loader
//#define LEGORR_DEBUG_LOADER

#ifdef _DEBUG
    // Defaults for Debug Mode
    #define LEGORR_WINDOWED_DEFAULT // For convenience, default to windowed mode in debug builds
    #define LEGORR_FORCE_DEV_MODE

    #define LEGORR_FORCE_SHOW_VERSION

    // Since we currently have a different name for the debug executable,
    // we need to specify LegoRR for the program name in order to load the right WAD files.
    #define LEGORR_FORCE_PROGRAM_NAME_WAD_ONLY

    #define LEGORR_FIX_CLION_CONSOLE_OUTPUT

    //#define LEGORR_DEBUG_LOADER

#else
    // Defaults for Release Mode

#endif