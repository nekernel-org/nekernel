/* -------------------------------------------

	Copyright Zeta Electronics Corporation

	File: MSDOS.hpp
	Purpose: MS-DOS header for Kernel.

	Revision History:

	30/01/24: Added file (amlel)

------------------------------------------- */

#ifndef __MSDOS_EXEC__
#define __MSDOS_EXEC__

#include <KernelKit/PE.hxx>
#include <NewKit/Defines.hpp>

// Last Rev
// Sat Feb 24 CET 2024

#define kMagMz0 'M'
#define kMagMz1 'Z'

typedef Kernel::UInt32 DosWord;
typedef Kernel::Long	  DosLong;

typedef struct _DosHeader
{
	Kernel::UInt8 eMagic[2];
	DosWord		 eMagLen;
	DosWord		 ePagesCount;
	DosWord		 eCrlc;
	DosWord		 eCParHdr;
	DosWord		 eMinAlloc;
	DosWord		 eMaxAlloc;
	DosWord		 eStackSeg;
	DosWord		 eStackPtr;
	DosWord		 eChksum;
	DosWord		 eIp;
	DosWord		 eCs;
	DosWord		 eLfarlc;
	DosWord		 eOvno;
	DosWord		 eRes[4];
	DosWord		 eOemid;
	DosWord		 eOeminfo;
	DosWord		 eRes2[10];
	DosLong		 eLfanew;
} DosHeader, *DosHeaderPtr;

namespace Kernel
{
	/// @brief Find the PE header inside the the blob.
	inline auto rt_find_exec_header(DosHeaderPtr ptrDos) -> VoidPtr
	{
		if (!ptrDos)
			return nullptr;
		if (ptrDos->eMagic[0] != kMagMz0)
			return nullptr;
		if (ptrDos->eMagic[1] != kMagMz1)
			return nullptr;

		return (VoidPtr)(&ptrDos->eLfanew + 1);
	}
} // namespace Kernel

#endif /* ifndef __MSDOS_EXEC__ */
