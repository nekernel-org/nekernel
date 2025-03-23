/* -------------------------------------------

	Copyright (C) 2024-2025, Amlal EL Mahrouss, all rights reserved.

------------------------------------------- */

#pragma once

#include <NewKit/Defines.h>
#include <NewKit/KString.h>
#include <NewKit/Json.h>

namespace NeOS
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
		NE_COPY_DEFAULT(Variant);

		~Variant() = default;

	public:
		explicit Variant(KString* stringView)
			: fPtr((VoidPtr)stringView), fKind(VariantKind::kString)
		{
		}

		explicit Variant(Json* json)
			: fPtr((VoidPtr)json), fKind(VariantKind::kJson)
		{
		}

		explicit Variant(nullPtr ptr)
			: fPtr(ptr), fKind(VariantKind::kNull)
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

		VariantKind& Kind();

	private:
		voidPtr		fPtr{nullptr};
		VariantKind fKind{VariantKind::kNull};
	};
} // namespace NeOS
