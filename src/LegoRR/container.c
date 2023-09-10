#include <stdio.h>
#include "container.h"
#include "error.h"
#include "activities.h"
#include "main.h"
#include "mem.h"
#include "config.h"
#include "utils.h"
#include "lws.h"
#include "anim_clone.h"
#include "bmp.h"
#include "dxbug.h"

extern Container_Globs containerGlobs = { NULL };

#define CONTAINER_NULLSTRING			"NULL"
#define CONTAINER_MESHSTRING			"MESH"
#define CONTAINER_MESHNOTEXTURESTRING	"MESH:NOTEXTURE"
#define CONTAINER_FRAMESTRING			"FRAME"
#define CONTAINER_ANIMSTRING			"ANIM"
#define CONTAINER_LWSSTRING				"LWS"
#define CONTAINER_LWOSTRING				"LWO"
#define CONTAINER_LWONOTEXTURESTRING	"LWO:NOTEXTURE"
#define CONTAINER_ACTIVITYSTRING		"ACT"

lpContainer Container_Initialize(const char* gameName)
{
    lpContainer root;

    if (containerGlobs.flags & CONTAINER_GLOB_FLAG_INITIALIZED)
        Error_Fatal(TRUE, "Containers already initialized");

    memset(&containerGlobs, 0, sizeof(containerGlobs));

    for (U32 loop = 0; loop < CONTAINER_MAXLISTS; loop++)
        containerGlobs.listSet[loop] = NULL;

    containerGlobs.typeName[Container_Null] = CONTAINER_NULLSTRING;
    containerGlobs.typeName[Container_Mesh] = CONTAINER_MESHSTRING;
    containerGlobs.typeName[Container_Frame] = CONTAINER_FRAMESTRING;
    containerGlobs.typeName[Container_Anim] = CONTAINER_ANIMSTRING;
    containerGlobs.typeName[Container_LWS] = CONTAINER_LWSSTRING;
    containerGlobs.typeName[Container_LWO] = CONTAINER_LWOSTRING;
    containerGlobs.typeName[Container_FromActivity] = CONTAINER_ACTIVITYSTRING;
    containerGlobs.typeName[Container_Light] = NULL;

    containerGlobs.extensionNames[Container_Null] = "";
    containerGlobs.extensionNames[Container_Mesh] = "x";
    containerGlobs.extensionNames[Container_Frame] = "x";
    containerGlobs.extensionNames[Container_Anim] = "x";
    containerGlobs.extensionNames[Container_FromActivity] = ACTIVITY_FILESUFFIX;
    containerGlobs.extensionNames[Container_Light] = "";

    containerGlobs.gameName = gameName;
    containerGlobs.freeList = NULL;
    containerGlobs.listCount = 0;
    containerGlobs.flags = CONTAINER_GLOB_FLAG_INITIALIZED;

    containerGlobs.textureCount = 0;
    containerGlobs.sharedDir = NULL;

    containerGlobs.rootContainer = root = Container_Create(NULL);
    Error_Fatal(root == NULL, "Could not create root container");

    root->masterFrame->lpVtbl->SetSortMode(root->masterFrame, D3DRMSORT_NONE);

    return root;
}

void Container_Shutdown()
{
    // TODO: Implement Container_Shutdown
}

lpContainer Container_Create(lpContainer parent)
{
    lpContainer newContainer;
    LPDIRECT3DRMFRAME3 parentFrame = NULL, hiddenParentFrame = NULL;

    Container_DebugCheckOK(CONTAINER_DEBUG_NOTREQUIRED);

    if (parent)
    {
        parentFrame = parent->masterFrame;
        hiddenParentFrame = parent->hiddenFrame;
    }

    if (containerGlobs.freeList == NULL)
        Container_AddList();

    newContainer = containerGlobs.freeList;
    containerGlobs.freeList = newContainer->nextFree;
    memset(newContainer, 0, sizeof(Container));
    newContainer->nextFree = newContainer;

    if (lpD3DRM()->lpVtbl->CreateFrame(lpD3DRM(), parentFrame, &newContainer->masterFrame) == D3DRM_OK)
    {
        Container_NoteCreation(newContainer->masterFrame);
        if (lpD3DRM()->lpVtbl->CreateFrame(lpD3DRM(), newContainer->masterFrame, &newContainer->activityFrame) == D3DRM_OK)
        {
            Container_NoteCreation(newContainer->activityFrame);
            if (lpD3DRM()->lpVtbl->CreateFrame(lpD3DRM(), hiddenParentFrame, &newContainer->hiddenFrame) == D3DRM_OK)
            {
                Container_NoteCreation(newContainer->hiddenFrame);

                // These frames should all appear as the same part of the heirarchy
                // so set them up as all having newContainer as being their owner...
                // Getting the children of the Container will be tricky...

                Container_Frame_SetAppData(newContainer->masterFrame, newContainer, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL);
                Container_Frame_SetAppData(newContainer->activityFrame, newContainer, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL);
                Container_Frame_SetAppData(newContainer->hiddenFrame, newContainer, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL);

                return newContainer;
            }
        }
    }

    Error_Fatal(TRUE, "Unable to Create Container Frames");
    Container_Remove(newContainer);
    return NULL;
}

void Container_AddList()
{
    lpContainer list;
    U32 loop, count;

    Container_DebugCheckOK(CONTAINER_DEBUG_NOTREQUIRED);

    Error_Fatal(containerGlobs.listCount + 1 >= CONTAINER_MAXLISTS, "Run out of lists");

    count = 0x00000001 << containerGlobs.listCount;

    list = containerGlobs.listSet[containerGlobs.listCount] = Mem_Alloc(sizeof(Container) * count);
    if (list)
    {
        containerGlobs.listCount++;

        for (loop = 1; loop < count; loop++)
        {
            list[loop - 1].nextFree = &list[loop];
        }
        list[count - 1].nextFree = containerGlobs.freeList;
        containerGlobs.freeList = list;
    } else {
        Error_Fatal(TRUE, Error_Format("Unable to allocate %d bytes of memory for new list.\n", sizeof(Container) * count));
    }
}

void Container_Remove(lpContainer dead)
{
    Container_Remove2(dead, FALSE);
}

void Container_Remove2(lpContainer dead, B32 kill)
{
    // TODO: Implement Container_Remove2
}

void Container_SetTypeData(lpContainer cont, const char* name, LPDIRECT3DRMLIGHT light, LPDIRECT3DRMMESH mesh, struct Mesh* transMesh)
{
    Container_DebugCheckOK(cont);

    if (cont->typeData)
    {
        if (name != NULL)
            cont->typeData->name = name;
        if (light != NULL)
            cont->typeData->light = light;
        if (mesh != NULL)
            cont->typeData->mesh = mesh;
        if (transMesh != NULL)
            cont->typeData->transMesh = transMesh;
    }
    else
    {
        cont->typeData = Mem_Alloc(sizeof(Container_TypeData));
        cont->typeData->name = name;
        cont->typeData->light = light;
        cont->typeData->mesh = mesh;
        cont->typeData->transMesh = transMesh;
    }
}

void Container_Frame_SetAppData(LPDIRECT3DRMFRAME3 frame, lpContainer owner, lpAnimClone animClone, const char* asfname, U32* frameCount, const char* frameName, F32* currTime, F32* transCo, const char* actSample, void* soundRecord, U32* trigger)
{
    Container_AppData *appData;

    if ((appData = (Container_AppData*)frame->lpVtbl->GetAppData(frame)))
    {
        if (owner != NULL)
            appData->ownerContainer = owner;

        if (animClone != NULL)
            appData->animClone = animClone;

        if (asfname != NULL && appData->animSetFileName != NULL)
            Mem_Free(appData->animSetFileName);

        if (frameCount != NULL)
            appData->frameCount = *frameCount;

        if (frameName != NULL)
            appData->frameName = frameName;

        if (currTime != NULL)
            appData->currTime = *currTime;

        if (transCo != NULL)
            appData->transCo = *transCo;

        if (actSample != NULL)
            appData->activitySample = actSample;

        if (trigger != NULL)
            appData->trigger = *trigger;
    } else {
        appData = Mem_Alloc(sizeof(Container_AppData));
        appData->ownerContainer = owner;
        appData->animClone = animClone;
        appData->animSetFileName = NULL;
        if (frameCount)
            appData->frameCount = *frameCount;
        else
            appData->frameCount = 0;

        appData->frameName = frameName;

        if (currTime)
            appData->currTime = *currTime;

        if (transCo)
            appData->transCo = *transCo;
        else
            appData->transCo = 0.0f;

        if (trigger)
            appData->trigger = *trigger;
        else
            appData->trigger = 0;

        appData->activitySample = actSample;

        frame->lpVtbl->SetAppData(frame, (DWORD)appData);
    }

    if (asfname != NULL)
    {
        appData->animSetFileName = Mem_Alloc(strlen(asfname) + 1);
        strcpy(appData->animSetFileName, asfname);
    }
}

void Container_SetSharedTextureDirectory(const char* path)
{
    containerGlobs.sharedDir = Util_StrCpy(path);
}

void Container_SetTriggerFrameCallback(void (*Callback)(lpContainer cont, void* data), void* data)
{
    containerGlobs.triggerFrameCallback = Callback;
    containerGlobs.triggerFrameData;
}

void Container_GetFrames(lpContainer cont, lpContainer ref, LPDIRECT3DRMFRAME3 *contFrame, LPDIRECT3DRMFRAME3 *refFrame)
{
    Container_DebugCheckOK(cont);

    *contFrame = cont->masterFrame;

    Error_Fatal(!(*contFrame), "Container has no masterFrame");

    if (ref == NULL)
        *refFrame = containerGlobs.rootContainer->masterFrame;
    else
        *refFrame = ref->masterFrame;
}

void Container_SetPosition(lpContainer cont, lpContainer ref, F32 x, F32 y, F32 z)
{
    LPDIRECT3DRMFRAME3  refFrame, frame;
    Container_GetFrames(cont, ref, &frame, &refFrame);
    frame->lpVtbl->SetPosition(frame, refFrame, x, y, z);
}

void Container_SetOrientation(lpContainer cont, lpContainer ref, F32 dirx, F32 diry, F32 dirz, F32 upx, F32 upy, F32 upz)
{
    LPDIRECT3DRMFRAME3  refFrame, frame;
    Container_GetFrames(cont, ref, &frame, &refFrame);
    frame->lpVtbl->SetOrientation(frame, refFrame, dirx, diry, dirz, upx, upy, upz);
}

void Container_SetParent(lpContainer child, lpContainer parent)
{
    // Pass NULL as the parent to unhook the Container...

    LPDIRECT3DRMFRAME3 parentFrame, childFrame, childFrameHidden;
    Container_DebugCheckOK(child);

    childFrame = child->masterFrame;
    childFrameHidden = child->hiddenFrame;
    Error_Fatal(!childFrame || !childFrameHidden, "Child has no master/hiddenFrame");

    if (parent == NULL)
    {
        // Delete the Child from its existing parent...
        childFrame->lpVtbl->GetParent(childFrame, &parentFrame);

        if (parentFrame)
        {
            parentFrame->lpVtbl->DeleteChild(parentFrame, childFrame);
            parentFrame->lpVtbl->Release(parentFrame);
        }

        // Do the same with the hidden frame...
        childFrame->lpVtbl->GetParent(childFrameHidden, &parentFrame);

        if (parentFrame)
        {
            parentFrame->lpVtbl->DeleteChild(parentFrame, childFrameHidden);
            parentFrame->lpVtbl->Release(parentFrame);
        }
    } else
    {
        parentFrame = parent->masterFrame;
        Error_Fatal(!parentFrame, "Parent has no masterFrame");

        Container_Frame_SafeAddChild(parentFrame, childFrame);

        // Do the same with the hidden frame...
        parentFrame = parent->hiddenFrame;
        Container_Frame_SafeAddChild(parentFrame, childFrameHidden);
    }
}

void Container_Frame_SafeAddChild(LPDIRECT3DRMFRAME3 parent, LPDIRECT3DRMFRAME3 child)
{
    // Stop addchild from corrupting the transformation matrix...
    D3DRMMATRIX4D mat;
    LPDIRECT3DRMFRAME3 oldParent;

    child->lpVtbl->GetParent(child, &oldParent);
    child->lpVtbl->GetTransform(child, oldParent, mat);
    if (oldParent)
        oldParent->lpVtbl->Release(oldParent);
    parent->lpVtbl->AddChild(parent, child);
    child->lpVtbl->AddTransform(child, D3DRMCOMBINE_REPLACE, mat);
}

void Container_GetPosition(lpContainer cont, lpContainer ref, lpPoint3F pos)
{
    LPDIRECT3DRMFRAME3 refFrame, frame;
    Container_GetFrames(cont, ref, &frame, &refFrame);
    frame->lpVtbl->GetPosition(frame, refFrame, (LPD3DVECTOR) pos);
}

void Container_GetOrientation(lpContainer cont, lpContainer ref, lpPoint3F dir, lpPoint3F up)
{
    LPDIRECT3DRMFRAME3 refFrame, frame;
    Point3F vdir, vup;

    Container_GetFrames(cont, ref, &frame, &refFrame);
    frame->lpVtbl->GetOrientation(frame, refFrame, (LPD3DVECTOR) &vdir, (LPD3DVECTOR) &vup);
    if (dir)
        *dir = vdir;
    if (up)
        *up = vup;
}

F32 Container_GetAnimationTime(lpContainer cont)
{
    LPDIRECT3DRMFRAME3 frame;
    char* currAnimName;
    F32 time = 0.0f;

    Container_DebugCheckOK(cont);

    if (cont->type == Container_FromActivity)
    {
        // TODO: Implement Container_GetAnimationTime
    }
    else if (cont->type == Container_Anim)
    {
        time = Container_Frame_GetCurrTime(cont->activityFrame);
    }

    return time;
}

F32 Container_SetAnimationTime(lpContainer cont, F32 time)
{
    LPDIRECT3DRMFRAME3 frame;
    char* currAnimName;
    U32 frameCount;
    lpAnimClone animClone = NULL;
    F32 overrun = 0.0f;

    Container_DebugCheckOK(cont);

    if (cont->type == Container_FromActivity)
    {
        // TODO: Implement Container_SetAnimationTime
    }
    else if (cont->type == Container_Anim)
    {
        animClone = Container_Frame_GetAnimClone(cont->activityFrame);
        frame = cont->activityFrame;
    }

    if (animClone)
    {
        B32 skipSetTime = (cont->flags & CONTAINER_FLAG_HIDDEN);
        F32 oldTime = Container_GetAnimationTime(cont);

        // If the container is hidden then don't bother updating the animation
        // unless the animation has ended...

        frameCount = Container_Frame_GetFrameCount(frame);
        Container_Frame_SetAppData(frame, NULL, NULL, NULL, NULL, NULL, &time, NULL, NULL, NULL, NULL);
        if (frameCount)
        {
            frameCount--;
            if (time > frameCount)
            {
                overrun = (time - frameCount);
                skipSetTime = FALSE;
            }

            if (skipSetTime == FALSE)
            {
                F32 triggerFrame = (F32) Container_Frame_GetTrigger(frame);
                //#pragma message("Not restoring the time on original animation set")
                if (frameCount != 1)
                {
                    AnimClone_SetTime(animClone, time, NULL);
                    if (containerGlobs.triggerFrameCallback && triggerFrame)
                    {
                        if (oldTime < triggerFrame && time >= triggerFrame)
                            containerGlobs.triggerFrameCallback(cont, containerGlobs.triggerFrameData);
                    }
                }
            }
            else
            {
                cont->flags |= CONTAINER_FLAG_ANIMATIONSKIPPED;
            }
        }
    }

    return overrun;
}

U32 Container_GetAnimationFrames(lpContainer cont)
{
    LPDIRECT3DRMFRAME3 frame = NULL;
    char* currAnimName;

    if (cont->type == Container_FromActivity)
    {
        Error_Fatal(!cont->typeData, "Container has no typeData");
        currAnimName = cont->typeData->name;
        frame = Container_Frame_Find(cont, currAnimName, 0);
    }
    else if (cont->type == Container_Anim)
    {
        frame = cont->activityFrame;
    }

    if (frame)
        return Container_Frame_GetFrameCount(frame);
    else
        return 0;
}

LPDIRECT3DRMFRAME3 Container_Frame_Find(lpContainer cont, const char* findName, U32 hidden)
{
    // TODO: Implement Container_Frame_Find
    return NULL;
}

F32 Container_Frame_GetCurrTime(LPDIRECT3DRMFRAME3 frame)
{
    Container_AppData *appData = (Container_AppData*) frame->lpVtbl->GetAppData(frame);
    Error_Fatal(!appData, "AppData not set on frame");
    if (appData)
        return appData->currTime;

    return 0.0f;
}

U32 Container_Frame_GetFrameCount(LPDIRECT3DRMFRAME3 frame)
{
    Container_AppData *appData = (Container_AppData*) frame->lpVtbl->GetAppData(frame);
    Error_Fatal(!appData, "AppData not set on frame");
    if (appData)
        return appData->frameCount;

    return 0;
}

lpAnimClone Container_Frame_GetAnimClone(LPDIRECT3DRMFRAME3 frame)
{
    Container_AppData *appData = (Container_AppData*) frame->lpVtbl->GetAppData(frame);
    Error_Fatal(!appData, "AppData not set on frame");
    if (appData)
        return appData->animClone;

    return NULL;
}

U32 Container_Frame_GetTrigger(LPDIRECT3DRMFRAME3 frame)
{
    Container_AppData *appData = (Container_AppData*) frame->lpVtbl->GetAppData(frame);
    Error_Fatal(!appData, "AppData not set on frame");
    if (appData)
        return appData->trigger;

    return 0;
}

Container_Type Container_ParseTypeString(const char* str, B32* noTexture)
{
    // For now, just check against one type...

    U32 loop, argc;
    char* argv[10];
    char string[40];

    Container_DebugCheckOK(CONTAINER_DEBUG_NOTREQUIRED);

    if (str != NULL)
    {
        strcpy(string, str);
        argc = Util_Tokenize(string, argv, ":");
        if (argc > 1 && stricmp(argv[1], "NOTEXTURE") == 0)
            *noTexture = TRUE;
        else
            *noTexture = FALSE;

        for (loop = 0; loop < Container_TypeCount; loop++)
        {
            if (containerGlobs.typeName[loop] != NULL)
            {
                if (stricmp(containerGlobs.typeName[loop], string) == 0)
                    return loop;
            }
        }
    } else
    {
        Error_Fatal(TRUE, "Null string passed to Container_ParseTypeString()");
    }

    return Container_Invalid;
}

lpContainer Container_Load(lpContainer parent, const char* filename, const char* typestr, B32 looping)
{
    Container_Type type;
    lpConfig rootConf, conf;
    U8 tempString[UTIL_DEFSTRINGLEN];
    U8 name[UTIL_DEFSTRINGLEN], baseDir[UTIL_DEFSTRINGLEN];
    lpContainer cont = NULL;
    F32 scale;
    B32 noTexture;

    Container_DebugCheckOK(CONTAINER_DEBUG_NOTREQUIRED);

    type = Container_ParseTypeString(typestr, &noTexture);

    strcpy(name, filename);

    if (type == Container_FromActivity)
    {
        // TODO: Implement Container_Load
    } else if (type == Container_Frame)
    {
        // TODO: Implement Container_Load
    } else if (type == Container_Mesh)
    {
        // TODO: Implement Container_Load
    } else if (type == Container_Anim || type == Container_LWS)
    {
        lpAnimClone animClone;
        U32 frameCount;

        cont = Container_Create(parent);
        cont->type = Container_Anim;
        sprintf(tempString, "%s.%s", name, containerGlobs.extensionNames[Container_Anim]);
        if ((animClone = Container_LoadAnimSet(tempString, cont->activityFrame, &frameCount, (type == Container_LWS) ? TRUE : FALSE, looping)))
        {
            Container_Frame_SetAppData(cont->activityFrame, cont, animClone, name, &frameCount, NULL, NULL, NULL, NULL, NULL, NULL);
        } else {
            Error_Warn(TRUE, Error_Format("Cannot Load File \"%s\".\n", tempString));
            Container_Remove(cont);
            cont = NULL;
        }
    } else if (type == Container_LWO)
    {
        // TODO: Implement Container_Load
    } else if (type == Container_Invalid)
    {
        Error_Fatal(TRUE, "Do not recognize container type");
    } else {
        Error_Warn(TRUE, Error_Format("Code not implemented for Container type #%d", type));
    }

#ifdef _DEBUG
    if (cont)
    {
        Container_Frame_FormatName(cont->masterFrame, "Master Frame (%s:%s)", name, typestr);
        Container_Frame_FormatName(cont->activityFrame, "Activity Frame (%s:%s)", name, typestr);
        Container_Frame_FormatName(cont->hiddenFrame, "Hidden Frame (%s:%s)", name, typestr);
    }
#endif

    return cont;
}

lpContainer Container_MakeLight(lpContainer parent, U32 type, F32 r, F32 g, F32 b)
{
    lpContainer cont = Container_Create(parent);
    LPDIRECT3DRMLIGHT light;
    Container_DebugCheckOK(CONTAINER_DEBUG_NOTREQUIRED);

    if (cont)
    {
        cont->type = Container_Light;
        //if (type == Container_Light_Ambient)
        //    Mesh_SetAmbientLight(r, g, b);

        if (lpD3DRM()->lpVtbl->CreateLightRGB(lpD3DRM(), type, r, g, b, &light) == D3DRM_OK)
        {
            Container_NoteCreation(light);

            cont->activityFrame->lpVtbl->AddLight(cont->activityFrame, light);
            Container_SetTypeData(cont, NULL, light, NULL, NULL);
        }
        else
        {
            Error_Fatal(TRUE, "Unable to create light");
        }

#ifdef _DEBUG
        Container_Frame_FormatName(cont->masterFrame, "Light type #%d (%0.2f,%0.2f,%0.2f)", type, r, g, b);
#endif
    }

    return cont;
}

lpContainer Container_MakeMesh2(lpContainer parent, Container_MeshType type)
{
    lpContainer cont = Container_Create(parent);
    LPDIRECT3DRMMESH mesh;

    Container_DebugCheckOK(CONTAINER_DEBUG_NOTREQUIRED);

    if (cont)
    {
        cont->type = Container_Mesh;

        if (type == Container_MeshType_Transparent ||
            type == Container_MeshType_Immediate ||
            type == Container_MeshType_Additive ||
            type == Container_MeshType_Subtractive)
        {
            U32 flags = MESH_FLAG_TRANSFORM_PARENTPOS;
            lpMesh transmesh;

            switch (type)
            {
            case Container_MeshType_Transparent:
                flags |= MESH_FLAG_RENDER_ALPHATRANS;
                break;
            case Container_MeshType_Additive:
                flags |= MESH_FLAG_RENDER_ALPHASA1;
                break;
            case Container_MeshType_Subtractive: // Not in LRR originally
                flags |= MESH_FLAG_RENDER_ALPHASA0;
                break;
            }

            transmesh = Mesh_CreateOnFrame(cont->activityFrame, NULL, flags, NULL, Mesh_Type_Norm);
            Container_SetTypeData(cont, NULL, NULL, NULL, transmesh);
        }
        else
        {
            if (lpD3DRM()->lpVtbl->CreateMesh(lpD3DRM(), &mesh) == D3DRM_OK)
            {
                Container_NoteCreation(mesh);

                cont->activityFrame->lpVtbl->AddVisual(cont->activityFrame, (struct IUnknown*) mesh);
                Container_SetTypeData(cont, NULL, NULL, mesh, NULL);

                if (type == Container_MeshType_SeperateMeshes)
                {
                    lpContainer_MeshAppData appdata = Mem_Alloc(sizeof(Container_MeshAppData));
                    appdata->listSize = CONTAINER_MESHGROUPBLOCKSIZE;
                    appdata->usedCount = 0;
                    appdata->meshList = Mem_Alloc(sizeof(LPDIRECT3DRMMESH) * appdata->listSize);
                    appdata->groupZeroHidden = FALSE;
                    appdata->firstAddGroup = TRUE;
                    mesh->lpVtbl->SetAppData(mesh, (U32) appdata);
                }
                else
                {
                    mesh->lpVtbl->SetAppData(mesh, 0);
                }
            } else
            {
                Error_Fatal(TRUE, "Cannot create mesh object");
            }
        }

#ifdef _DEBUG
        Container_Frame_FormatName(cont->masterFrame, "Mesh");
#endif // _DEBUG
    }

    return cont;
}

void Container_Hide2(lpContainer cont, B32 hide)
{
    if (hide)
        cont->flags |= CONTAINER_FLAG_HIDDEN2;
    else
        cont->flags &= ~CONTAINER_FLAG_HIDDEN2;
}

void Container_Hide(lpContainer cont, B32 hide)
{
    // Move the activity frame onto the hidden frame (will not hide any children attached
    // to the master frame)...

    B32 hidden;

    if (cont == NULL)
        return;

    hidden = cont->flags & CONTAINER_FLAG_HIDDEN;
    Container_DebugCheckOK(cont);

    if (hide && !hidden)
    {
        Container_Frame_SafeAddChild(cont->hiddenFrame, cont->activityFrame);
        cont->flags |= CONTAINER_FLAG_HIDDEN;
    }
    else if (!hide && hidden)
    {
        Container_Frame_SafeAddChild(cont->masterFrame, cont->activityFrame);
        cont->flags &= ~CONTAINER_FLAG_HIDDEN;
    }
}

void Container_SetSoundTriggerCallback(ContainerSoundTriggerCallback callback, void* data)
{
    containerGlobs.soundTriggerCallback = callback;
    containerGlobs.soundTriggerData = data;
    containerGlobs.flags |= CONTAINER_GLOB_FLAG_TRIGGERENABLED;
}

inline LPDIRECT3DRMFRAME3 Container_GetMasterFrame(lpContainer cont)
{
    return cont->masterFrame;
}

void Container_Frame_FormatName(LPDIRECT3DRMFRAME3 frame, const char* msg, ...)
{
    U8 buffer[1024];
    char* name;
    U32 len;
    va_list args;

    va_start(args, msg);
    len = vsprintf(buffer, msg, args);
    va_end(args);

    name = Mem_Alloc(len + 1);
    va_start(args, msg);
    vsprintf(name, msg, args);
    va_end(args);

    frame->lpVtbl->SetName(frame, name);
    Container_Frame_SetAppData(frame, NULL, NULL, NULL, NULL, name, NULL, NULL, NULL, NULL, NULL);
}

B32 Container_GetDecalColour(const char* fname, U32* colour)
{
    char *s, *t;

    for (s = t = fname; *t != '\0'; t++)
    {
        if (*t == '\\')
            s = t + 1;
    }

    if ((*s == 'a' || *s == 'A') && isdigit(*(s + 1)) && isdigit(*(s + 2)) && isdigit(*(s + 3)) && *(s + 4) == '_')
    {
        *colour = atoi(s + 1);
        return TRUE;
    }

    return FALSE;
}

lpAnimClone Container_LoadAnimSet(const char* fname, LPDIRECT3DRMFRAME3 frame, U32* frameCount, B32 lws, B32 looping)
{
    LPDIRECT3DRMANIMATIONSET2 animSet = NULL;
    LPDIRECT3DRMFRAME3 rootFrame;
    D3DRMLOADMEMORY buffer;
    Container_TextureData tData;
    lpAnimClone animClone = NULL;
    U32 fc;

    Error_Debug(Error_Format("Attempting to load animation from xfile \"%s\"\n", fname));

    if (lws)
    {
        lpLws_Info scene;
        char file[FILE_MAXPATH];

        strcpy(file, fname);
        file[strlen(file) - 2] = '\0';
        if ((scene = Lws_Parse(file, looping)))
        {
            Lws_LoadMeshes(scene, frame);
            Lws_SetTime(scene, 0.0f);
            if (frameCount)
                *frameCount = Lws_GetFrameCount(scene);
            animClone = AnimClone_RegisterLws(scene, frame, *frameCount);
        } else
        {
            Error_Warn(TRUE, "Cannot load file");
        }
    } else
    {
        // TODO: Implement Container_LoadAnimSet
    }

    return animClone;
}

LPDIRECTDRAWSURFACE4 Container_LoadTextureSurface(const char* fname, B32 managed, U32* width, U32* height, B32* trans)
{
    char* fileData;
    U32 size;
    D3DRMIMAGE image;
    LPDIRECTDRAWSURFACE4 surface = NULL;
    DDSURFACEDESC2 desc, descBak;
    LPDIRECTDRAWPALETTE palette;
    B32 copy = FALSE;
    HRESULT result;

    if ((fileData = File_LoadBinary(fname, &size)))
    {
        BMP_Parse(fileData, size, &image);

        if (image.rgb == FALSE && image.depth == 8)
        {
            memset(&desc, 0, sizeof(desc));
            desc.dwSize = sizeof(DDSURFACEDESC2);
            desc.dwFlags = DDSD_CAPS | DDSD_WIDTH | DDSD_HEIGHT | DDSD_PIXELFORMAT;
            desc.dwWidth = image.width;
            desc.dwHeight = image.height;
            desc.ddsCaps.dwCaps = DDSCAPS_TEXTURE;

            if (!(mainGlobs.flags & MAIN_FLAG_DONTMANAGETEXTURES))
            {
                if (managed)
                    desc.ddsCaps.dwCaps2 = DDSCAPS2_TEXTUREMANAGE;
                else
                    desc.ddsCaps.dwCaps |= DDSCAPS_SYSTEMMEMORY;
            } else
            {
                desc.ddsCaps.dwCaps |= DDSCAPS_VIDEOMEMORY;
            }

            // Find the preferred 8 bit palettized format...
            desc.ddpfPixelFormat.dwSize = sizeof(DDPIXELFORMAT);
            if (lpDevice()->lpVtbl->FindPreferredTextureFormat(lpDevice(), DDBD_8, D3DRMFPTF_PALETTIZED, &desc.ddpfPixelFormat) != D3DRM_OK)
            {
                memcpy(&descBak, &desc, sizeof(descBak));

                // If the card doesn't like this then create a standard 8 bit surface and then blit it to one it does like...
                memset(&desc.ddpfPixelFormat, 0, sizeof(desc.ddpfPixelFormat));

                desc.ddpfPixelFormat.dwSize = sizeof(DDPIXELFORMAT);
                desc.ddpfPixelFormat.dwFlags = DDPF_PALETTEINDEXED8 | DDPF_RGB;
                desc.ddpfPixelFormat.dwRGBBitCount = 8;

                desc.ddsCaps.dwCaps &= ~DDSCAPS_TEXTURE;
                desc.ddsCaps.dwCaps &= ~DDSCAPS_VIDEOMEMORY;
                desc.ddsCaps.dwCaps2 &= ~DDSCAPS2_TEXTUREMANAGE;

                desc.ddsCaps.dwCaps |= DDSCAPS_SYSTEMMEMORY;
                desc.ddsCaps.dwCaps |= DDSCAPS_OFFSCREENPLAIN;

                copy = TRUE;
            }

            if (DirectDraw()->lpVtbl->CreateSurface(DirectDraw(), &desc, &surface, NULL) == DD_OK)
            {
                memset(&desc, 0, sizeof(desc));
                desc.dwSize = sizeof(desc);

                if ((result = surface->lpVtbl->Lock(surface, NULL, &desc, DDLOCK_WAIT, NULL)) == DD_OK)
                {
                    S32 y;
                    char *surfaceMem = desc.lpSurface, *imageMem = image.buffer1;
                    for (y = 0; y < image.height; y++)
                    {
                        memcpy(surfaceMem, imageMem, image.bytes_per_line);
                        surfaceMem += desc.lPitch;
                        imageMem += image.bytes_per_line;
                    }
                    surface->lpVtbl->Unlock(surface, NULL);

                    if (DirectDraw()->lpVtbl->CreatePalette(DirectDraw(), DDPCAPS_INITIALIZE | DDPCAPS_8BIT | DDPCAPS_ALLOW256, (LPPALETTEENTRY) image.palette, &palette, NULL) == D3DRM_OK)
                    {
                        if (surface->lpVtbl->SetPalette(surface, palette) == D3DRM_OK)
                        {
                            U32 r, g, b;
                            U32 decalColour;

                            if (copy) // Find the cards preferred texture format...
                            {
                                memset(&descBak.ddpfPixelFormat, 0, sizeof(descBak.ddpfPixelFormat));
                                descBak.ddpfPixelFormat.dwSize = sizeof(DDPIXELFORMAT);
                                if (lpDevice()->lpVtbl->FindPreferredTextureFormat(lpDevice(), DDBD_16, 0, &descBak.ddpfPixelFormat) == D3DRM_OK)
                                {
                                    // Replace 'surface' with one of a format that the card will accept...

                                    LPDIRECTDRAWSURFACE4 oldSurface = surface;
                                    if (DirectDraw()->lpVtbl->CreateSurface(DirectDraw(), &descBak, &surface, NULL) == D3DRM_OK)
                                    {
                                        if (descBak.ddpfPixelFormat.dwRGBBitCount == 16)
                                            DirectDraw_Blt8To16(surface, oldSurface, (LPPALETTEENTRY) image.palette);

                                        oldSurface->lpVtbl->Release(oldSurface);
                                    } else
                                    {
                                        Error_Fatal(TRUE, "Error creating new texture surface");
                                    }
                                }
                            }

                            if (trans)
                            {
                                if (Container_GetDecalColour(fname, &decalColour))
                                {
                                    DDCOLORKEY ddck;

                                    if (copy)
                                    {
                                        r = image.palette[decalColour].red;
                                        g = image.palette[decalColour].green;
                                        b = image.palette[decalColour].blue;
                                        decalColour = DirectDraw_GetColour(surface, RGB_MAKE(r, g, b));
                                    }

                                    ddck.dwColorSpaceLowValue = ddck.dwColorSpaceHighValue = decalColour;
                                    surface->lpVtbl->SetColorKey(surface, DDCKEY_SRCBLT, &ddck);

                                    *trans = TRUE;
                                } else
                                {
                                    *trans = FALSE;
                                }
                            }

                            if (width)
                                *width = image.width;

                            if (height)
                                *height = image.height;

                            surface->lpVtbl->AddRef(surface);
                        }

                        palette->lpVtbl->Release(palette);
                    } else
                    {
                        Error_Fatal(TRUE, "Cannot create Palette");
                    }
                } else
                {
#ifdef _DEBUG
                    char error[128];

                    sprintf(error, "Texture file %s : Cannot lock surface ", fname);

                    switch(result)
                    {
                        case DDERR_INVALIDOBJECT:
                            strcat(error, "(DDERR_INVALIDOBJECT) ");
                            break;
                        case DDERR_INVALIDPARAMS:
                            strcat(error, "(DDERR_INVALIDPARAMS) ");
                            break;
                        case DDERR_OUTOFMEMORY:
                            strcat(error, "(DDERR_OUTOFMEMORY) ");
                            break;
                        case DDERR_SURFACEBUSY:
                            strcat(error, "(DDERR_SURFACEBUSY) ");
                            break;
                        case DDERR_SURFACELOST:
                            strcat(error, "(DDERR_SURFACELOST) ");
                            break;
                        case DDERR_WASSTILLDRAWING:
                            strcat(error, "(DDERR_WASSTILLDRAWING) ");
                            break;
                        case DDERR_LOCKEDSURFACES:
                            strcat(error, "(DDERR_LOCKEDSURFACES) ");
                            break;
                        default:
                            CHKDD(result);
                            break;
                    }

                    {
                        U32 refCount;
                        surface->lpVtbl->AddRef(surface);
                        refCount = surface->lpVtbl->Release(surface);
                        sprintf(&error[strlen(error)], "- Reference count == %i", refCount);
                    }

                    Error_Fatal(TRUE, error);
#endif // _DEBUG
                }

                if (surface->lpVtbl->Release(surface) == 0)
                    surface = NULL;
            }

        } else
        {
            Error_Warn(TRUE, Error_Format("Non 8bit/palettized texture %s", fname));
        }

        BMP_Cleanup(&image);
        Mem_Free(fileData);
    }
    return surface;
}

lpContainer Container_GetRoot()
{
    return containerGlobs.rootContainer;
}
