/* -------------------------------------------

	Copyright ZKA Technologies.

------------------------------------------- */

#pragma once

#include <NewKit/Defines.hxx>
#include <NewKit/String.hxx>
#include <NewKit/Json.hxx>

namespace Kernel
{
	class Variant final
	{
	public:
		enum class VariantKind
		{
			kString,
			kBlob,
			kNull,
			kJson,
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

		explicit Variant(JsonType* json)
			: fPtr((voidPtr)json), fKind(VariantKind::kJson)
		{
		}

		explicit Variant(nullPtr)
			: fPtr(nullptr), fKind(VariantKind::kNull)
		{
		}

		explicit Variant(voidPtr ptr)
			: fPtr(ptr), fKind(VariantKind::kBlob)
		{
		}

	public:
		const Char* ToString();
		VoidPtr		Leak();

	private:
		voidPtr		fPtr{nullptr};
		VariantKind fKind{VariantKind::kNull};
	};
} // namespace Kernel
