/* -------------------------------------------

	Copyright ZKA Technologies

------------------------------------------- */

#include <NewKit/Variant.hpp>

namespace Kernel
{
	const Char* Variant::ToString()
	{
		switch (fKind)
		{
		case VariantKind::kString:
			return ("Class:{String}");
		case VariantKind::kPointer:
			return ("Class:{Pointer}");
		default:
			return ("Class:{Undefined}");
		}
	}

} // namespace Kernel
