#include "material.h"
#include "error.h"
#include "main.h"

lpMaterial Material_Create(F32 emissiveRed, F32 emissiveGreen, F32 emissiveBlue, F32 specularRed, F32 specularGreen, F32 specularBlue, F32 power)
{
    LPDIRECT3DRMMATERIAL2 mat;

    if (lpD3DRM()->lpVtbl->CreateMaterial(lpD3DRM(), power, &mat) == D3DRM_OK)
    {
        mat->lpVtbl->SetEmissive(mat, emissiveRed, emissiveGreen, emissiveBlue);
        mat->lpVtbl->SetSpecular(mat, specularRed, specularGreen, specularBlue);

        return (lpMaterial) mat;
    }

    return NULL;
}

void Material_Remove(lpMaterial material)
{
    LPDIRECT3DRMMATERIAL mat = (LPDIRECT3DRMMATERIAL) material;

    Error_Fatal(!mat, "Null passed as material to Material_Remove()");

    mat->lpVtbl->Release(mat);
}

void Material_SetEmissive(lpMaterial material, F32 emissiveRed, F32 emissiveGreen, F32 emissiveBlue)
{
    LPDIRECT3DRMMATERIAL mat = (LPDIRECT3DRMMATERIAL) material;

    Error_Fatal(!mat, "Null passed as material to Material_SetEmissive()");

    mat->lpVtbl->SetEmissive(mat, emissiveRed, emissiveGreen, emissiveBlue);
}

void Material_SetSpecular(lpMaterial material, F32 specularRed, F32 specularGreen, F32 specularBlue, F32 power)
{
    LPDIRECT3DRMMATERIAL mat = (LPDIRECT3DRMMATERIAL) material;

    Error_Fatal(!mat, "Null passed as material to Material_SetSpecular()");

    mat->lpVtbl->SetPower(mat, power);
    mat->lpVtbl->SetSpecular(mat, specularRed, specularGreen, specularBlue);
}
