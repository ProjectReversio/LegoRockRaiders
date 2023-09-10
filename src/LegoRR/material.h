#pragma once

#include "common.h"

#include <d3drm.h>

typedef struct Material_Dummy { U32 dummy; } *lpMaterial;

extern lpMaterial Material_Create(F32 emissiveRed, F32 emissiveGreen, F32 emissiveBlue, F32 specularRed, F32 specularGreen, F32 specularBlue, F32 power);
extern void Material_Remove(lpMaterial material);
extern void Material_SetEmissive(lpMaterial material, F32 emissiveRed, F32 emissiveGreen, F32 emissiveBlue);
extern void Material_SetSpecular(lpMaterial material, F32 specularRed, F32 specularGreen, F32 specularBlue, F32 power);
