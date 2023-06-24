#pragma once

#include "platform.h"

#define RNC_COMPRESS_BEST 1
#define RNC_COMPRESS_FAST 2
#define COMPRESSION_UNPACK_BUFFER_EXTEND 20000

extern U32 RNC_Compress(S32 mode, void* bufferIn, U32 length, void** bufferOut);
extern U32 RNC_Uncompress(void* bufferIn, void** bufferOut);
extern S32 RNC_UncompressFast(void* input, void* output, U32 len);
