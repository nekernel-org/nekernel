/* -------------------------------------------

	Copyright EL Mahrouss Logic.

------------------------------------------- */

#pragma once

#define STBI_NO_STDIO		  1
#define STBI_NO_SIMD		  1
#define STBI_NO_THREAD_LOCALS 1
#define STBI_NO_LINEAR		  1

#define STBI_ONLY_PNG  1
#define STBI_ONLY_JPEG 1
#define STBI_ONLY_BMP  1
#define STBI_ONLY_GIF  1

#define STBI_ASSERT(x)			 MUST_PASS(x)
#define STBI_MALLOC(x)			 Kernel::mm_new_heap(x, true, true)
#define STBI_REALLOC(p, x)		 Kernel::mm_realloc_heap(p, x);
#define STBI_FREE(x)			 Kernel::mm_delete_heap(x)
#define STB_IMAGE_IMPLEMENTATION 1

#include <KernelKit/Heap.h>

#include <BootKit/Vendor/stb_image.h>
