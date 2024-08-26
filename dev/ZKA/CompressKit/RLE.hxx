/* -------------------------------------------

	Copyright ZKA Technologies.

------------------------------------------- */

#ifndef __KERNELKIT_RLE_HXX__
#define __KERNELKIT_RLE_HXX__

#include <NewKit/Defines.hxx>

namespace Kernel
{
	EXTERN_C Void rle_compress(VoidPtr data, Size sz, VoidPtr out, Size out_sz);
	EXTERN_C Void rle_decompress(VoidPtr data, Size sz, VoidPtr out, Size out_sz);
} // namespace Kernel

#endif // !ifndef __KERNELKIT_RLE_HXX__
