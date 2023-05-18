#pragma once

#include "types.h"

typedef struct Container
{
    void* temp;
    // TODO: Implement Container
} Container, *lpContainer;

extern lpContainer Container_Initialize(const char* gameName);
extern void Container_Shutdown();
