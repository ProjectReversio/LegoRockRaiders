#pragma once

#include "message.h"
#include "platform.h"

#define PTL_PROPERTY_TABLE_MAX 40

typedef struct PTL_Property
{
    Message_Type fromType; // The original "posted" event message type.
    Message_Type toType; // The output "translated" event message type.
} PTL_Property;

typedef struct PTL_Globs
{
    PTL_Property table[PTL_PROPERTY_TABLE_MAX];
    U32 count;
} PTL_Globs;

extern PTL_Globs ptlGlobs;

extern B32 PTL_Initialize(const char* filename, const char* gameName);
