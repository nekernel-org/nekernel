#ifndef __CFKIT_LOADER_UTILS_HXX__
#define __CFKIT_LOADER_UTILS_HXX__

#include <KernelKit/PE.hxx>
#include <KernelKit/MSDOS.hxx>

namespace Kernel
{
	/// @brief Find the PE header inside the blob.
	inline auto ldr_find_exec_header(DosHeaderPtr ptrDos) -> LDR_EXEC_HEADER_PTR
	{
		if (!ptrDos)
			return nullptr;

		if (ptrDos->eMagic[0] != kMagMz0)
			return nullptr;

		if (ptrDos->eMagic[1] != kMagMz1)
			return nullptr;

		return (LDR_EXEC_HEADER_PTR)(VoidPtr)(&ptrDos->eLfanew + 1);
	}

	/// @brief Find the PE optional header inside the blob.
	inline auto ldr_find_opt_exec_header(DosHeaderPtr ptrDos) -> LDR_OPTIONAL_HEADER_PTR
	{
		if (!ptrDos)
			return nullptr;

		auto exec = ldr_find_exec_header(ptrDos);

		if (!exec)
			return nullptr;

		return (LDR_OPTIONAL_HEADER_PTR)(VoidPtr)(&exec->mCharacteristics + 1);
	}

	/// @brief Find the PE header inside the blob.
	/// @note overloaded function.
	inline auto ldr_find_exec_header(const Char* ptrDos) -> LDR_EXEC_HEADER_PTR
	{
		return ldr_find_exec_header((DosHeaderPtr)ptrDos);
	}

	/// @brief Find the PE header inside the blob.
	/// @note overloaded function.
	inline auto ldr_find_opt_exec_header(const Char* ptrDos) -> LDR_OPTIONAL_HEADER_PTR
	{
		return ldr_find_opt_exec_header((DosHeaderPtr)ptrDos);
	}
} // namespace Kernel

#endif // ifndef __CFKIT_LOADER_UTILS_HXX__
