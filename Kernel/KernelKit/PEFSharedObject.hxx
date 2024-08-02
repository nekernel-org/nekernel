/*
 *      ========================================================
 *
 *      Kernel
 *      Copyright ZKA Technologies, all rights reserved.
 *
 *      ========================================================
 */

#ifndef __KERNELKIT_SHARED_OBJECT_HXX__
#define __KERNELKIT_SHARED_OBJECT_HXX__

#include <KernelKit/LoaderInterface.hxx>
#include <KernelKit/PEF.hxx>
#include <KernelKit/PEFCodeManager.hxx>
#include <NewKit/Defines.hxx>

namespace Kernel
{
	/// @brief Pure implementation, missing method/function handler.
	extern "C" void __mh_purecall(void);

	/**
	 * @brief Shared Library class
	 * Load library from this class
	 */
	class PEFSharedObjectInterface final
	{
	public:
		struct PEF_SHARED_OBJECT_TRAITS final
		{
			VoidPtr fImageObject{nullptr};
			VoidPtr fImageEntrypointOffset{nullptr};

			Bool IsValid() { return fImageObject && fImageEntrypointOffset; }
		};

	public:
		explicit PEFSharedObjectInterface() = default;
		~PEFSharedObjectInterface()			= default;

	public:
		NEWOS_COPY_DEFAULT(PEFSharedObjectInterface);

	private:
		PEF_SHARED_OBJECT_TRAITS* fMounted{nullptr};

	public:
		PEF_SHARED_OBJECT_TRAITS** GetAddressOf()
		{
			return &fMounted;
		}

		PEF_SHARED_OBJECT_TRAITS* Get()
		{
			return fMounted;
		}

	public:
		void Mount(PEF_SHARED_OBJECT_TRAITS* to_mount)
		{
			if (!to_mount || !to_mount->fImageObject)
				return;

			fMounted = to_mount;

			if (fLoader && to_mount)
			{
				delete fLoader;
				fLoader = nullptr;
			}

			if (!fLoader)
			{
				fLoader = new PEFLoader(fMounted->fImageObject);
			}
		}

		void Unmount()
		{
			if (fMounted)
				fMounted = nullptr;
		};

		template <typename SymbolType>
		SymbolType Load(const char* symbol_name, SizeT len, Int32 kind)
		{
			if (symbol_name == nullptr || *symbol_name == 0)
				return nullptr;
			if (len > kPathLen || len < 1)
				return nullptr;

			auto ret =
				reinterpret_cast<SymbolType>(fLoader->FindSymbol(symbol_name, kind));

			if (!ret)
			{
				if (kind == kPefCode)
					return (VoidPtr)__mh_purecall;

				return nullptr;
			}

			return ret;
		}

	private:
		PEFLoader* fLoader{nullptr};
	};

	typedef PEFSharedObjectInterface* SharedObjectPtr;

	EXTERN_C SharedObjectPtr rtl_init_shared_object(PROCESS_HEADER_BLOCK* header);
	EXTERN_C Void			 rtl_fini_shared_object(PROCESS_HEADER_BLOCK* header, SharedObjectPtr lib, Bool* successful);
} // namespace Kernel

#endif /* ifndef __KERNELKIT_SHARED_OBJECT_HXX__ */

