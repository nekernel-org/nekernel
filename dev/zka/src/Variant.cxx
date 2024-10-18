/* -------------------------------------------

	Copyright ZKA Web Services Co.

------------------------------------------- */

#include <NewKit/Variant.hxx>

namespace Kernel
{
	const Char* Variant::ToString()
	{
		switch (fKind)
		{
		case VariantKind::kXML:
			return ("Class:{XML}");
		case VariantKind::kJson:
			return ("Class:{Json}");
		case VariantKind::kString:
			return ("Class:{String}");
		case VariantKind::kBlob:
			return ("Class:{Blob}");
		default:
			return ("Class:{Null}");
		}
	}

	/// @brief Leak variant's instance.
	VoidPtr Variant::Leak()
	{
		return fPtr;
	}
} // namespace Kernel
