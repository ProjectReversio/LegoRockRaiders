#include "container.h"
#include "error.h"
#include "activities.h"
#include "main.h"
#include "mem.h"

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
