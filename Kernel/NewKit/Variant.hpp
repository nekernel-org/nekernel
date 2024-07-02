/* -------------------------------------------

	Copyright Zeta Electronics Corporation

------------------------------------------- */

#pragma once

#include <NewKit/Defines.hpp>
#include <NewKit/String.hpp>

namespace Kernel
{
	class Variant final
	{
	public:
		enum class VariantKind
		{
			kString,
			kPointer,
			kUndefined
		};

	public:
		explicit Variant() = delete;

	public:
		Variant& operator=(const Variant&) = default;
		Variant(const Variant&)			   = default;

		~Variant() = default;

	public:
		explicit Variant(StringView* stringView)
			: fPtr((voidPtr)stringView), fKind(VariantKind::kString)
		{
		}
		explicit Variant(nullPtr)
			: fPtr(nullptr), fKind(VariantKind::kUndefined)
		{
		}
		explicit Variant(voidPtr ptr)
			: fPtr(ptr), fKind(VariantKind::kPointer)
		{
		}

	public:
		const Char* ToString();

	private:
		voidPtr		fPtr{nullptr};
		VariantKind fKind{VariantKind::kUndefined};
	};
} // namespace Kernel
