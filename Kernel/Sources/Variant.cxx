/* -------------------------------------------

	Copyright SoftwareLabs

------------------------------------------- */

#include <NewKit/Variant.hpp>

namespace NewOS
{
	const Char* Variant::ToString()
	{
		switch (fKind)
		{
		case VariantKind::kString:
			return ("Class:{String}");
		case VariantKind::kPointer:
			return ("Memory:{Pointer}");
		default:
			return ("Memory:{Undefined}");
		}
	}

} // namespace NewOS
