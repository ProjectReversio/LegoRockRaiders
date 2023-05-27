#include "loader.h"
#include "file.h"
#include "utils.h"
#include "mem.h"
#include "main.h"

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
    if (section == NULL)
    {
        loaderGlobs.flags &= ~LOADER_GLOB_FLAG_ENABLED;
        return;
    }

    // TODO: Cleanup this decompile and avoid goto statements
    lpLoaderSection loaderSec = NULL;
    S32 it = 0;
    lpLoaderSection secList = loaderGlobs.sectionList;
    while(!secList->name)
    {
        loaderSec = secList;
        LABEL_7:
        secList++;
        it++;
        if (secList >= &loaderGlobs.current)
            goto LABEL_10;
    }

    if (_strcmpi(secList->name, section))
        goto LABEL_7;

    loaderGlobs.current = &loaderGlobs.sectionList[it];
    loaderGlobs.current->currentSize = 0;

    LABEL_10:
    if (it == 50)
    {
        if (loaderSec)
        {
            char* secName = Mem_Alloc(strlen(section) + 1);
            strcpy(secName, section);
            loaderSec->name = secName;
            loaderSec->currentSize = 0;
            loaderSec->totalSize = 0;
            loaderGlobs.current = loaderSec;
        }
    }
    loaderGlobs.progressLast = -1.0f;
    loaderGlobs.flags |= LOADER_GLOB_FLAG_ENABLED;
    Loader_FileLoadCallback(NULL, 0, NULL);
}

void Loader_FileLoadCallback(const char* filename, U32 fileSize, void* data)
{
    if ((loaderGlobs.flags & LOADER_GLOB_FLAG_ENABLED) == 0)
        return;

    F32 progress = 0.0f;

    if (loaderGlobs.current && (loaderGlobs.current->currentSize += fileSize, loaderGlobs.current) && loaderGlobs.current->totalSize != 0)
    {
        progress = ((F32)loaderGlobs.current->currentSize / (F32)loaderGlobs.current->totalSize);
    } else {
        progress = 0;
    }

    //progress = 0.75f;

    if (progress <= (loaderGlobs.progressLast - 0.002f))
        return;

    Point2F barPos;
    barPos.x = loaderGlobs.progressWindow.x;
    barPos.y = loaderGlobs.progressWindow.y;

    Size2F barSize;
    barSize.width = loaderGlobs.progressWindow.width;
    barSize.height = loaderGlobs.progressWindow.height;

    Image_Display(loaderGlobs.loadScreen, NULL);

    if (progress > 1.0f)
        progress = 1.0f;

    switch (loaderGlobs.progressDirection)
    {
        case DIRECTION_UP:
            barPos.y = (barSize.height + barPos.y) - barSize.height * (F32)progress;
            barSize.height = (barSize.height + barPos.y) - barPos.y;
            break;
        case DIRECTION_RIGHT:
            barSize.width *= (F32)progress;
            break;
        case DIRECTION_DOWN:
            barSize.height *= (F32)progress;
            break;
        case DIRECTION_LEFT:
            barPos.x = (barSize.width + barPos.x) - barSize.width * (F32)progress;
            barSize.width = (barSize.width + barPos.x) - barPos.x;
            break;
    }

    if (loaderGlobs.progressBar)
        Image_DisplayScaled(loaderGlobs.progressBar, NULL, &barPos, &barSize);

    if (loaderGlobs.loadingText)
    {
#ifdef _DEBUG
        U32 progressNum = (U32)(progress * 100.0f);
        if (filename)
        {
            Font_PrintF(loaderGlobs.font, (loaderGlobs.progressWindow.width * 0.5f + loaderGlobs.progressWindow.x) - (loaderGlobs.loadingWidth >> 1),
                        loaderGlobs.progressWindow.y - 1,
                        "%s[%d%%]: %s",
                        loaderGlobs.loadingText, progressNum, filename);
        } else {
            Font_PrintF(loaderGlobs.font, (loaderGlobs.progressWindow.width * 0.5f + loaderGlobs.progressWindow.x) - (loaderGlobs.loadingWidth >> 1),
                        loaderGlobs.progressWindow.y - 1,
                        "%s[%d%%]",
                        loaderGlobs.loadingText, progressNum);
        }
#else
        Font_PrintF(loaderGlobs.font, (loaderGlobs.progressWindow.width * 0.5f + loaderGlobs.progressWindow.x) - (loaderGlobs.loadingWidth >> 1),
                    loaderGlobs.progressWindow.y - 1,
                    "%s",
                    loaderGlobs.loadingText);
#endif
    }

    Main_LoopUpdate(FALSE);
    loaderGlobs.progressLast = progress;
}
