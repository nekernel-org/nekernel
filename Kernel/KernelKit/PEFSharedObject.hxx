/*
 *      ========================================================
 *
 *      NewOS
 *      Copyright SoftwareLabs, all rights reserved.
 *
 *      ========================================================
 */

#ifndef __KERNELKIT_SHARED_OBJECT_HXX__
#define __KERNELKIT_SHARED_OBJECT_HXX__

#include <KernelKit/LoaderInterface.hpp>
#include <KernelKit/PEF.hpp>
#include <KernelKit/PEFCodeManager.hxx>
#include <NewKit/Defines.hpp>

namespace NewOS
{
	/// @brief Pure implementation, missing method/function handler.
	extern "C" void __mh_purecall(void);

	/**
	 * @brief Shared Library class
	 * Load library from this class
	 */
	class SharedObject final
	{
	public:
		struct SharedObjectTrait final
		{
			VoidPtr fImageObject;
			VoidPtr fImageEntrypointOffset;
		};

	public:
		explicit SharedObject() = default;
		~SharedObject()			= default;

	public:
		NEWOS_COPY_DEFAULT(SharedObject);

	private:
		SharedObjectTrait* fMounted{nullptr};

	public:
		SharedObjectTrait** GetAddressOf()
		{
			return &fMounted;
		}

		SharedObjectTrait* Get()
		{
			return fMounted;
		}

	public:
		void Mount(SharedObjectTrait* to_mount)
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

	typedef SharedObject* SharedObjectPtr;
} // namespace NewOS

#endif /* ifndef __KERNELKIT_SHARED_OBJECT_HXX__ */
