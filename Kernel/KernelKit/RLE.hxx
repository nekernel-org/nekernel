/* -------------------------------------------

	Copyright ZKA Technologies

------------------------------------------- */

#ifndef __KERNELKIT_RLE_HXX__
#define __KERNELKIT_RLE_HXX__

#include <NewKit/Defines.hpp>

EXTERN_C void rle_compress(void* data, long sz, void* out, long out_sz);
EXTERN_C void rle_decompress(void* data, long sz, void* out, long out_sz);

#endif // !ifndef __KERNELKIT_RLE_HXX__
