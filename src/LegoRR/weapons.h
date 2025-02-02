#pragma once

#include "platform.h"
#include "config.h"

extern B32 Weapon_Initialize(lpConfig config, const char* gameName);

// Update Projectile and Lazer shots.
extern void Weapon_Update(F32 elapsedGame);
