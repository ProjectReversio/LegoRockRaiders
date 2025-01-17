#pragma once

#include "platform.h"

typedef enum TeleportObjectType
{
    TELEPORT_SERVIVE_NONE          = 0,
    TELEPORT_SERVIVE_VEHICLE       = 0x1,
    TELEPORT_SERVIVE_MINIFIGURE    = 0x2,
    TELEPORT_SERVIVE_ROCKMONSTER   = 0x4,
    TELEPORT_SERVIVE_BUILDING      = 0x8,
    TELEPORT_SERVIVE_BOULDER       = 0x10,
    TELEPORT_SERVIVE_POWERCRYSTAL  = 0x20,
    TELEPORT_SERVIVE_ORE           = 0x40,
    TELEPORT_SERVIVE_DYNAMITE      = 0x80,
    TELEPORT_SERVIVE_BARRIER       = 0x100,
    TELEPORT_SERVIVE_UPGRADEPART   = 0x200,
    TELEPORT_SERVIVE_ELECTRICFENCE = 0x400,
    TELEPORT_SERVIVE_SPIDERWEB     = 0x800,
    TELEPORT_SERVIVE_OOHSCARY      = 0x1000,
} TeleportObjectType;

extern void Teleporter_Restart();
extern B32 Teleporter_ServiceAll(TeleportObjectType teleportObjTypes);
