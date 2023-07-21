#include <stdio.h>
#include "lws.h"
#include "mem.h"
#include "file.h"
#include "error.h"
#include "utils.h"

Lws_Globs lwsGlobs = { NULL };

void Lws_Initialize(const char* sharedDir, B32 (*FindSFXID)(const char* name, SFX_ID* sfxID), S32 (*PlaySample3D)(lpContainer cont, SFX_ID type, B32 loop, B32 onCont, Point3F* wPos), B32 (*SoundEnabled)())
{
    if (sharedDir)
    {
        U16 len = strlen(sharedDir);
        if (sharedDir[len - 1] != '\\')
            len++;
        lwsGlobs.sharedDir = Mem_Alloc(len + 1);
        strcpy(lwsGlobs.sharedDir, sharedDir);
        lwsGlobs.sharedDir[len - 1] = '\\';
        lwsGlobs.sharedDir[len] = '\0';
    }

    lwsGlobs.FindSFXIDFunc = FindSFXID;
    lwsGlobs.PlaySample3DFunc = PlaySample3D;
    lwsGlobs.SoundEnabledFunc = SoundEnabled;

#ifdef _DEBUG
    lwsGlobs.initialized = TRUE;
#endif
}

lpLws_Info Lws_Parse(const char* fname, B32 looping)
{
    lpFile ifp;
    lpLws_Info scene = NULL;
    char path[FILE_MAXPATH];

#ifdef _DEBUG
    Error_Fatal(!lwsGlobs.initialized, "Lws_Initialize() has not been called");
#endif

    sprintf(path, "%s.%s", fname, LWS_FILESUFFIX);

    if (ifp = File_Open(path, "r"))
    {
        char line[LWS_MAXLINELEN];
        char lineSplit[LWS_MAXLINELEN];
        U16 length;

        File_GetLine(line, sizeof(line), ifp);
        length = strlen(line) + 1;
        File_GetLine(&line[length], sizeof(line) - length, ifp);

        if (strcmp(line, LWS_HEADER1) == 0 && strcmp(&line[length], LWS_HEADER2) == 0)
        {
            char* argv[LWS_MAXARGS];
            S32 argc, loop;
            S32 motionMode = 0, dissolveMode = 0;
            lpLws_Node currNode = NULL, parent;
            U16* parentList;
            char *s, *t;

            for (s = t = path; *s != '\0'; s++)
            {
                if (*s == '\\')
                    t = s;
            }

            *(t + (t == path ? 0 : 1)) = '\0';

            scene = Mem_Alloc(sizeof(Lws_Info));
            memset(scene, 0, sizeof(Lws_Info));

            if (path[0] != '\0')
                scene->filePath = Util_StrCpy(path);
            else
                scene->filePath = NULL;

            scene->nodeListSize = LWS_NODELISTBLOCKSIZE;
            scene->referenceCount = 1;
            if (looping)
                scene->flags |= LWS_FLAG_LOOPING;

            scene->nodeList = Mem_Alloc(sizeof(Lws_Node) * scene->nodeListSize);
            parentList = Mem_Alloc(sizeof(U16) * scene->nodeListSize);

            while (File_GetLine(line, sizeof(line), ifp))
            {
                strcpy(lineSplit, line);

                // TODO: Implement Lws_Parse
            }

            // TODO: Implement Lws_Parse

            Mem_Free(parentList);

            Lws_SetupSoundTriggers(scene);

        } else {
            Error_Fatal(TRUE, "Invalid scene file");
        }

        File_Close(ifp);
    }

    return scene;
}

void Lws_LoadMeshes(lpLws_Info scene, LPDIRECT3DRMFRAME3 parent)
{
    // TODO: Implement Lws_LoadMeshes
}

void Lws_SetTime(lpLws_Info scene, F32 time)
{
    // TODO: Implement Lws_SetTime
}

U32 Lws_GetFrameCount(lpLws_Info scene)
{
    return scene->lastFrame;
}

void Lws_SetupSoundTriggers(lpLws_Info scene)
{
    // TODO: Implement Lws_SetupSoundTriggers
}
