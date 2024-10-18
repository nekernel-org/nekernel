/* -------------------------------------------

	Copyright ZKA Web Services Co.

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
			kXML,
		};

	public:
		explicit Variant() = delete;

	public:
		ZKA_COPY_DEFAULT(Variant);

		~Variant() = default;

	public:
		explicit Variant(StringView* stringView)
			: fPtr((VoidPtr)stringView), fKind(VariantKind::kString)
		{
		}

		explicit Variant(JSON* json)
			: fPtr((VoidPtr)json), fKind(VariantKind::kJson)
		{
		}

		explicit Variant(nullPtr)
			: fPtr(nullptr), fKind(VariantKind::kNull)
		{
		}

		explicit Variant(VoidPtr ptr)
			: fPtr(ptr), fKind(VariantKind::kBlob)
		{
		}

	public:
		const Char* ToString();
		VoidPtr		Leak();

		template <typename T>
		T* As()
		{
			return reinterpret_cast<T*>(fPtr);
		}

	private:
		voidPtr		fPtr{nullptr};
		VariantKind fKind{VariantKind::kNull};
	};
} // namespace Kernel
