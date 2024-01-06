/*
 *      ========================================================
 *
 *      hCore
 *      Copyright Mahrouss Logic, all rights reserved.
 *
 *      ========================================================
 */

#ifndef __SUPPORTKIT_SHARED_OBJECT_CORE_HXX__
#define __SUPPORTKIT_SHARED_OBJECT_CORE_HXX__

#include <NewKit/Defines.hpp>
#include <KernelKit/Loader.hpp>

namespace hCore
{
	class SharedObject final
	{
	public:
		struct SharedObjectTraits final
		{
			VoidPtr fImageObject;
			VoidPtr fImageEntrypointOffset;
		};

	public:
		explicit SharedObject() = default;
		~SharedObject() = default;

	public:
		HCORE_COPY_DEFAULT(SharedObject);

	private:
		SharedObjectTraits* fMounted{ nullptr };

	public:
		SharedObjectTraits** GetAddressOf() { return &fMounted; }
		SharedObjectTraits* Get() { return fMounted; }

	public:
		void Mount(SharedObjectTraits* to_mount) { fMounted = to_mount; }
		void Unmount() { if (fMounted) fMounted = nullptr; };
		
		template <typename SymbolType>
		SymbolType Load(const char* symbol_name);

	};
	
	inline void hcore_pure_call(void)
	{
		// virtual placeholder.
		return;	
	}
}

#endif /* ifndef __SUPPORTKIT_SHARED_OBJECT_CORE_HXX__ */
