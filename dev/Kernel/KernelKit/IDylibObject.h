/*
 *      ========================================================
 *
 *      Kernel
 *      Copyright (C) 2024, t& Corporation, all rights reserved., all rights reserved.
 *
 *      ========================================================
 */

#pragma once

#include <NewKit/Defines.h>
#include <CompilerKit/CompilerKit.h>

#define ZKA_DLL_OBJECT : public IDylibObject

namespace Kernel
{
	/// @brief DLL class object. A handle to a shared library.
	class IDylibObject
	{
	public:
		explicit IDylibObject() = default;
		virtual ~IDylibObject() = default;

		struct DLL_TRAITS final
		{
			VoidPtr ImageObject{nullptr};
			VoidPtr ImageEntrypointOffset{nullptr};

			Bool IsValid()
			{
				return ImageObject && ImageEntrypointOffset;
			}
		};

		ZKA_COPY_DEFAULT(IDylibObject);

		virtual DLL_TRAITS** GetAddressOf() = 0;
		virtual DLL_TRAITS*	 Get()			= 0;

		virtual Void Mount(DLL_TRAITS* to_mount) = 0;
		virtual Void Unmount()					 = 0;
	};

	/// @brief Pure implementation, missing method/function handler.
	EXTERN_C void __zka_pure_call(void);
} // namespace Kernel
