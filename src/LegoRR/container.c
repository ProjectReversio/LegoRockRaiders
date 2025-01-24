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

#define CONTAINER_DISABLEFRAMESETTINGS

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
    U32 loop, count, sub, unfreed = 0;
    lpContainer testCont;

    Container_DebugCheckOK(CONTAINER_DEBUG_NOTREQUIRED);

    for (loop = 0; loop < CONTAINER_MAXLISTS; loop++)
    {
        if (containerGlobs.listSet[loop])
        {
            count = 0x00000001 << loop;

            for (sub = 0; sub < count; sub++)
            {
                if ((testCont = &containerGlobs.listSet[loop][sub]))
                {
                    if (testCont == testCont->nextFree)
                    {
                        Error_Debug(Error_Format("Warning: Unfreed Container type #%d\n", testCont->type));
                        unfreed++;
                        Container_Remove2(testCont, TRUE);
                    }
                }
            }

            Mem_Free(containerGlobs.listSet[loop]);
        }
    }

    containerGlobs.freeList = NULL;
    containerGlobs.flags = 0x00000000;

    for (loop = 0; loop < containerGlobs.textureCount; loop++)
    {
        if (containerGlobs.textureSet[loop].fileName)
        {
            Error_Debug(Error_Format("Texture %s was not removed\n", containerGlobs.textureSet[loop].fileName));
            Mem_Free(containerGlobs.textureSet[loop].fileName);
        }
    }

    if (containerGlobs.sharedDir)
        Mem_Free(containerGlobs.sharedDir);

#ifdef _DEBUG
    if (unfreed)
        Error_Debug(Error_Format("Warning: %d Unremoved Container%s\n", unfreed, unfreed == 1 ? "" : "s"));
#endif // _DEBUG

#ifdef _HIERARCHY_DEBUG
    Container_Debug_DumpCreationInfo();
#endif // _HIERARCHY_DEBUG
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

void Container_Frame_FreeName(LPDIRECT3DRMFRAME3 frame)
{
    const char* name;

    if ((name = Container_Frame_GetName(frame)))
    {
        Mem_Free(name);
    }
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
    LPDIRECT3DRMFRAME3 parentFrame;
    HRESULT r;

    Container_DebugCheckOK(dead);

    Error_Fatal(!dead, "NULL passed to Container_Remove()");

    if (dead->type != Container_Reference)
        Container_SetParent(dead, NULL); // Unparent it first...

    if (dead->type == Container_FromActivity || dead->type == Container_Anim)
    {
        if (dead->cloneData)
        {
            if (kill)
            {
                if (dead->cloneData->cloneTable)
                    Mem_Free(dead->cloneData->cloneTable);
                Mem_Free(dead->cloneData);
            }
            else
            {
                dead->cloneData->used = FALSE;
                return;
            }
        }
    }

    // The Animation sets for the activity type object are not stored in the
    // typeData (as there are an unlimited/unordered number of them) therefore
    // they are stored in the frames AppData section... So release them...
    if (dead->type == Container_FromActivity)
    {
        lpAnimClone *animCloneList;
        LPDIRECT3DRMFRAME3 *frameList;
        U32 loop, count;

        count = Container_GetActivities(dead, NULL, NULL, NULL);

        // Allocate two temporary buffers to be filled in with the list of
        // animation sets and frames (one for each activity)...
        animCloneList = Mem_Alloc(sizeof(lpAnimClone) * count);
        frameList = Mem_Alloc(sizeof(LPDIRECT3DRMFRAME3) * count);
        Container_GetActivities(dead, frameList, animCloneList, NULL);

        for (loop = 0; loop < count; loop++)
        {
            // Release the animation set,
            AnimClone_Remove(animCloneList[loop]);
            // and free the string defining the frame's name,
            Container_Frame_FreeName(frameList[loop]);
            // Remove the appdata...
            Container_Frame_RemoveAppData(frameList[loop]);
            // Then release out reference to the frame...
            r = frameList[loop]->lpVtbl->Release(frameList[loop]);
        }

        // Free the temporary buffers...
        Mem_Free(animCloneList);
        Mem_Free(frameList);
    }
    else if (dead->type == Container_Mesh)
    {
        lpContainer_MeshAppData appData;
        LPDIRECT3DRMMESH mesh, subMesh;
        lpMesh transmesh;
        U32 loop;

        if (dead->typeData)
        {
            if ((transmesh = dead->typeData->transMesh))
            {
                Mesh_Remove(dead->typeData->transMesh, dead->activityFrame);
            }
            else
            {
                mesh = dead->typeData->mesh;

                // Free the separate mesh groups if used...
                if ((appData = (lpContainer_MeshAppData) mesh->lpVtbl->GetAppData(mesh)))
                {
                    for (loop = 0; loop < appData->usedCount; loop++)
                    {
                        subMesh = appData->meshList[loop];
                        dead->activityFrame->lpVtbl->DeleteVisual(dead->activityFrame, (struct IUnknown*) subMesh);
                        r = subMesh->lpVtbl->Release(subMesh);
                    }

                    // Root mesh gets removed in Container_FreeTypeData()...

                    Mem_Free(appData->meshList);
                    Mem_Free(appData);
                }
            }
        }
    }
    else if (dead->type == Container_Anim)
    {
        lpAnimClone animClone;
        if ((animClone = Container_Frame_GetAnimClone(dead->activityFrame)))
        {
            AnimClone_Remove(animClone);
        }
    }

#ifdef _DEBUG
    if (dead->type == Container_Reference)
    {
        Container_Frame_FreeName(dead->masterFrame);
        Container_Frame_FreeName(dead->activityFrame);
    }
    Container_Frame_FreeName(dead->hiddenFrame);
#endif // _DEBUG

    //Remove the typeData from the container and the AppData from the frames...
    Container_FreeTypeData(dead);
    if (dead->type != Container_Reference || !(dead->flags & CONTAINER_FLAG_DEADREFERENCE))
        Container_Frame_RemoveAppData(dead->masterFrame);
    if (dead->type != Container_Reference)
        Container_Frame_RemoveAppData(dead->activityFrame);
    Container_Frame_RemoveAppData(dead->hiddenFrame);

    // Remove the frames and all unreferenced descendants from the hierarchy...

    if (dead->type != Container_Reference)
    {
        dead->masterFrame->lpVtbl->GetParent(dead->masterFrame, &parentFrame);

        if (parentFrame)
        {
            parentFrame->lpVtbl->DeleteChild(parentFrame, dead->masterFrame);
            parentFrame->lpVtbl->Release(parentFrame);
        }

        r = dead->masterFrame->lpVtbl->Release(dead->masterFrame);
        r = dead->activityFrame->lpVtbl->Release(dead->activityFrame);

        dead->hiddenFrame->lpVtbl->GetParent(dead->hiddenFrame, &parentFrame);
        if (parentFrame)
        {
            parentFrame->lpVtbl->DeleteChild(parentFrame, dead->hiddenFrame);
            parentFrame->lpVtbl->Release(parentFrame);
        }

        r = dead->hiddenFrame->lpVtbl->Release(dead->hiddenFrame);
    }
    else
    {
        dead->hiddenFrame->lpVtbl->Release(dead->hiddenFrame);
    }

    // Trash the container structure...
    Mem_DebugTrash(dead, CONTAINER_TRASHVALUE, sizeof(Container));

    // Link the freed Container in at the beginning of the free list so it may be re-used...
    dead->nextFree = containerGlobs.freeList;
    containerGlobs.freeList = dead;
}

U32 Container_GetActivities(lpContainer cont, LPDIRECT3DRMFRAME3* frameList, lpAnimClone* acList, char* nameList)
{
    // Either as List or nameList may be passed as NULL in which case they will not
    // be filled in (if both are NULL the result is the size of array required to hold them)...
    // If nameList is not passed as NULL, the char* pointers returned MUST BE FREED
    // AFTER USE!!!!

    LPDIRECT3DRMFRAMEARRAY children;
    LPDIRECT3DRMFRAME3 sourceFrame, childFrame;
    LPDIRECT3DRMFRAME frame1;
    U32 count, loop, source, nameLen = 0, listSize = 0;
    const char* name;
    HRESULT r;

    if (cont->type == Container_FromActivity)
    {
        for (source = 0; source < 2; source++)
        {
            if (source == 0)
                sourceFrame = cont->activityFrame;
            if (source == 1)
                sourceFrame = cont->hiddenFrame;

            if (sourceFrame->lpVtbl->GetChildren(sourceFrame, &children) == D3DRM_OK)
            {
                count = children->lpVtbl->GetSize(children);
                //Error_Warn(!count, "Can't find any children on frame");

                for (loop = 0; loop < count; loop++)
                {
                    children->lpVtbl->GetElement(children, loop, &frame1);

                    r = frame1->lpVtbl->QueryInterface(frame1, &IID_IDirect3DRMFrame3, &childFrame);
                    Error_Fatal(r, "Cannot query frame3");
                    frame1->lpVtbl->Release(frame1);

                    childFrame->lpVtbl->GetName(childFrame, &nameLen, name);
                    if (nameLen)
                    {
                        name = Mem_Alloc(nameLen);
                        childFrame->lpVtbl->GetName(childFrame, &nameLen, name);

                        if (_strnicmp(name, CONTAINER_ACTIVITYFRAMEPREFIX, strlen(CONTAINER_ACTIVITYFRAMEPREFIX)) == 0)
                        {
                            if (frameList != NULL)
                                frameList[listSize] = childFrame;
                            if (acList != NULL)
                                acList[listSize] = Container_Frame_GetAnimClone(childFrame);
                            if (nameList != NULL)
                                nameList[listSize] = name;
                            listSize++;

                            if (nameList == NULL)
                                Mem_Free(name);
                        }
                        else
                        {
                            Mem_Free(name);
                        }
                    }

                    r = childFrame->lpVtbl->Release(childFrame);
                }

                r = children->lpVtbl->Release(children);
            }
            else
            {
                Error_Fatal(TRUE, "GetChildren() call failed");
            }
        }
    }
    else
    {
        Error_Fatal(TRUE, "Container_GetActivities() supplied with a non-activity object");
    }

    return listSize;
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

void Container_FreeTypeData(lpContainer cont)
{
    HRESULT r;

    if (cont->typeData)
    {
        if (cont->type == Container_FromActivity)
        {
            Mem_Free(cont->typeData->name);
        }
        else if (cont->type == Container_Light)
        {
            r = cont->typeData->light->lpVtbl->Release(cont->typeData->light);
        }
        else if (cont->type == Container_Mesh)
        {
            if (cont->typeData->mesh)
            {
                cont->activityFrame->lpVtbl->DeleteVisual(cont->activityFrame, (struct IUnknown*) cont->typeData->mesh);
                r = cont->typeData->mesh->lpVtbl->Release(cont->typeData->mesh);
            }
        }
        else if (cont->type == Container_LWO)
        {
            if (cont->typeData->transMesh)
            {
                Mesh_Remove(cont->typeData->transMesh, cont->activityFrame);
            }
        }
        else
        {
            Error_Warn(TRUE, Error_Format("Code not implemented for Container type #%d", cont->type));
        }

        Mem_Free(cont->typeData);
        cont->typeData = NULL;
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

void Container_Frame_ReferenceDestroyCallback(LPDIRECT3DRMOBJECT lpD3DRMobj, void* lpArg)
{
    LPDIRECT3DRMFRAME3 frame = (LPDIRECT3DRMFRAME3) lpD3DRMobj;
    lpContainer cont;

    Error_Warn(TRUE, "Reference container's frame is being destroyed");

    if ((cont = Container_Frame_GetOwner(frame)))
    {
        cont->flags |= CONTAINER_FLAG_DEADREFERENCE;
        Container_Frame_RemoveAppData(frame);
    }
}

lpContainer Container_Frame_GetContainer(LPDIRECT3DRMFRAME3 frame)
{
    // Unlike Container_Frame_GetOwner() this will create a Container if there is
    // none already assigned...

    lpContainer cont;
    HRESULT r;

    Container_DebugCheckOK(frame);

    if ((cont = Container_Frame_GetOwner(frame)) == NULL)
    {
        // Create a new Container (along with redundant master frame).
        if ((cont = Container_Create(NULL)))
        {
            /*
            // Then move over the activity frame to the 'real' master frame,
            Container_Frame_SafeAddChild(frame, cont->activityFrame);
            // Release the unused master frame and replace it with the correct one.
            Container_Frame_RemoveAppData(cont->masterFrame);
            */

            // Remove the redundant frames from the container...
            r = cont->masterFrame->lpVtbl->Release(cont->masterFrame);
            r = cont->activityFrame->lpVtbl->Release(cont->activityFrame);
            //r = cont->hiddenFrame->lpVtbl->Release(cont->hiddenFrame);

            cont->masterFrame = frame;
            //cont->hiddenFrame = NULL;
            cont->activityFrame = NULL;

            cont->type = Container_Reference;

            frame->lpVtbl->AddDestroyCallback(frame, Container_Frame_ReferenceDestroyCallback, NULL);

            Container_Frame_SetAppData(frame, cont, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL);
        }
        else
        {
            Error_Warn(TRUE, "Couldn't create Container to surround frame");
        }
    }

    return cont;
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

void Container_AddRotation(lpContainer cont, Container_Combine combine, F32 x, F32 y, F32 z, F32 angle)
{
    LPDIRECT3DRMFRAME3 frame;
    Container_DebugCheckOK(cont);

    frame = cont->masterFrame;
    Error_Fatal(!frame, "Container has no masterFrame");

    frame->lpVtbl->AddRotation(frame, combine, x, y, z, angle);
}

void Container_AddScale(lpContainer cont, Container_Combine combine, F32 x, F32 y, F32 z)
{
    LPDIRECT3DRMFRAME3 frame;
    Container_DebugCheckOK(cont);

    frame = cont->masterFrame;
    Error_Fatal(!frame, "Container has no masterFrame");

    frame->lpVtbl->AddScale(frame, combine, x, y, z);
}

void Container_AddTranslation(lpContainer cont, Container_Combine combine, F32 x, F32 y, F32 z)
{
    LPDIRECT3DRMFRAME3 frame;
    Container_DebugCheckOK(cont);

    frame = cont->masterFrame;
    Error_Fatal(!frame, "Container has no masterFrame");

    frame->lpVtbl->AddTranslation(frame, combine, x, y, z);
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

B32 Container_Light_SetSpotPenumbra(lpContainer spotlight, F32 angle)
{
    Container_DebugCheckOK(spotlight);
    Error_Fatal(spotlight->type != Container_Light, "Container is not a light");

    if (spotlight->typeData->light->lpVtbl->SetPenumbra(spotlight->typeData->light, angle) == D3DRM_OK)
        return TRUE;
    else
        return FALSE;
}

B32 Container_Light_SetSpotUmbra(lpContainer spotlight, F32 angle)
{
    Container_DebugCheckOK(spotlight);
    Error_Fatal(spotlight->type != Container_Light, "Container is not a light");

    if (spotlight->typeData->light->lpVtbl->SetUmbra(spotlight->typeData->light, angle) == D3DRM_OK)
        return TRUE;
    else
        return FALSE;
}

B32 Container_Light_SetSpotRange(lpContainer spotlight, F32 dist)
{
    Container_DebugCheckOK(spotlight);
    Error_Fatal(spotlight->type != Container_Light, "Container is not a light");

    if (spotlight->typeData->light->lpVtbl->SetRange(spotlight->typeData->light, dist) == D3DRM_OK)
        return TRUE;
    else
        return FALSE;
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

B32 Container_AddActivity2(lpContainer cont, const char* filename, const char* actname, F32 transCo, U32 trigger, const char* sample, struct AnimClone* origClone, B32 lws, B32 looping)
{
    LPDIRECT3DRMFRAME3 newFrame;
    char xFile[UTIL_DEFSTRINGLEN];
    U32 frameCount;
    lpAnimClone animClone;

    sprintf(xFile, "%s.%s", filename, containerGlobs.extensionNames[Container_Anim]);

    if (lpD3DRM()->lpVtbl->CreateFrame(lpD3DRM(), cont->hiddenFrame, &newFrame) == D3DRM_OK)
    {
        Container_NoteCreation(newFrame);

        // Set the name of the parent to the animation set.
        // Freed when the container is destroyed

        // This will also set the appData's pointer to the name...
        Container_Frame_FormatName(newFrame, "%s_%s", CONTAINER_ACTIVITYFRAMEPREFIX, actname);

        // Load in the AnimationSet.
        if (origClone == NULL)
            animClone = Container_LoadAnimSet(xFile, newFrame, &frameCount, lws, looping);
        else
            animClone = AnimClone_Make(origClone, newFrame, &frameCount);

        Container_Frame_SetAppData(newFrame, cont, animClone, filename, &frameCount, NULL, NULL, &transCo, sample, NULL, &trigger);

        return TRUE;
    }
    else
    {
        Error_Warn(TRUE, "Unable to create frame for new activity");
    }

    return FALSE;
}

B32 Container_SetActivity(lpContainer cont, const char* actname)
{
    LPDIRECT3DRMFRAME3 frame, currFrame;
    B32 result = FALSE;
    const char* currAnimName, *name, *freeAddr = NULL;

    Container_DebugCheckOK(cont);

    if (cont->type == Container_FromActivity)
    {
        if ((frame = Container_Frame_Find(cont, actname, 1)))
        {
            if (cont->typeData != NULL)
            {
                if (cont->typeData->name != NULL)
                {
                    currAnimName = cont->typeData->name;
                    if ((currFrame = Container_Frame_Find(cont, currAnimName, 0)))
                    {
                        Container_Frame_SafeAddChild(cont->hiddenFrame, currFrame);
                    }
                    freeAddr = currAnimName;
                }
                else
                {
                    Error_Warn(TRUE, "container has no typedata name");
                }
            }

            Container_Frame_SafeAddChild(cont->activityFrame, frame);
            name = Mem_Alloc(strlen(actname) + 1);
            strcpy(name, actname);
            Container_SetTypeData(cont, name, NULL, NULL, NULL);

            result = TRUE;

            // Flag that the sample should be played on the next SetTime...
            cont->flags |= CONTAINER_FLAG_TRIGGERSAMPLE;

            if (freeAddr)
                Mem_Free(freeAddr);
        }
        else
        {
            result = FALSE;
            Error_Warn(!Container_Frame_Find(cont, actname, 0), Error_Format("Unknown activity (\"%s\") passed to Container_SetActivity()", actname));
        }

        // Notify the game that the activity has changed...

#pragma message("Is the activity change callback required?")
        if (cont->activityCallback)
            cont->activityCallback(cont, cont->activityCallbackData);
    }

    return result;
}

F32 Container_GetAnimationTime(lpContainer cont)
{
    LPDIRECT3DRMFRAME3 frame;
    char* currAnimName;
    F32 time = 0.0f;

    Container_DebugCheckOK(cont);

    if (cont->type == Container_FromActivity)
    {
        Error_Fatal(!cont->typeData, "Container has no typeData");

        currAnimName = cont->typeData->name;
        if ((frame = Container_Frame_Find(cont, currAnimName, 0)))
            time = Container_Frame_GetCurrTime(frame);
        else
            Error_Warn(TRUE, "Couldn't find frame (thus AnimationSet) to SetTime() on");
    }
    else if (cont->type == Container_Anim)
    {
        time = Container_Frame_GetCurrTime(cont->activityFrame);
    }
    else
    {
        //Error_Fatal(TRUE, "Container_SetTime() called with non-animation type container");
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
        Error_Fatal(!cont->typeData, "Container has no typeData");

        currAnimName = cont->typeData->name;

        if ((frame = Container_Frame_Find(cont, currAnimName, 0)))
        {
            animClone = Container_Frame_GetAnimClone(frame);

            if (cont->flags & CONTAINER_FLAG_TRIGGERSAMPLE)
            {
                const char* sound;
                if ((sound = Container_Frame_GetSample(frame)))
                {
                    if (containerGlobs.soundTriggerCallback && (containerGlobs.flags & CONTAINER_GLOB_FLAG_TRIGGERENABLED))
                        containerGlobs.soundTriggerCallback(sound, cont, containerGlobs.soundTriggerData);
                }

                cont->flags &= ~CONTAINER_FLAG_TRIGGERSAMPLE;
            }
        } else
        {
            Error_Warn(TRUE, "Couldn't find frame (thus AnimationSet) to SetTime() on");
        }
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

U32 Container_GetAnimFileFrameCount(const char* fileData)
{
    U32 count = 0;

    if ((fileData = strstr(fileData, "DDiScene")))
    {
        while (!isdigit(*fileData))
            fileData++;
        count = atoi(fileData);
    }

    if (count == 0)
        count = 1;
    return count;
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
    LPDIRECT3DRMFRAME3 frame, childFrame, foundFrame = NULL;
    LPDIRECT3DRMFRAME frame1;
    LPDIRECT3DRMFRAMEARRAY children;
    U32 count, loop, nameLen;
    char tempString[UTIL_DEFSTRINGLEN];
    const char* name;
    HRESULT r;

    if (hidden)
        frame = cont->hiddenFrame;
    else
        frame = cont->activityFrame;

    if (frame->lpVtbl->GetChildren(frame, &children) == D3DRM_OK)
    {
        count = children->lpVtbl->GetSize(children);
        Error_Warn(!count, "Can't find any children on frame");

        for (loop = 0; loop < count; loop++)
        {
            children->lpVtbl->GetElement(children, loop, &frame1);

            r = frame1->lpVtbl->QueryInterface(frame1, &IID_IDirect3DRMFrame3, &childFrame);
            Error_Fatal(r, "Error getting frame3");
            frame1->lpVtbl->Release(frame1);

            childFrame->lpVtbl->GetName(childFrame, &nameLen, NULL);
            if (nameLen)
            {
                name = Mem_Alloc(nameLen + 1);
                childFrame->lpVtbl->GetName(childFrame, &nameLen, name);

                sprintf(tempString, "%s_%s", CONTAINER_ACTIVITYFRAMEPREFIX, findName);
                if (stricmp(name, tempString) == 0)
                    foundFrame = childFrame;
                Mem_Free(name);
            }

            r = childFrame->lpVtbl->Release(childFrame);

            if (foundFrame)
                break;
        }

        r = children->lpVtbl->Release(children);
    }
    else
    {
        Error_Fatal(TRUE, "GetChildren() call failed");
    }

    return foundFrame;
}

void Container_Frame_RemoveAppData(LPDIRECT3DRMFRAME3 frame)
{
    Container_AppData* appData = (Container_AppData*) frame->lpVtbl->GetAppData(frame);
    Error_Fatal(!appData, "AppData not set on frame");
    if (appData)
    {
        if (appData->animSetFileName != NULL)
            Mem_Free(appData->animSetFileName);
        Mem_Free(appData);
    }

    frame->lpVtbl->SetAppData(frame, (DWORD)NULL);
}

const char* Container_Frame_GetName(LPDIRECT3DRMFRAME3 frame)
{
    Container_AppData* appData = (Container_AppData*) frame->lpVtbl->GetAppData(frame);
    Error_Fatal(!appData, "AppData not set on frame");
    if (appData)
        return appData->frameName;
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

lpContainer Container_Frame_GetOwner(LPDIRECT3DRMFRAME3 frame)
{
    Container_AppData *appData = (Container_AppData*) frame->lpVtbl->GetAppData(frame);
    //Error_Warn(!appData, Error_Format("AppData not set on frame 0x%0.8x", frame));
    if (appData)
        return appData->ownerContainer;

    return NULL;
}

const char* Container_Frame_GetAnimSetFileName(LPDIRECT3DRMFRAME3 frame)
{
    Container_AppData* appData = (Container_AppData*) frame->lpVtbl->GetAppData(frame);
    Error_Fatal(!appData, "AppData not set on frame");
    if (appData)
        return appData->animSetFileName;

    return NULL;
}

lpAnimClone Container_Frame_GetAnimClone(LPDIRECT3DRMFRAME3 frame)
{
    Container_AppData *appData = (Container_AppData*) frame->lpVtbl->GetAppData(frame);
    Error_Fatal(!appData, "AppData not set on frame");
    if (appData)
        return appData->animClone;

    return NULL;
}

const char* Container_Frame_GetSample(LPDIRECT3DRMFRAME3 frame)
{
    Container_AppData *appData = (Container_AppData*) frame->lpVtbl->GetAppData(frame);
    Error_Fatal(!appData, "AppData not set on frame");
    if (appData)
            return appData->activitySample;

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

U32 Container_Mesh_GetVertices(lpContainer cont, U32 groupID, U32 index, U32 count, lpVertex retArray)
{
    LPDIRECT3DRMMESH mesh;
    lpMesh transmesh;

    Container_DebugCheckOK(cont);

    Error_Fatal(cont->type != Container_Mesh && cont->type != Container_LWO, "Container_Mesh_GetVertices() called with non mesh object");

    transmesh = cont->typeData->transMesh;
    if (transmesh)
    {
        Mesh_GetVertices(transmesh, groupID, index, count, retArray);
    }
    else
    {
        mesh = cont->typeData->mesh;
        Error_Fatal(!mesh, "Container has no mesh object");

        Container_Mesh_DebugCheckOK(cont, groupID);

        Container_Mesh_HandleSeparateMeshGroups(&mesh, &groupID);

        if (mesh->lpVtbl->GetVertices(mesh, groupID, index, count, (LPD3DRMVERTEX) retArray) != D3DRM_OK)
        {
            Error_Fatal(TRUE, "Unable to GetVertices");
            return CONTAINER_ULONG_NULL;
        }
    }

    return 0;
}

U32 Container_Mesh_SetVertices(lpContainer cont, U32 groupID, U32 index, U32 count, lpVertex values)
{
    LPDIRECT3DRMMESH mesh;
    lpMesh transmesh;

    Container_DebugCheckOK(cont);

    Error_Fatal(cont->type != Container_Mesh && cont->type != Container_LWO, "Container_Mesh_SetVertices() called with non mesh object");

    transmesh = cont->typeData->transMesh;
    if (transmesh)
    {
        Mesh_SetVertices(transmesh, groupID, index, count, values);
    }
    else
    {
        mesh = cont->typeData->mesh;
        Error_Fatal(!mesh, "Container has no mesh object");

        Container_Mesh_DebugCheckOK(cont, groupID);

        Container_Mesh_HandleSeparateMeshGroups(&mesh, &groupID);

        if (mesh->lpVtbl->SetVertices(mesh, groupID, index, count, (LPD3DRMVERTEX) values) != D3DRM_OK)
        {
            Error_Fatal(TRUE, "Unable to SetVertices");
            return CONTAINER_ULONG_NULL;
        }
    }

    return 0;
}

lpContainer Container_SearchTree(lpContainer root, const char* name, Container_SearchMode mode, U32* count)
{
    Container_SearchData search;

    Container_DebugCheckOK(root);

    search.string = name;
    search.stringLen = strlen(name);
    search.caseSensitive = FALSE;
    search.resultFrame = NULL;
    search.count = 0;
    search.mode = mode;

    if (count)
        search.matchNumber = *count;
    else
        search.matchNumber = 0;

#ifdef CONTAINER_MATCHHIDDENHIERARCHY
    if (root->flags & CONTAINER_FLAG_HIDDEN)
    {
        Container_Frame_WalkTree(root->activityFrame, 0, Container_Frame_SearchCallback, &search);
    } else
    {
#endif // CONTAINER_MATCHHIDDENHIERARCHY

    Container_Frame_WalkTree(root->masterFrame, 0, Container_Frame_SearchCallback, &search);

#ifdef CONTAINER_MATCHHIDDENHIERARCHY
    }
#endif

    if (mode == Container_SearchMode_FirstMatch || mode == Container_SearchMode_NthMatch)
    {
        if (search.resultFrame)
            return Container_Frame_GetContainer(search.resultFrame);
    }
    else if (mode == Container_SearchMode_MatchCount)
    {
        *count = search.count;
    }

    return NULL;
}

B32 Container_Frame_WalkTree(LPDIRECT3DRMFRAME3 frame, U32 level, Container_SearchCallback callback, void* data)
{
    LPDIRECT3DRMFRAMEARRAY children;
    LPDIRECT3DRMFRAME3 child;
    LPDIRECT3DRMFRAME child1;
    U32 count, loop;
    B32 finished = FALSE;
    HRESULT r;

    if (callback(frame, data))
        return TRUE;

    frame->lpVtbl->GetChildren(frame, &children);
    count = children->lpVtbl->GetSize(children);

    for (loop = 0; loop < count; loop++)
    {
        children->lpVtbl->GetElement(children, loop, &child1);
        child1->lpVtbl->QueryInterface(child1, &IID_IDirect3DRMFrame3, &child);
        child1->lpVtbl->Release(child1);

        if (Container_Frame_WalkTree(child, level + 1, callback, data))
        {
            finished = TRUE;
            r = child->lpVtbl->Release(child);
            break;
        }
        r = child->lpVtbl->Release(child);
    }

    r = children->lpVtbl->Release(children);

    return finished;
}

B32 Container_Frame_SearchCallback(LPDIRECT3DRMFRAME3 frame, void* data)
{
    Container_SearchData *search = data;
    char* name;
    U32 len, loop;

    frame->lpVtbl->GetName(frame, &len, NULL);

    if (search->stringLen == len - 1)
    {
        name = Mem_Alloc(len);
        name[0] = '\0';
        frame->lpVtbl->GetName(frame, &len, name);

        // Replace any characters in the name string with '?' if their position
        // corresponds to a '?' in the search string...
        for (loop = 0; loop < len; loop++)
        {
            if (search->string[loop] == '?')
                name[loop] = '?';
        }

        if (search->mode == Container_SearchMode_FirstMatch)
        {
            search->resultFrame = NULL;
            if (search->caseSensitive)
            {
                if (strcmp(name, search->string) == 0)
                    search->resultFrame = frame;
            }
            else
            {
                if (_stricmp(name, search->string) == 0)
                    search->resultFrame = frame;
            }
        }
        else if (search->mode == Container_SearchMode_MatchCount)
        {
            if (search->caseSensitive)
            {
                if (strcmp(name, search->string) == 0)
                    search->count++;
            }
            else
            {
                if (_stricmp(name, search->string) == 0)
                    search->count++;
            }
        }
        else if (search->mode == Container_SearchMode_NthMatch)
        {
            search->resultFrame = NULL;
            if (search->caseSensitive)
            {
                if (strcmp(name, search->string) == 0)
                    search->count++;
            }
            else
            {
                if (_stricmp(name, search->string) == 0)
                    search->count++;
            }

            if (search->count == search->matchNumber + 1)
                search->resultFrame = frame;
        }
        else
        {
            Error_Fatal(TRUE, "Unknown search type");
        }

        Mem_Free(name);

        if (search->resultFrame)
            return TRUE;
    }

    return FALSE;
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
        if (argc > 1 && _stricmp(argv[1], "NOTEXTURE") == 0)
            *noTexture = TRUE;
        else
            *noTexture = FALSE;

        for (loop = 0; loop < Container_TypeCount; loop++)
        {
            if (containerGlobs.typeName[loop] != NULL)
            {
                if (_stricmp(containerGlobs.typeName[loop], string) == 0)
                    return loop;
            }
        }
    } else
    {
        Error_Fatal(TRUE, "Null string passed to Container_ParseTypeString()");
    }

    return Container_Invalid;
}

const char* Container_FormatPartName(lpContainer cont, const char* partname, U32* instance)
{
    // e.g. xf_????????_lphead_stationary_00_DDc_00 <- clone number (redundant (always zero))
    //         ^uid     ^part  ^filename  ^instance

    static char name[1024];
    char tempString[1024];
    LPDIRECT3DRMFRAME3 frame;
    char* fname;
    char* s;
    lpAnimClone animClone;

    Container_DebugCheckOK(cont);

    if (cont->type == Container_FromActivity)
    {
        Error_Fatal(!cont->typeData, "Container has no typeData");
        frame = Container_Frame_Find(cont, cont->typeData->name, 0);
        Error_Fatal(frame == NULL, "Cannot locate current activities frame");
    }
    else if (cont->type == Container_Anim)
    {
        frame = cont->activityFrame;
    }
    else
    {
        Error_Fatal(TRUE, "Calling this function with a non-animation typ Container serves no purpose");
    }

    animClone = Container_Frame_GetAnimClone(frame);
    // ^^^^^^ If animClone is NULL then you have probably misspelled the animation filename. Duh!

    if (AnimClone_IsLws(animClone))
    {
        if (instance)
            sprintf(name, "%s_%0.2i", partname, *instance);
        else
            sprintf(name, "%s_??", partname);
    }
    else
    {
        sprintf(tempString, "%s", Container_Frame_GetAnimSetFileName(frame));
        _strlwr(tempString);

        for (fname = s = tempString; *s != '\0'; s++)
        {
            if (*s == '\\')
                fname = s + 1;
        }

        if (instance)
            sprintf(name, "xf_????????_%s_%s_%0.2d_DDc_00", partname, fname, *instance);
        else
            sprintf(name, "xf_????????_%s_%s_??_DDc_00", partname, fname);
    }

    return name;
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
        const char* argv[20];
        U32 argc;

        Error_Fatal(!containerGlobs.gameName, "Need to set game name in Container_Initialize() in order to load from Activity File");

        // The name of the 'FromActivity' container defines the directory
        // in which all of the files will be located.

        strcpy(baseDir, name);
        argc = Util_Tokenize(name, argv, "\\");
        sprintf(tempString, "%s\\%s.%s", baseDir, argv[argc - 1], containerGlobs.extensionNames[Container_FromActivity]);

        Error_Debug(Error_Format("Loading activity file \"%s\"\n", tempString));

        if ((rootConf = Config_Load(tempString)))
        {
            char tempString2[UTIL_DEFSTRINGLEN];

            sprintf(tempString, "%s%s%s", containerGlobs.gameName, CONFIG_SEPARATOR, ACTIVITY_ACTIVITYSTRING);
            Error_Debug(Error_Format("Searching for \"%s\" in activity file\n", tempString));
            if ((conf = Config_FindArray(rootConf, tempString)))
            {
                cont = Container_Create(parent);
                cont->type = type;

                while (conf)
                {
                    const char *fileStr, *sampleStr;
                    F32 transCo;
                    U32 trigger;
                    B32 lws = FALSE, looping;
                    const char* itemName = conf->itemName;
                    //lpSound sample;

                    if ((*itemName != '!') || !(mainGlobs.flags & MAIN_FLAG_REDUCEANIMATION))
                    {
                        if (*itemName == '!')
                            itemName++;

                        sprintf(tempString2, "%s%s%s%s%s", containerGlobs.gameName, CONFIG_SEPARATOR, conf->dataString, CONFIG_SEPARATOR, ACTIVITY_FILESTRING);

                        if ((fileStr = Config_GetTempStringValue(rootConf, tempString2)))
                        {
                            sprintf(tempString, "%s\\%s", baseDir, fileStr);
                            sprintf(tempString2, "%s%s%s%s%s", containerGlobs.gameName, CONFIG_SEPARATOR, conf->dataString, CONFIG_SEPARATOR, ACTIVITY_TRANSCOSTRING);

                            //if (Config_FindItem(rootConf, tempString2))
                            //{
                            transCo = Config_GetFloatValue(rootConf, tempString2);
                            sprintf(tempString2, "%s%s%s%s%s", containerGlobs.gameName, CONFIG_SEPARATOR, conf->dataString, CONFIG_SEPARATOR, ACTIVITY_TRIGGERSTRING);
                            trigger = Config_GetIntValue(rootConf, tempString2);
                            sprintf(tempString2, "%s%s%s%s%s", containerGlobs.gameName, CONFIG_SEPARATOR, conf->dataString, CONFIG_SEPARATOR, "LWSFILE");
                            lws = Config_GetBoolValue(rootConf, tempString2) == BOOL3_TRUE;
                            sprintf(tempString2, "%s%s%s%s%s", containerGlobs.gameName, CONFIG_SEPARATOR, conf->dataString, CONFIG_SEPARATOR, "LOOPING");
                            if (Config_GetBoolValue(rootConf, tempString2) == BOOL3_FALSE)
                                looping = FALSE;
                            else
                                looping = TRUE;

                            // Get the activity sample...

                            sprintf(tempString2, "%s%s%s%s%s", containerGlobs.gameName, CONFIG_SEPARATOR, conf->dataString, CONFIG_SEPARATOR, ACTIVITY_SAMPLESTRING);
                            sampleStr = Config_GetStringValue(rootConf, tempString2);
                            // if (sampleStr)
                            // {
                            //     sprintf(tempString2, "%s\\%s", baseDir, fileStr);
                            //     if ((sample = Sound_Load(tempString2)) == NULL)
                            //         Error_Fatal(TRUE, "Cannot load sample");
                            // }
                            // else
                            // {
                            //     sample = NULL;
                            // }

                            if (Container_AddActivity(cont, tempString, itemName, transCo, trigger, sampleStr, NULL, lws, looping))
                            {
                                if (!cont->typeData)
                                    Container_SetActivity(cont, itemName);
                            }
                            else
                            {
                                Error_Fatal(TRUE, Error_Format("Unable to load Activity \"%s\" from \"%s\"", itemName, tempString));
                            }

                            //}
                            //else
                            //{
                            //    Error_Fatal(TRUE, Error_Format("Cannot get \"%s\" value from activity file", tempString2));
                            //}
                        }
                        else
                        {
                            Error_Fatal(TRUE, Error_Format("Unable to get item \"%s\" from activity file", tempString2));
                        }
                    }

                    conf = Config_GetNextItem(conf);
                }
            }
            else
            {
                Error_Fatal(TRUE, Error_Format("Cannot Find Activity List %s", tempString));
            }

            sprintf(tempString, "%s%s%s", containerGlobs.gameName, CONFIG_SEPARATOR, CONTAINER_SCALESTRING);
            scale = Config_GetFloatValue(rootConf, tempString);
            if (scale != 0.0f)
                cont->activityFrame->lpVtbl->AddScale(cont->activityFrame, D3DRMCOMBINE_REPLACE, scale, scale, scale);

            Config_Free(rootConf);
        }
        else
        {
            Error_Warn(TRUE, Error_Format("Cannot Find File %s", tempString));
        }
    } else if (type == Container_Frame)
    {
        cont = Container_Create(parent);
        cont->type = type;

        // Just add it onto the activity frame...
        sprintf(tempString, "%s.%s", name, containerGlobs.extensionNames[type]);
        if (!Container_FrameLoad(tempString, cont->activityFrame))
        {
            Error_Warn(TRUE, Error_Format("Cannot Load File \"%s\".\n", tempString));
        }
    } else if (type == Container_Mesh)
    {
        LPDIRECT3DRMMESH mesh;
        LPVOID fdata;
        U32 fsize;

        // Create a meshbuilder, retrieve the mesh object
        // then attach it to the activity frame...
        sprintf(tempString, "%s.%s", name, containerGlobs.extensionNames[type]);
        if ((fdata = File_LoadBinary(tempString, &fsize)))
        {
            cont = Container_Create(parent);
            cont->type = type;
            if ((mesh = Container_MeshLoad(fdata, fsize, tempString, cont->activityFrame, noTexture)))
            {
                Container_SetTypeData(cont, NULL, NULL, mesh, NULL);
            }
            else
            {
                //Error_Warn(TRUE, Error_Format("Cannot Load File \"%s\"", tempString));
            }
            Mem_Free(fdata);
        }
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
        lpMesh mesh;
        cont = Container_Create(parent);
        cont->type = type;

        //sprintf(tempString, "%s.lwo", name);
        //if ((mesh = Lws_LoadMesh(NULL, name, cont->activityFrame, noTexture)))
        if ((mesh = Mesh_Load(name, cont->activityFrame, noTexture)))
        {
            Container_SetTypeData(cont, NULL, NULL, NULL, mesh);
        }
        else
        {
            Error_Warn(TRUE, Error_Format("Cannot Load File \"%s\".", tempString));
            Container_Remove(cont);
            cont = NULL;
        }
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

void Container_SetColourAlpha(lpContainer cont, F32 r, F32 g, F32 b, F32 a)
{
    D3DCOLOR colour;

    Container_DebugCheckOK(cont);
    Error_Fatal(!cont->typeData, "Container has no typeData");

    colour = Container_GetRGBAColour(r, g, b, a);

    if (cont->type == Container_Light)
    {
        Error_Fatal(!cont->typeData->light, "typedata has no light");
        cont->typeData->light->lpVtbl->SetColor(cont->typeData->light, colour);

        /// NEW GODS98: Feature not present in LegoRR
        //if (cont->typeData->light->lpVtbl->GetType(cont->typeData->light) == D3DRMLIGHT_AMBIENT)
        //    Mesh_SetAmbientLight(r, g, b);
    }
}

lpContainer_Texture Container_LoadTexture2(const char* fname, B32 immediate, U32* width, U32* height)
{
    LPDIRECT3DRMTEXTURE3 texture = NULL;
    const char* path = File_VerifyFilename(fname);
    lpContainer_Texture newText;
    U32 decalColour;
    HRESULT r;

    // This malloc() (not Mem_Alloc()) is deliberate...
    if ((newText = malloc(sizeof(Container_Texture))))
    {
        //#ifdef CONTAINER_USEOWNTEXTURELOAD
        if (immediate)
        {
            LPDIRECTDRAWSURFACE surf1;
            LPDIRECTDRAWSURFACE4 surf;
            PALETTEENTRY entry;

            if ((surf = Container_LoadTextureSurface(fname, immediate, width, height, NULL)))
            {
                Error_Debug(Error_Format("Loaded texture %s\n", path));

                if (surf->lpVtbl->QueryInterface(surf, &IID_IDirectDrawSurface, &surf1) == D3DRM_OK)
                {
                    if (lpD3DRM()->lpVtbl->CreateTextureFromSurface(lpD3DRM(), surf1, &texture) == D3DRM_OK)
                    {
                        Container_NoteCreation(texture);
                        surf1->lpVtbl->Release(surf1);

                        if (Container_GetDecalColour(fname, &decalColour))
                        {
                            LPDIRECTDRAWPALETTE palette;
                            surf->lpVtbl->GetPalette(surf, &palette);

                            palette->lpVtbl->GetEntries(palette, 0, decalColour, 1, &entry);
                            palette->lpVtbl->Release(palette);

                            texture->lpVtbl->SetDecalTransparency(texture, TRUE);
                            texture->lpVtbl->SetDecalTransparentColor(texture, RGB_MAKE(entry.peRed, entry.peGreen, entry.peBlue));
                        }

                        newText->texture = texture;
                        newText->surface = surf;
                        texture->lpVtbl->SetAppData(texture, (U32) newText);

                        return newText;
                    }

                    surf1->lpVtbl->Release(surf1);
                }

                surf->lpVtbl->Release(surf);
            }

        } else
        //#else // CONTAINER_USEOWNTEXTURELOAD
        {
            if ((r = lpD3DRM()->lpVtbl->LoadTexture(lpD3DRM(), path, &texture)))
            {
                LPD3DRMIMAGE image;
                U32 r, g, b;
                Container_NoteCreation(texture);

                Error_Debug(Error_Format("Loaded texture %s\n", path));
                Error_LogLoad(TRUE, path);

                if ((image = texture->lpVtbl->GetImage(texture)))
                {
                    if (Container_GetDecalColour(fname, &decalColour))
                    {
                        r = image->palette[decalColour].red;
                        g = image->palette[decalColour].green;
                        b = image->palette[decalColour].blue;
                        texture->lpVtbl->SetDecalTransparency(texture, TRUE);
                        texture->lpVtbl->SetDecalTransparentColor(texture, RGB_MAKE(r, g, b));
                    }

                    if (width)
                        *width = image->width;
                    if (height)
                        *height = image->height;
                }

                newText->texture = texture;
                newText->surface = NULL;
                texture->lpVtbl->SetAppData(texture, (U32)newText);

            } else
            {
                if (r == D3DRMERR_FILENOTFOUND)
                {
                    Error_Warn(TRUE, Error_Format("Invalid filename specified \"%s\"", path));
                    Error_LogLoadError(TRUE, Error_Format("%d\t%s", Error_LoadError_InvalidFName, path));
                } else
                {
                    Error_LogLoadError(TRUE, Error_Format("%d\t%s", Error_LoadError_RMTexture, path));
                    Error_Warn(TRUE, Error_Format("Cannot open file %s", path));
                }
            }
        }
        //#endif // CONTAINER_USEOWNTEXTURELOAD

        free(newText);
    }

    return NULL;
}

void Container_Mesh_SetTexture(lpContainer cont, U32 groupID, lpContainer_Texture itext)
{
    LPDIRECT3DRMTEXTURE3 texture;
    LPDIRECT3DRMMESH mesh;
    LPDIRECT3DRMTEXTURE text1;
    HRESULT r;
    lpMesh transmesh;

    if (itext)
        texture = itext->texture;
    else
        texture = NULL;

    Container_DebugCheckOK(cont);

    Error_Fatal(cont->type != Container_Mesh && cont->type != Container_LWO, "Container_Mesh_SetTexture() called with non mesh object");

    transmesh = cont->typeData->transMesh;
    if (transmesh)
    {
        Error_Fatal(itext && itext->surface != NULL && itext->texture == NULL, "Texture is required to be created by Mesh_LoadTexture()");
        Mesh_SetGroupTexture(transmesh, groupID, itext);
    }
    else
    {
        Container_Mesh_DebugCheckOK(cont, groupID);

        mesh = cont->typeData->mesh;
        Error_Fatal(!mesh, "Container has no mesh object");

        Container_Mesh_HandleSeparateMeshGroups(&mesh, &groupID);

        if (texture)
        {
            r = texture->lpVtbl->QueryInterface(texture, &IID_IDirect3DRMTexture, &text1);
            Error_Fatal(r, "Unable to query texture1");
        }
        else
        {
            text1 = NULL;
        }

        if (mesh->lpVtbl->SetGroupTexture(mesh, groupID, text1) == D3DRM_OK)
        {
#ifndef CONTAINER_DISABLEFRAMESETTINGS
            cont->activityFrame->lpVtbl->SetMaterialMode(cont->activityFrame, D3DRMMATERIAL_FROMMESH);
#endif // CONTAINER_DISABLEFRAMESETTINGS
        }
        else
        {
            Error_Fatal(TRUE, "Unable to SetGroupTexture");
        }

        if (text1)
            text1->lpVtbl->Release(text1);
    }
}

U32 Container_Mesh_AddGroup(lpContainer cont, U32 vertexCount, U32 faceCount, U32 vPerFace, U32* faceData)
{
    U32 groupID;
    LPDIRECT3DRMMESH mesh;
    lpContainer_MeshAppData appdata;
    lpMesh transmesh;

    Container_DebugCheckOK(cont);

    Error_Fatal(cont->type != Container_Mesh, "Container_Mesh_AddGroup() called with non mesh object");

    transmesh = cont->typeData->transMesh;
    if (transmesh)
    {
        groupID = Mesh_AddGroup(transmesh, vertexCount, faceCount, vPerFace, faceData);

        return groupID;
    }
    else
    {
        Error_Fatal(vertexCount == 0, "Don't know if that will work!!!");

        mesh = cont->typeData->mesh;
        Error_Fatal(!mesh, "Container has no mesh object");

        appdata = (lpContainer_MeshAppData) mesh->lpVtbl->GetAppData(mesh);
        if (appdata)
        {
            if(!appdata->firstAddGroup)
            {
                // Make sure it doesn't crash
                if (appdata->usedCount == appdata->listSize)
                    return D3DRMGROUP_ALLGROUPS;

                // Using separate meshes instead of groups...
                if (lpD3DRM()->lpVtbl->CreateMesh(lpD3DRM(), &mesh) == D3DRM_OK)
                {
                    Container_NoteCreation(mesh);

                    appdata->meshList[appdata->usedCount++] = mesh;
                    cont->activityFrame->lpVtbl->AddVisual(cont->activityFrame, (struct IUnknown *) mesh);

                    {
                        // Set the msb if the group is added as a visual...
                        U32 value = appdata->usedCount;
                        value |= 0x80000000;
                        mesh->lpVtbl->SetAppData(mesh, value);
                    }

                    if (appdata->usedCount == appdata->listSize)
                    {
                        LPDIRECT3DRMMESH *newList;
                        newList = Mem_ReAlloc(appdata->meshList, sizeof(LPDIRECT3DRMMESH) * (appdata->listSize + CONTAINER_MESHGROUPBLOCKSIZE));
                        if (newList)
                        {
                            appdata->listSize += CONTAINER_MESHGROUPBLOCKSIZE;
                            appdata->meshList = newList;
                        }
                    }
                } else
                {
                    Error_Warn(TRUE, "Cannot add separated mesh as group");
                    return D3DRMGROUP_ALLGROUPS;
                }
            }
            else
            {
                appdata->firstAddGroup = FALSE;
            }
        }

        if (mesh->lpVtbl->AddGroup(mesh, vertexCount, faceCount, vPerFace, faceData, &groupID) == D3DRM_OK)
        {
            if (appdata)
                return appdata->usedCount;
            else
                return groupID;
        }
        else
        {
            Error_Fatal(TRUE, "Unable to AddGroup");
            return D3DRMGROUP_ALLGROUPS;
        }
    }

    return D3DRMGROUP_ALLGROUPS;
}

void Container_Mesh_HideGroup(lpContainer cont, U32 group, B32 hide)
{
    B32 hidden;
    LPDIRECT3DRMMESH mesh, groupmesh;
    LPDIRECT3DRMVISUAL visual;
    lpContainer_MeshAppData appdata;
    lpMesh transmesh;

    Container_DebugCheckOK(cont);
    Error_Fatal(cont->type != Container_Mesh, "Container_Mesh_HideGroup() called with non mesh object");
    Error_Fatal(!cont->typeData, "Container has no typeData");

    transmesh = cont->typeData->transMesh;
    if (transmesh)
    {
        Mesh_HideGroup(transmesh, group, hide);
    }
    else
    {
        mesh = cont->typeData->mesh;
        Error_Fatal(!mesh, "Container has no mesh object");

        Container_Mesh_DebugCheckOK(cont, group);

        appdata = (lpContainer_MeshAppData) mesh->lpVtbl->GetAppData(mesh);
        if (appdata)
        {
            if (group != 0)
            {
                groupmesh = appdata->meshList[group - 1];
                hidden = !(groupmesh->lpVtbl->GetAppData(groupmesh) & 0x80000000);
                visual = (LPDIRECT3DRMVISUAL) groupmesh;
                group = 0;
            }
            else
            {
                groupmesh = NULL;
                hidden = appdata->groupZeroHidden;
                visual = (LPDIRECT3DRMVISUAL) mesh;
            }

            if (hide && !hidden)
                cont->activityFrame->lpVtbl->DeleteVisual(cont->activityFrame, (struct IUnknown *) visual);
            else if (!hide && hidden)
                cont->activityFrame->lpVtbl->AddVisual(cont->activityFrame, (struct IUnknown *) visual);

            if (groupmesh)
            {
                U32 keep = groupmesh->lpVtbl->GetAppData(groupmesh) & 0x7fffffff;
                if (hide)
                    groupmesh->lpVtbl->SetAppData(groupmesh, keep);
                else
                    groupmesh->lpVtbl->SetAppData(groupmesh, keep | 0x80000000);
            }
            else
            {
                appdata->groupZeroHidden = hide;
            }
        }
        //else Error_Fatal(TRUE, "Wrong mesh type");
    }
}

void Container_Mesh_SetQuality(lpContainer cont, U32 groupID, Graphics_Quality quality)
{
    U32 d3drmqual;
    LPDIRECT3DRMMESH mesh;
    lpMesh transmesh;

    Error_Fatal(cont->type != Container_Mesh, "Container_Mesh_SetQuality() called with non mesh object");

    transmesh = cont->typeData->transMesh;
    if (transmesh)
    {
        Error_Warn(TRUE, "Not supported yet");
    }
    else
    {
        if (quality == Wireframe)
            d3drmqual = D3DRMRENDER_WIREFRAME;
        if (quality == UnlitFlat)
            d3drmqual = D3DRMRENDER_UNLITFLAT;
        if (quality == Flat)
            d3drmqual = D3DRMRENDER_FLAT;
        if (quality == Gouraud)
            d3drmqual = D3DRMRENDER_GOURAUD;

        Container_DebugCheckOK(cont);
        Error_Fatal(!cont->typeData, "Container has no typeData");

        mesh = cont->typeData->mesh;
        Error_Fatal(!mesh, "Container has no mesh object");

        Container_Mesh_DebugCheckOK(cont, groupID);

        Container_Mesh_HandleSeparateMeshGroups(&mesh, &groupID);
        cont->typeData->mesh->lpVtbl->SetGroupQuality(mesh, groupID, d3drmqual);
    }
}

void Container_Mesh_SetEmissive(lpContainer cont, U32 groupID, F32 r, F32 g, F32 b)
{
    lpMesh transmesh;

    Container_DebugCheckOK(cont);
    Error_Fatal(cont->type != Container_Mesh && cont->type != Container_LWO, "Container_Mesh_SetEmissive() called with non mesh object");

    transmesh = cont->typeData->transMesh;
    if (transmesh)
    {
        Mesh_SetGroupEmissive(transmesh, groupID, r, g, b);
    }
}

void Container_Mesh_SetColourAlpha(lpContainer cont, U32 groupID, F32 r, F32 g, F32 b, F32 a)
{
    D3DCOLOR colour;
    lpMesh transmesh;

    Container_DebugCheckOK(cont);
    Error_Fatal(cont->type != Container_Mesh && cont->type != Container_LWO, "Container_Mesh_SetColourAlpha() called with non mesh object");

    colour = Container_GetRGBAColour(r, g, b, a);

    transmesh = cont->typeData->transMesh;
    if (transmesh)
    {
        Mesh_SetGroupDiffuse(transmesh, groupID, r, g, b);
        Mesh_SetGroupAlpha(transmesh, groupID, a);
    }
    else
    {
#ifndef CONTAINER_DISABLEFRAMESETTINGS
        LPDIRECT3DRMMESH mesh = cont->typeData->mesh;
        Error_Fatal(!mesh, "Container has no mesh object");

        Container_Mesh_DebugCheckOK(cont, groupID);

        Container_Mesh_HandleSeparateMeshGroups(&mesh, &groupID);

        mesh->lpVtbl->SetGroupColor(mesh, groupID, colour);
        cont->activityFrame->lpVtbl->SetMaterialMode(cont->activityFrame, D3DRMMATERIAL_FROMMESH);
#endif
    }
}

void Container_Mesh_SetPerspectiveCorrection(lpContainer cont, U32 group, B32 on)
{
    LPDIRECT3DRMMESH mesh;
    lpMesh transmesh;

    Container_DebugCheckOK(cont);
    Error_Fatal(cont->type != Container_Mesh && cont->type != Container_LWO, "Container_Mesh_SetPerspectiveCorrection() called with non mesh object");

    transmesh = cont->typeData->transMesh;
    if (transmesh)
    {
        Error_Warn(TRUE, "Not yet implemented for immediate mode meshes");
    }
    else
    {
        mesh = cont->typeData->mesh;

        Error_Fatal(!mesh, "Container has no mesh object");

        Container_Mesh_DebugCheckOK(cont, groupID);

        Container_Mesh_HandleSeparateMeshGroups(&mesh, &group);

        if (on)
            mesh->lpVtbl->SetGroupMapping(mesh, group, D3DRMMAP_PERSPCORRECT);
        else
            mesh->lpVtbl->SetGroupMapping(mesh, group, 0);
    }
}

void Container_Mesh_Swap(lpContainer target, lpContainer origin, B32 restore)
{
    // If not restoring then move any visuals on the container onto its hidden frame
    // and add the mesh from the origin container onto the target container...
    // Otherwise, restore the original visuals...

    LPDIRECT3DRMMESH mesh;
    LPDIRECT3DRMVISUAL* visuals;
    LPDIRECT3DRMVISUAL visual;
    LPDIRECT3DRMFRAME3 frame;
    U32 count, loop;
    lpMesh transmesh;

    Container_DebugCheckOK(target);
    Error_Fatal(target->type != Container_Reference && target->type != Container_Mesh, "Container_Mesh_Swap() can only be used with a reference or mesh object as the 'target' container");
    Error_Fatal((target->flags & CONTAINER_FLAG_MESHSWAPPED) && !restore, "Container_Mesh_Swap() called without restoring previous swap");
    Error_Fatal(!(target->flags & CONTAINER_FLAG_MESHSWAPPED) && restore, "Container_Mesh_Swap() called with restore without a previous swap");

    if (target->type == Container_Reference)
        frame = target->masterFrame;
    else
        frame = target->activityFrame;

    if (!restore) // Move all the existing visuals onto the hidden frame...
    {
        frame->lpVtbl->GetVisuals(frame, &count, NULL);
        if (count)
        {
            Error_Fatal(count >= CONTAINER_MAXVISUALS, "CONTAINER_MAXVISUALS too small");
            visuals = containerGlobs.visualArray;
            frame->lpVtbl->GetVisuals(frame, &count, (struct IUnknown**) visuals);

            //Error_Debug(Error_Format("Moving %i visuals to the hidden frame\n", count));
            for (loop = 0; loop < count; loop++)
            {
                visual = visuals[loop];

                target->hiddenFrame->lpVtbl->AddVisual(target->hiddenFrame, (struct IUnknown*) visual);
                frame->lpVtbl->DeleteVisual(frame, (struct IUnknown*) visual);
            }
        }

        if (origin)
        {
            //Error_Fatal(origin->type != Container_Mesh, "Container_Mesh_Swap() called with non-mesh object as 'origin' container");
            if ((mesh = origin->typeData->mesh) == NULL)
            {
                transmesh = origin->typeData->transMesh;
                Error_Fatal(transmesh == NULL, "Container has no mesh object");
                frame->lpVtbl->AddVisual(frame, (struct IUnknown*) transmesh->uv);
            }
            else
            {
                Error_Fatal(mesh->lpVtbl->GetAppData(mesh), "Not yet supported with separate mesh groups");

                // Add the origin's mesh as the new visual
                frame->lpVtbl->AddVisual(frame, (struct IUnknown*) mesh);
            }
        }

        target->flags |= CONTAINER_FLAG_MESHSWAPPED;
    }
    else
    {
        // Delete the visual (will still be used by the origin container) then restore the original visuals...

        frame->lpVtbl->GetVisuals(frame, &count, NULL);
        if (count)
        {
            Error_Fatal(count >= CONTAINER_MAXVISUALS, "CONTAINER_MAXVISUALS too small");
            visuals = containerGlobs.visualArray;
            frame->lpVtbl->GetVisuals(frame, &count, (struct IUnknown**) visuals);

            //Error_Debug(Error_Format("Deleting %i visuals...\n", count));
            for (loop = 0; loop < count; loop++)
            {
                visual = visuals[loop];
                frame->lpVtbl->DeleteVisual(frame, (struct IUnknown*) visual);
            }
        }

        target->hiddenFrame->lpVtbl->GetVisuals(target->hiddenFrame, &count, NULL);
        if (count)
        {
            Error_Fatal(count >= CONTAINER_MAXVISUALS, "CONTAINER_MAXVISUALS too small");
            visuals = containerGlobs.visualArray;
            target->hiddenFrame->lpVtbl->GetVisuals(target->hiddenFrame, &count, (struct IUnknown**) visuals);

            //Error_Debug(Error_Format("Restoring %i visuals from the hidden frame...\n", count));
            for (loop = 0; loop < count; loop++)
            {
                visual = visuals[loop];
                frame->lpVtbl->AddVisual(frame, (struct IUnknown*) visual);
                target->hiddenFrame->lpVtbl->DeleteVisual(target->hiddenFrame, (struct IUnknown*) visual);
            }
        }

        target->flags &= ~CONTAINER_FLAG_MESHSWAPPED;
    }
}

void Container_EnableFog(B32 on)
{
    containerGlobs.rootContainer->masterFrame->lpVtbl->SetSceneFogEnable(containerGlobs.rootContainer->masterFrame, on);

    if (on)
        containerGlobs.rootContainer->masterFrame->lpVtbl->SetSceneBackground(containerGlobs.rootContainer->masterFrame, containerGlobs.fogColor);
    else
        containerGlobs.rootContainer->masterFrame->lpVtbl->SetSceneBackground(containerGlobs.rootContainer->masterFrame, 0);
}

void Container_SetFogColour(F32 r, F32 g, F32 b)
{
    containerGlobs.fogColor = Container_GetRGBColour(r, g, b);
    containerGlobs.rootContainer->masterFrame->lpVtbl->SetSceneFogColor(containerGlobs.rootContainer->masterFrame, containerGlobs.fogColor);
}

void Container_GetFogColour(F32* r, F32* g, F32* b)
{
    *r = RGB_GETRED(containerGlobs.fogColor) / 255.0f;
    *g = RGB_GETGREEN(containerGlobs.fogColor) / 255.0f;
    *b = RGB_GETBLUE(containerGlobs.fogColor) / 255.0f;
}

void Container_SetFogMode(U32 mode)
{
    containerGlobs.rootContainer->masterFrame->lpVtbl->SetSceneFogMode(containerGlobs.rootContainer->masterFrame, mode);

    // 3Dfx requires table fog and software requires vertex fog...
    containerGlobs.rootContainer->masterFrame->lpVtbl->SetSceneFogMethod(containerGlobs.rootContainer->masterFrame, Main_GetFogMethod());
}

void Container_SetFogParams(F32 start, F32 end, F32 density)
{
    containerGlobs.rootContainer->masterFrame->lpVtbl->SetSceneFogParams(containerGlobs.rootContainer->masterFrame, start, end, density);
}

B32 Container_Mesh_HandleSeparateMeshGroups(LPDIRECT3DRMMESH *mesh, U32* group)
{
    lpContainer_MeshAppData appdata = (lpContainer_MeshAppData) (*mesh)->lpVtbl->GetAppData(*mesh);
    if (appdata)
    {
        // Leave the default mesh if the group is zero and the (group-1)'th off the
        // list if not...

        if ((*group) != 0)
            *mesh = appdata->meshList[(*group)-1];
        (*group) = 0;

        return TRUE;
    }

    return FALSE;
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

void Container_YFlipTexture(LPDIRECT3DRMTEXTURE3 texture)
{
    LPD3DRMIMAGE image;
    char* buffer, *topline, *bottomline;
    S32 y, byteWidth;

    if ((image = texture->lpVtbl->GetImage(texture)))
    {
        byteWidth = (image->depth * image->width) / 8;
        if ((buffer = Mem_Alloc(byteWidth)))
        {
            topline = (char*) image->buffer1;
            bottomline = &((char*) image->buffer1)[(image->height - 1) * image->bytes_per_line];

            for (y = 0; y < image->height / 2; y++)
            {
                memcpy(buffer, bottomline, byteWidth);
                memcpy(bottomline, topline, byteWidth);
                memcpy(topline, buffer, byteWidth);
                topline += image->bytes_per_line;
                bottomline -= image->bytes_per_line;
            }

            Mem_Free(buffer);
        }
        else
        {
            Error_Warn(TRUE, "Cannot allocate buffer for yflip");
        }
    }
}

S32 __cdecl Container_TextureSetSort(const void* a, const void* b)
{
    if (((lpContainer_TextureRef) a)->fileName != NULL && ((lpContainer_TextureRef) b)->fileName == NULL)
        return -1;
    if (((lpContainer_TextureRef) a)->fileName == NULL && ((lpContainer_TextureRef) b)->fileName != NULL)
        return 1;

    return 0;
}

void Container_TextureDestroyCallback(LPDIRECT3DRMOBJECT lpD3DRMobj, void* lpArg)
{
    lpContainer_TextureRef textRef = lpArg;
    lpContainer_Texture text;

    Error_Debug(Error_Format("Removing %s from texture list\n", textRef->fileName));

    text = (lpContainer_Texture) textRef->texture->lpVtbl->GetAppData(textRef->texture);
    // This is deliberately not Mem_Free()
    free(text);

    Mem_Free(textRef->fileName);
    textRef->fileName = NULL;
    textRef->texture = NULL;
}

HRESULT Container_TextureLoadCallback(const char* name, void* data, LPDIRECT3DRMTEXTURE3 *texture)
{
    lpContainer_TextureData textureData = data;
    lpContainer_Texture text;

    if (!(textureData->flags & CONTAINER_TEXTURE_NOLOAD))
    {
        char path[1024];
        const char *s;
        char *p;
        char *tag = NULL;

        U32 loop;
        S32 location;

        s = textureData->xFileName;
        p = path;

        for (location = 0; location < (containerGlobs.sharedDir ? 2 : 1); location++)
        {
            if (location == 0)
            {
                // Strip the xfilename off the end of the path...
                while (*s != '\0')
                {
                    if (*s == '\\')
                    {
                        if (tag)
                            *tag = '\\';
                        *p = '\0';
                        tag = p;
                    }
                    else
                    {
                        *p = *s;
                    }
                    s++;
                    p++;
                }

                if (tag)
                {
                    strcat(path, "\\");
                    strcat(path, name);
                }
                else
                {
                    strcpy(path, name);
                }
            }
            else
            {
                sprintf(path, "%s\\%s", containerGlobs.sharedDir, name);
            }

            *texture = NULL;

            for (loop = 0; loop < containerGlobs.textureCount; loop++)
            {
                if (containerGlobs.textureSet[loop].fileName && (strcmp(containerGlobs.textureSet[loop].fileName, path) == 0))
                {
                    *texture = containerGlobs.textureSet[loop].texture;
                    (*texture)->lpVtbl->AddRef(*texture);

                    return D3DRM_OK;
                }
            }

            if ((text = Container_LoadTexture(path, NULL, NULL)))
            {
                *texture = text->texture;

#ifndef CONTAINER_DONTFLIPTEXTURES
                Container_YFlipTexture(*texture);
#endif

                if (Main_MIPMapEnabled())
                {
                    HRESULT r = (*texture)->lpVtbl->GenerateMIPMap(*texture, 0);
                }

                if (containerGlobs.textureCount == CONTAINER_MAXTEXTURES)
                {
                    Error_Warn(TRUE, "Sorting texture list");
                    qsort(containerGlobs.textureSet, containerGlobs.textureCount, sizeof(Container_TextureRef), Container_TextureSetSort);
                    for (; containerGlobs.textureCount >= 0; containerGlobs.textureCount--)
                    {
                        if (containerGlobs.textureSet[containerGlobs.textureCount - 1].fileName)
                            break;
                    }
                    Error_Fatal(containerGlobs.textureCount == CONTAINER_MAXTEXTURES, "CONTAINER_MAXTEXTURES overflowed");
                }

                if (containerGlobs.textureCount < CONTAINER_MAXTEXTURES)
                {
                    lpContainer_TextureRef textRef = &containerGlobs.textureSet[containerGlobs.textureCount];

                    (*texture)->lpVtbl->AddDestroyCallback(*texture, Container_TextureDestroyCallback, textRef);

                    textRef->fileName = Mem_Alloc(strlen(path) + 1);
                    strcpy(textRef->fileName, path);
                    textRef->texture = *texture;

                    containerGlobs.textureCount++;
                }

                return D3DRM_OK;
            }
        }
    }

    return D3DRMERR_NOTFOUND;
}

B32 Container_FrameLoad(const char* fname, LPDIRECT3DRMFRAME3 frame)
{
    B32 res = FALSE;
    D3DRMLOADMEMORY buffer;
    Container_TextureData tData;

    if ((buffer.lpMemory = File_LoadBinary(fname, &buffer.dSize)))
    {
        tData.xFileName = fname;
        tData.flags = 0;

        if (frame->lpVtbl->Load(frame, &buffer, NULL, D3DRMLOAD_FROMMEMORY, Container_TextureLoadCallback, &tData) == D3DRM_OK)
        // if (frame->lpVtbl->Load(frame, &buffer, NULL, D3DRMLOAD_FROMMEMORY, NULL, NULL) == D3DRM_OK)
        {
            res = TRUE;
        }
        Mem_Free(buffer.lpMemory);
    }

    return res;
}

LPDIRECT3DRMMESH Container_MeshLoad(void* file_data, U32 file_size, const char* file_name, LPDIRECT3DRMFRAME3 frame, B32 noTexture)
{
    LPDIRECT3DRMMESHBUILDER3 mb;
    LPDIRECT3DRMMESH mesh;
    HRESULT r;
    D3DRMLOADMEMORY buffer;
    Container_TextureData tData;

    buffer.lpMemory = file_data;
    buffer.dSize = file_size;

    if (lpD3DRM()->lpVtbl->CreateMeshBuilder(lpD3DRM(), &mb) == D3DRM_OK)
    {
        Container_NoteCreation(mb);

        tData.xFileName = file_name;
        tData.flags = 0;

        if (noTexture)
            tData.flags |= CONTAINER_TEXTURE_NOLOAD;

        if (mb->lpVtbl->Load(mb, &buffer, NULL, D3DRMLOAD_FROMMEMORY, Container_TextureLoadCallback, &tData))
        //if (mb->lpVtbl->Load(mb, &buffer, NULL, D3DRMLOAD_FROMMEMORY, NULL, NULL))
        {
            mb->lpVtbl->CreateMesh(mb, &mesh);
            Container_NoteCreation(mesh);
            r = mb->lpVtbl->Release(mb);
            if (frame->lpVtbl->AddVisual(frame, (struct IUnknown*) mesh) == D3DRM_OK)
            {
                return mesh;
            }
            else
            {
                Error_Fatal(TRUE, "Unable to add visual to frame");
            }
        }
        else
        {
            Error_Fatal(TRUE, Error_Format("Unable to load MeshBuilder from memory (%s)", file_name));
        }
        r = mb->lpVtbl->Release(mb);
    }
    else
    {
        Error_Fatal(TRUE, "Unable to create MeshBuilder");
    }

    return NULL;
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
        if ((buffer.lpMemory = File_LoadBinary(fname, &buffer.dSize)))
        {
            if (lpD3DRM()->lpVtbl->CreateFrame(lpD3DRM(), frame, &rootFrame) == D3DRM_OK)
            {
                Container_NoteCreation(rootFrame);
                if (lpD3DRM()->lpVtbl->CreateAnimationSet(lpD3DRM(), &animSet) == D3DRM_OK)
                {
                    Container_NoteCreation(animSet);

                    tData.xFileName = fname;
                    tData.flags = 0;

                    if (animSet->lpVtbl->Load(animSet, &buffer, NULL, D3DRMLOAD_FROMMEMORY, Container_TextureLoadCallback, &tData, rootFrame) == D3DRM_OK)
                    {
                        animSet->lpVtbl->SetTime(animSet, 0.0f);

                        fc = Container_GetAnimFileFrameCount(buffer.lpMemory);
                        if (frameCount)
                            *frameCount = fc;

                        animClone = AnimClone_Register(animSet, rootFrame, fc);
                    }

                    Mem_Free(buffer.lpMemory);
                }
                rootFrame->lpVtbl->Release(rootFrame);
            }
        }
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
