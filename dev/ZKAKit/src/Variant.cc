/* -------------------------------------------

	Copyright (C) 2024, Amlal EL Mahrouss, all rights reserved.

------------------------------------------- */

#include <NewKit/Variant.h>

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
