#pragma once

#include "types.h"
#include "common.h"
#include "maths.h"
#include "fonts.h"
#include "images.h"

#define LOADER_MAX_SECTIONS 50

typedef enum Loader_GlobFlags
{
    LOADER_GLOB_FLAG_NONE,
    LOADER_GLOB_FLAG_ENABLED
} Loader_GlobFlags;

typedef struct LoaderSection
{
    const char* name; // Name of the section files are being loaded from
    U32 currentSize; // Current total size of files loaded for this section
    U32 totalSize; // Predefined total "expected" size for files to load from this section
} LoaderSection, *lpLoaderSection;

typedef struct Loader_Globs
{
    lpImage loadScreen;
    lpFont font;
    LoaderSection sectionList[LOADER_MAX_SECTIONS];
    lpLoaderSection current; // current section being loaded
    lpImage shutdownScreen;
    lpImage progressBar;
    Area2F progressWindow;
    Direction progressDirection; // expand direction of progress bar: U, R, D, L
    const char* loadingText;
    U32 loadingWidth; // measured width of font with loadingText
    F32 progressLast; // percentage of filesize for section (stores percent of last render)
    Loader_GlobFlags flags;

} Loader_Globs, *lpLoader_Globs;

extern Loader_Globs loaderGlobs;

extern void Loader_Initialize(const char* loadScreenName, const char* shutdownScreenName, lpFont font5HI, const char* profileName, Direction progressDirection, const char* progressBarName, Area2F* progressWindow, const char* loadingText);
extern void Loader_Display_Loading_Bar(const char* section);

extern void Loader_FileLoadCallback(const char* filename, U32 fileSize, void* data);
