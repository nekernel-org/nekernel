/* -------------------------------------------

	Copyright ZKA Technologies.

	FILE: main.cxx
	PURPOSE: ZWM entrypoint.

------------------------------------------- */

#include <ddk/KernelStd.h>

static uint32_t					   cGPUIndexHW = 0U;
static struct DDK_OBJECT_MANIFEST* cGPUObject  = nullptr;

/** @brief ZWM main, ZWM acts a proxy to the Installed GPU Driver. */
int32_t ModuleMain(void)
{
	int							index_gpu  = 0;
	struct DDK_OBJECT_MANIFEST* gpu_object = nullptr;

	while (!gpu_object)
	{
		gpu_object = KernelGetObject(index_gpu, "GPU_OBJECT");
		++index_gpu;
	}

	cGPUIndexHW = index_gpu;
	cGPUObject	= gpu_object;

	return 0;
}
