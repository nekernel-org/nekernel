/*
 *      ========================================================
 *
 *      Kernel
 *      Copyright ZKA Technologies., all rights reserved.
 *
 *      ========================================================
 */

#pragma once

#include <NewKit/Defines.hxx>

namespace Kernel
{
	class DLLInterface
	{
	public:
		explicit DLLInterface() = default;
		virtual ~DLLInterface() = default;

		struct DLL_TRAITS final
		{
			VoidPtr fImageObject{nullptr};
			VoidPtr fImageEntrypointOffset{nullptr};

			Bool IsValid()
			{
				return fImageObject && fImageEntrypointOffset;
			}
		};

		ZKA_COPY_DEFAULT(DLLInterface);

		virtual DLL_TRAITS** GetAddressOf() = 0;
		virtual DLL_TRAITS*	 Get()			= 0;

		virtual Void Mount(DLL_TRAITS* to_mount) = 0;
		virtual Void Unmount()					 = 0;


		template <typename SymbolType>
		SymbolType Load(const Char* symbol_name, SizeT len, Int32 kind)
		{
			return nullptr;
		}
	};

	/// @brief Pure implementation, missing method/function handler.
	EXTERN_C void __zka_pure_call(void);
} // namespace Kernel
