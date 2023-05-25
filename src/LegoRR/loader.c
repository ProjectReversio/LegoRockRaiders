#include "loader.h"
#include "file.h"
#include "utils.h"

Loader_Globs loaderGlobs = { NULL };

void Loader_Initialize(const char* loadScreenName, const char* shutdownScreenName, lpFont font5HI, const char* profileName, Direction progressDirection, const char* progressBarName, Area2F* progressWindow, const char* loadingText)
{
    memset(&loaderGlobs, 0, sizeof(loaderGlobs));

    loaderGlobs.loadScreen = Image_LoadBMP(loadScreenName);
    loaderGlobs.shutdownScreen = Image_LoadBMP(shutdownScreenName);
    loaderGlobs.font = font5HI;
    loaderGlobs.current = 0;
    loaderGlobs.progressBar = Image_LoadBMP(progressBarName);
    loaderGlobs.progressWindow = *progressWindow;
    loaderGlobs.progressDirection = progressDirection;
    if (loadingText)
    {
        char* loadTxt = Util_StrCpy(loadingText);
        loaderGlobs.loadingText = loadTxt;
        for (char i = *loadTxt; i; loadTxt++)
        {
            if (i == '_')
                *loadTxt = ' ';
            i = loadTxt[1];
        }
        Font_GetStringInfo(font5HI, &loaderGlobs.loadingWidth, 0, "%s", loadingText);
    } else {
        loaderGlobs.loadingText = NULL;
    }

    lpFile profileFile = NULL;
    if (profileName)
        profileFile = File_Open(profileName, "r");

    LoaderSection* section = loaderGlobs.sectionList;
    char buffer[256];
    char* stringParts[10];
    do
    {
        if (profileFile && File_GetS(buffer, 256, profileFile) && Util_Tokenize(buffer, stringParts, "=") == 2)
        {
            char* sectionName = malloc(strlen(stringParts[0]) + 1);
            strcpy(sectionName, stringParts[0]);
            section->name = sectionName;
            section->totalSize = atoi(stringParts[1]);
            section->currentSize = 0;
        } else {
            section->name = NULL;
        }
        section++;
    } while (section < &loaderGlobs.current);

    if (profileFile)
        File_Close(profileFile);
    File_SetLoadCallback(Loader_FileLoadCallback, 0);
}

void Loader_Display_Loading_Bar(const char* section)
{
    // TODO: Implement Loader_Display_Loading_Bar
}

void Loader_FileLoadCallback(const char* filename, U32 fileSize, void* data)
{
    // TODO: Implement Loader_FileLoadCallback
}
