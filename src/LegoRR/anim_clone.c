#include "anim_clone.h"

#include "error.h"
#include "mem.h"

lpAnimClone AnimClone_Register(LPDIRECT3DRMANIMATIONSET2 animSet, LPDIRECT3DRMFRAME3 root, U32 frameCount)
{
    AnimClone data;
    lpAnimClone orig = Mem_Alloc(sizeof(AnimClone));

    orig->clonedFrom = NULL;
    orig->animSet = animSet;
    orig->scene = NULL;
    orig->lws = FALSE;

    orig->root = root;
    orig->root->lpVtbl->AddRef(orig->root);
    orig->partCount = 0;
    orig->frameCount = frameCount;

    AnimClone_WalkTree(root, 0, AnimClone_FrameCountCallback, &orig->partCount);
    orig->partArray = Mem_Alloc(sizeof(LPDIRECT3DRMFRAME3) * orig->partCount);

    data.partArray = orig->partArray;
    data.partCount = 0;
    AnimClone_WalkTree(root, 0, AnimClone_SetupFrameArrayCallback, &data);

    return orig;
}

lpAnimClone AnimClone_RegisterLws(lpLws_Info scene, LPDIRECT3DRMFRAME3 root, U32 frameCount)
{
    AnimClone data;
    lpAnimClone orig = Mem_Alloc(sizeof(AnimClone));

    orig->clonedFrom = NULL;
    orig->animSet = NULL;
    orig->scene = scene;
    orig->lws = TRUE;

    orig->root = root;
    orig->root->lpVtbl->AddRef(orig->root);
    orig->partCount = 0;
    orig->frameCount = frameCount;

    AnimClone_WalkTree(root, 0, AnimClone_FrameCountCallback, &orig->partCount);
    orig->partArray = Mem_Alloc(sizeof(LPDIRECT3DRMFRAME3) * orig->partCount);

    data.partArray = orig->partArray;
    data.partCount = 0;
    AnimClone_WalkTree(root, 0, AnimClone_SetupFrameArrayCallback, &data);

    return orig;
}

B32 AnimClone_WalkTree(LPDIRECT3DRMFRAME3 frame, U32 level, B32 (*Callback)(LPDIRECT3DRMFRAME3 frame, void* data), void* data)
{
    LPDIRECT3DRMFRAMEARRAY children;
    LPDIRECT3DRMFRAME3 child;
    LPDIRECT3DRMFRAME child1;
    U32 count, loop;
    B32 finished = FALSE;
    HRESULT r;

    if (Callback(frame, data))
        return TRUE;

    if (frame->lpVtbl->GetChildren(frame, &children) == D3DRM_OK)
    {
        count = children->lpVtbl->GetSize(children);
        for (loop = 0; loop < count; loop++)
        {
            children->lpVtbl->GetElement(children, loop, &child1);
            child1->lpVtbl->QueryInterface(child1, &IID_IDirect3DRMFrame3, &child);
            child1->lpVtbl->Release(child1);
            if (AnimClone_WalkTree(child, level + 1, Callback, data))
            {
                finished = TRUE;
                r = child->lpVtbl->Release(child);
                break;
            }

            r = child->lpVtbl->Release(child);
        }

        r = children->lpVtbl->Release(children);
    }
    return finished;
}

B32 AnimClone_FrameCountCallback(LPDIRECT3DRMFRAME3 frame, void* data)
{
    // Count the total number of frames within a given hierarchy...

    U32* count = data;
    (*count)++;
    return FALSE; // Don't exit loop...
}

B32 AnimClone_SetupFrameArrayCallback(LPDIRECT3DRMFRAME3 frame, void* p)
{
    // Build the hierarchy into a table by building an array of pointers to each frame...

    lpAnimClone data = p;
    data->partArray[data->partCount++] = frame;
    return FALSE;
}

void AnimClone_SetTime(lpAnimClone clone, F32 time, F32* oldTime)
{
    lpAnimClone orig = clone->clonedFrom;

    if (orig)
    {
        LPDIRECT3DRMFRAME3 parent;
        D3DRMMATRIX4D mat;
        U32 loop;

        // Set time on the original animation then copy all of the transformations to the clone...
        if (orig->lws)
            Lws_SetTime(orig->scene, time);
        else
            orig->animSet->lpVtbl->SetTime(orig->animSet, time);

        for (loop = 0; loop < clone->partCount; loop++)
        {
            orig->partArray[loop]->lpVtbl->GetParent(orig->partArray[loop], &parent);
            orig->partArray[loop]->lpVtbl->GetTransform(orig->partArray[loop], parent, mat);
            parent->lpVtbl->Release(parent);
            clone->partArray[loop]->lpVtbl->AddTransform(clone->partArray[loop], D3DRMCOMBINE_REPLACE, mat);
        }

        // Restore the original animations' time if required...
        if (oldTime)
        {
            if (orig->lws)
                Lws_SetTime(orig->scene, *oldTime);
            else
                orig->animSet->lpVtbl->SetTime(orig->animSet, *oldTime);
        }
    }
    else
    {
        // 'clone' is actually the original...
        if (clone->lws)
            Lws_SetTime(clone->scene, time);
        else
            clone->animSet->lpVtbl->SetTime(clone->animSet, time);
    }
}

void AnimClone_CreateCopy(LPDIRECT3DRMFRAME3 orig, LPDIRECT3DRMFRAME3 clone, B32 lws)
{
    LPDIRECT3DRMFRAMEARRAY children;
    LPDIRECT3DRMFRAME3 parent;
    U32 count, loop;
    const char* name;
    U32 length;
    Matrix4F mat;

    // Link in the visuals...
    if (lws)
        AnimClone_CloneLwsMesh(orig, clone);
    else
        AnimClone_ReferenceVisuals(orig, clone);

    // Copy the frame name...
    orig->lpVtbl->GetName(orig, &length, NULL);
    if (length)
    {
        name = Mem_Alloc(length);
        orig->lpVtbl->GetName(orig, &length, name);
        clone->lpVtbl->SetName(clone, name);
        Mem_Free(name);
    }

    // Copy the transformation...
    orig->lpVtbl->GetParent(orig, &parent);
    orig->lpVtbl->GetTransform(orig, parent, mat);
    parent->lpVtbl->Release(parent);
    clone->lpVtbl->AddTransform(clone, D3DRMCOMBINE_REPLACE, mat);

    // Do exactly the same for each child of the frame...
    if (orig->lpVtbl->GetChildren(orig, &children) == D3DRM_OK)
    {
        count = children->lpVtbl->GetSize(children);
        for (loop = 0; loop < count; loop++)
        {
            LPDIRECT3DRMFRAME3 newFrame, childFrame;
            LPDIRECT3DRMFRAME child1;

            children->lpVtbl->GetElement(children, loop, &child1);
            child1->lpVtbl->QueryInterface(child1, &IID_IDirect3DRMFrame3, &childFrame);
            child1->lpVtbl->Release(child1);

            // All 'childFrame' are children of 'orig' thus all 'newFrame' should be children of 'clone'
            lpD3DRM()->lpVtbl->CreateFrame(lpD3DRM(), clone, &newFrame);
            Container_NoteCreation(newFrame);

            AnimClone_CreateCopy(childFrame, newFrame, lws);

            newFrame->lpVtbl->Release(newFrame);
            childFrame->lpVtbl->Release(childFrame);
        }
        children->lpVtbl->Release(children);
    }
}

void AnimClone_CloneLwsMesh(LPDIRECT3DRMFRAME3 orig, LPDIRECT3DRMFRAME3 clone)
{
    struct IUnknown *iunknown[ANIMCLONE_MAXVISUALS];
    LPDIRECT3DRMUSERVISUAL uv;
    lpMesh mesh;
    U32 count, loop;

    orig->lpVtbl->GetVisuals(orig, &count, NULL);
    if (count)
    {
        Error_Fatal(count >= ANIMCLONE_MAXVISUALS, "ANIMCLONE_MAXVISUALS too small");
        orig->lpVtbl->GetVisuals(orig, &count, iunknown);
        for (loop = 0; loop < count; loop++)
        {
            if (iunknown[loop]->lpVtbl->QueryInterface(iunknown[loop], &IID_IDirect3DRMUserVisual, &uv) == D3DRM_OK)
            {
                mesh = (lpMesh) uv->lpVtbl->GetAppData(uv);
                Error_Fatal(mesh == NULL, "Cannot get mesh");
                Mesh_Clone(mesh, clone);
            }
        }
    }
}

void AnimClone_ReferenceVisuals(LPDIRECT3DRMFRAME3 orig, LPDIRECT3DRMFRAME3 clone)
{
    //LPDIRECT3DRMVISUAL *visuals;
    LPDIRECT3DRMVISUAL visual;
    U32 count, loop;

    // Cut down on the Mem_Alloc()s
    LPDIRECT3DRMVISUAL visuals[ANIMCLONE_MAXVISUALS];

    orig->lpVtbl->GetVisuals(orig, &count, NULL);
    if (count)
    {
        Error_Fatal(count >= ANIMCLONE_MAXVISUALS, "ANIMCLONE_MAXVISUALS too small");
        //visuals = Mem_Alloc(sizeof(LPDIRECT3DRMVISUAL) * count);
        orig->lpVtbl->GetVisuals(orig, &count, (struct IUnknown **) visuals);

        for (loop = 0; loop < count; loop++)
        {
            visual = visuals[loop];
            clone->lpVtbl->AddVisual(clone, (struct IUnknown*) visual);
        }
    }
}

lpAnimClone AnimClone_Make(lpAnimClone orig, LPDIRECT3DRMFRAME3 parent, U32* frameCount)
{
    lpAnimClone clone = Mem_Alloc(sizeof(AnimClone));
    AnimClone data;

    // Setup from the first frame...
    if (orig->lws)
    {
        //Lws_SetTime(orig->scene, 0.0f);
        *clone = *orig;
        clone->scene = Lws_Clone(orig->scene, parent);
        clone->clonedFrom = NULL;
        if (frameCount)
            *frameCount = orig->frameCount;
    }
    else
    {
        memset(clone, 0, sizeof(AnimClone));
        orig->animSet->lpVtbl->SetTime(orig->animSet, 0.0f);
        clone->animSet = NULL;

        clone->clonedFrom = orig;
        lpD3DRM()->lpVtbl->CreateFrame(lpD3DRM(), parent, &clone->root);
        Container_NoteCreation(clone->root);
        clone->partArray = Mem_Alloc(sizeof(LPDIRECT3DRMFRAME3) * orig->partCount);

        AnimClone_CreateCopy(orig->root, clone->root, orig->lws);
        clone->partCount = orig->partCount;
        if (frameCount)
            *frameCount = orig->frameCount;

        data.partArray = clone->partArray;
        data.partCount = 0;
        AnimClone_WalkTree(clone->root, 0, AnimClone_SetupFrameArrayCallback, &data);
    }

    return clone;

}
