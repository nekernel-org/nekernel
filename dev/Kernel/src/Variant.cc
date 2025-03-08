/* -------------------------------------------

	Copyright (C) 2024-2025, Amlal EL Mahrouss, all rights reserved.

------------------------------------------- */

#include <NewKit/Variant.h>

namespace NeOS
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
		case VariantKind::kNull:
			return ("Class:{Null}");
		default:
			return ("Class:{Unknown}");
		}
	}

	/// @brief Leak variant's instance.
	VoidPtr Variant::Leak()
	{
		return this->fPtr;
	}
} // namespace NeOS
