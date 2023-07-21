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

                if ((argc = Util_WSTokenize(lineSplit, argv)))
                {
                    if (currNode == NULL)
                    {
                        if (strcmp("FirstFrame", argv[0]) == 0)
                            scene->firstFrame = atoi(argv[1]);

                        if (strcmp("LastFrame", argv[0]) == 0)
                            scene->lastFrame = atoi(argv[1]);

                        if (scene->lastFrame == 0)
                            scene->lastFrame = 1;

                        if (strcmp("FramesPerSecond", argv[0]) == 0)
                            scene->fps = (F32)atof(argv[1]);

                        if (strcmp("AddNullObject", argv[0]) == 0 || strcmp("LoadObject", argv[0]) == 0)
                        {
                            if (scene->nodeCount == scene->nodeListSize)
                            {
                                scene->nodeListSize += LWS_NODELISTBLOCKSIZE;
                                scene->nodeList = Mem_ReAlloc(scene->nodeList, sizeof(Lws_Node) * scene->nodeListSize);
                                parentList = Mem_ReAlloc(parentList, sizeof(U16) * scene->nodeListSize);
                            }

                            parentList[scene->nodeCount] = 0;
                            currNode = &scene->nodeList[scene->nodeCount];
                            memset(currNode, 0, sizeof(Lws_Node));

                            if (strcmp("AddNullObject", argv[0]) == 0)
                            {
                                currNode->flags |= LWSNODE_FLAG_NULL;
                                currNode->name = Util_StrCpy(&line[strlen("AddNullObject") + 1]);
                                if (strnicmp(currNode->name, LWS_SOUNDTRIGGERPREFIX, strlen(LWS_SOUNDTRIGGERPREFIX)) == 0 &&
                                    (strnicmp(&currNode->name[strlen(LWS_SOUNDTRIGGERPREFIX)], LWS_SOUNDTRIGGERSEPERATOR, strlen(LWS_SOUNDTRIGGERSEPERATOR)) == 0))
                                {
                                    currNode->flags |= LWSNODE_FLAG_SOUNDTRIGGER;
                                    Error_Fatal(scene->triggerCount == 256, "Too many sound trigger frames");
                                    scene->triggerCount++;
                                }
                            } else
                            {
                                char* lwoStr;
                                strcpy(lineSplit, line);
                                argc = Util_Tokenize(line, argv, "\\");
                                if ((lwoStr = Util_StrIStr(argv[argc - 1], ".lwo")))
                                    lwoStr[0] = '\0';
                                currNode->name = Util_StrCpy(argv[argc - 1]);
                            }

                            for (loop = scene->nodeCount - 1; loop >= 0; loop--)
                            {
                                if (((scene->nodeList[loop].flags & LWSNODE_FLAG_NULL) && (currNode->flags & LWSNODE_FLAG_NULL)) ||
                                    (!(scene->nodeList[loop].flags & LWSNODE_FLAG_NULL) && !(currNode->flags & LWSNODE_FLAG_NULL)))
                                {
                                    if (strcmp(currNode->name, scene->nodeList[loop].name) == 0)
                                    {
                                        currNode->reference = scene->nodeList[loop].reference + 1;
                                        break;
                                    }
                                }
                            }
                        }
                    } else if (motionMode)
                    {
                        // argv[1] is the first item as there is some white space at the start of the line...

                        if (motionMode == 1)
                        {
                            Error_Fatal(atoi(argv[1]) != 9, "Scene file error");
                        }
                        else if (motionMode == 2)
                        {
                            currNode->keyCount = atoi(argv[1]);
                            currNode->keyList = Mem_Alloc(sizeof(Lws_KeyInfo) * (currNode->keyCount + 1));
                        } else
                        {
                            U16 key = (motionMode - 3) / 2;
                            if (key < currNode->keyCount)
                            {
                                if (motionMode % 2)
                                {
                                    currNode->keyList[key].position.x = (F32) atof(argv[1]);
                                    currNode->keyList[key].position.y = (F32) atof(argv[2]);
                                    currNode->keyList[key].position.z = (F32) atof(argv[3]);
                                    currNode->keyList[key].hpb.x = ((F32) atof(argv[4]) / 180.0f) * M_PI;
                                    currNode->keyList[key].hpb.y = ((F32) atof(argv[5]) / 180.0f) * M_PI;
                                    currNode->keyList[key].hpb.z = ((F32) atof(argv[6]) / 180.0f) * M_PI;
                                    currNode->keyList[key].scale.x = (F32) atof(argv[7]);
                                    currNode->keyList[key].scale.y = (F32) atof(argv[8]);
                                    currNode->keyList[key].scale.z = (F32) atof(argv[9]);
                                } else
                                {
                                    currNode->keyList[key].frame = atoi(argv[1]);
                                }
                            }
                        }

                        motionMode++;

                        if (strcmp(argv[0], "EndBehavior") == 0)
                            motionMode = 0;
                    } else if (dissolveMode)
                    {
                        // argv[1] is the first item as there is some white space at the start of the line...

                        if (dissolveMode == 1)
                        {
                            Error_Fatal(atoi(argv[1]) != 1, "Scene file error");
                        } else if (dissolveMode == 2)
                        {
                            currNode->dissolveCount = atoi(argv[1]);
                            currNode->dissolveLevel = Mem_Alloc(sizeof(F32) * (currNode->dissolveCount + 1));
                            currNode->dissolveFrame = Mem_Alloc(sizeof(U16) * (currNode->dissolveCount + 1));
                        } else
                        {
                            U16 key = (dissolveMode - 3) / 2;
                            if (key < currNode->dissolveCount)
                            {
                                if (dissolveMode % 2)
                                {
                                    currNode->dissolveLevel[key] = (F32) atof(argv[1]);
                                    if (currNode->dissolveLevel[key] < 0.0f)
                                        currNode->dissolveLevel[key] = 0.0f;
                                    if (currNode->dissolveLevel[key] > 1.0f)
                                        currNode->dissolveLevel[key] = 1.0f;
                                    currNode->dissolveLevel[key] = 1.0f - currNode->dissolveLevel[key];
                                } else
                                {
                                    currNode->dissolveFrame[key] = atoi(argv[1]);
                                }
                            }
                        }

                        dissolveMode++;

                        if (strcmp(argv[0], "EndBehavior") == 0)
                            dissolveMode = 0;
                    } else
                    {
                        if (strcmp(argv[0], "ObjectMotion") == 0)
                        {
                            motionMode = 1;
                        }
                        else if (strcmp(argv[0], "ObjDissolve") == 0)
                        {
                            if (strcmp(argv[1], "(envelope)") == 0)
                            {
                                dissolveMode = 1;
                            } else {
                                Error_Fatal(lwsGlobs.staticDissolveCount == LWS_MAXSTATICDISSOLVES, "LWS_MAXSTATICDISSOLVES too small");
                                lwsGlobs.staticDissolveLevel[lwsGlobs.staticDissolveCount] = (F32) atof(argv[1]);
                                currNode->dissolveLevel = &lwsGlobs.staticDissolveLevel[lwsGlobs.staticDissolveCount];
                                lwsGlobs.staticDissolveCount++;
                                currNode->dissolveLevel = 0;
                            }
                        }
                        else if (strcmp(argv[0], "ParentObject") == 0)
                        {
                            parentList[scene->nodeCount] = atoi(argv[1]);
                        }
                        else if (strcmp(argv[0], "PivotPoint") == 0)
                        {
                            currNode->pivotVector.x = (F32) -atof(argv[1]);
                            currNode->pivotVector.y = (F32) -atof(argv[2]);
                            currNode->pivotVector.z = (F32) -atof(argv[3]);
                        }
                        else if (strcmp(argv[0], "UnseenByCamera") == 0 && atoi(argv[1]) == 1)
                        {
                            currNode->flags |= LWSNODE_FLAG_FACECAMERA;
                        } else if (strcmp(argv[0], "ShadowOptions") == 0)
                        {
                            currNode = NULL;
                            scene->nodeCount++;
                        }
                    }
                }
            }

            // If there is more than one key and the last one is before the end of the animation
            // then create a duplicate of the last one at the end.
            // The arrays are over allocated by one to ensure that there is enough space.

            for (loop = 0; loop < (S32)scene->nodeCount; loop++)
            {
                currNode = &scene->nodeList[loop];
                if (currNode->keyCount != 1 && currNode->keyList[currNode->keyCount - 1].frame < scene->lastFrame)
                {
                    currNode->keyList[currNode->keyCount].frame = scene->lastFrame;
                    currNode->keyList[currNode->keyCount].position = currNode->keyList[currNode->keyCount - 1].position;
                    currNode->keyList[currNode->keyCount].hpb = currNode->keyList[currNode->keyCount - 1].hpb;
                    currNode->keyList[currNode->keyCount].scale = currNode->keyList[currNode->keyCount - 1].scale;
                    currNode->keyCount++;
                }
            }

            // Do the same for the dissolve level (assume that if an envelope is defined, more than one key
            // will be used)...

            for (loop = 0; loop < (S32)scene->nodeCount; loop++)
            {
                currNode = &scene->nodeList[loop];
                if (currNode->dissolveCount)
                {
                    if (currNode->dissolveFrame[currNode->dissolveCount - 1] < scene->lastFrame)
                    {
                        currNode->dissolveFrame[currNode->dissolveCount] = scene->lastFrame;
                        currNode->dissolveLevel[currNode->dissolveCount] = currNode->dissolveLevel[currNode->dissolveCount - 1];
                        currNode->dissolveCount++;
                    }
                }
            }

            // Link the hierarchy from the node and parent lists...
            // New links are added to the front of the list, so reverse through the array to preserve the order...

            for (loop = scene->nodeCount - 1; loop >= 0; loop--)
            {
                currNode = &scene->nodeList[loop];
                if (parentList[loop])
                {
                    parent = &scene->nodeList[parentList[loop] - 1];
                    currNode->next = parent->childList;
                    parent->childList = currNode;
                } else
                {
                    currNode->next = scene->masterNode;
                    scene->masterNode = currNode;
                }
            }

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
    scene->triggerCount = 0;
}
