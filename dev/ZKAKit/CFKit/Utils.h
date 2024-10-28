#ifndef CFKIT_UTILS_H
#define CFKIT_UTILS_H

#include <KernelKit/PE.h>
#include <KernelKit/MSDOS.h>

namespace CFKit
{
	using namespace Kernel;

	/// @brief Finds the PE header inside the blob.
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

	/// @brief Finds the PE optional header inside the blob.
	inline auto ldr_find_opt_exec_header(DosHeaderPtr ptrDos) -> LDR_OPTIONAL_HEADER_PTR
	{
		if (!ptrDos)
			return nullptr;

		auto exec = ldr_find_exec_header(ptrDos);

		if (!exec)
			return nullptr;

		return (LDR_OPTIONAL_HEADER_PTR)(VoidPtr)(&exec->mCharacteristics + 1);
	}

	/// @brief Finds the PE header inside the blob.
	/// @note overloaded function.
	inline auto ldr_find_exec_header(const Char* ptrDos) -> LDR_EXEC_HEADER_PTR
	{
		return ldr_find_exec_header((DosHeaderPtr)ptrDos);
	}

	/// @brief Finds the PE header inside the blob.
	/// @note overloaded function.
	inline auto ldr_find_opt_exec_header(const Char* ptrDos) -> LDR_OPTIONAL_HEADER_PTR
	{
		return ldr_find_opt_exec_header((DosHeaderPtr)ptrDos);
	}
} // namespace CFKit

#endif // ifndef CFKIT_UTILS_H
