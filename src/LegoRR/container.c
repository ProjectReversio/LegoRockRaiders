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

void Container_Frame_SetAppData(LPDIRECT3DRMFRAME3 frame, lpContainer owner, lpAnimClone animClone, const char* asfname, U32* frameCount, const char* frameName, F32* currTime, F32* transCo, const char* actSample, void* soundRecord, U32* trigger)
{
    // TODO: Implement Container_Frame_SetAppData
}

void Container_SetSharedTextureDirectory(const char* path)
{
    // TODO: Implement Container_SetSharedTextureDirectory
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
    // TODO: Implement Container_SetParent
}

void Container_GetPosition(lpContainer cont, lpContainer ref, lpPoint3F pos)
{
    // TODO: Implement Container_GetPosition
}

void Container_GetOrientation(lpContainer cont, lpContainer ref, lpPoint3F dir, lpPoint3F up)
{
    // TODO: Implement Container_GetOrientation
}

F32 Container_SetAnimationTime(lpContainer cont, F32 time)
{
    // TODO: Implement Container_SetAnimationTime
    return 0.0f;
}

U32 Container_GetAnimationFrames(lpContainer cont)
{
    // TODO: Implement Container_GetAnimationFrames
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
    // TODO: Implement Container_MakeLight
    return NULL;
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
    // TODO: Implement Container_Hide
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
