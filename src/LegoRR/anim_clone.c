#include "anim_clone.h"
#include "mem.h"

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
