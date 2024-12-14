/* -------------------------------------------

	Copyright (C) 2024, Amlal EL Mahrouss, all rights reserved.

------------------------------------------- */

#include <HALKit/POWER/Processor.h>
#include <KernelKit/DebugOutput.h>
#include <HALKit/POWER/MMU.h>

/// @note Refer to SoC documentation.

using namespace Kernel;

EXTERN_C Void hal_write_tlb(UInt32 mas0, UInt32 mas1, UInt32 mas2, UInt32 mas3, UInt32 mas7)
{
	hal_mtspr(MAS0, mas0);
	hal_mtspr(MAS1, mas1);
	hal_mtspr(MAS2, mas2);
	hal_mtspr(MAS3, mas3);
	hal_mtspr(MAS7, mas7);

	hal_flush_tlb();
}

EXTERN_C Bool hal_set_tlb(UInt8 tlb, UInt32 epn, UInt64 rpn, UInt8 perms, UInt8 wimge, UInt8 ts, UInt8 esel, UInt8 tsize, UInt8 iprot)
{
	if ((hal_mfspr(SPRN_MMUCFG) & MMUCFG_MAVN) == MMUCFG_MAVN_V1 && (tsize & 1))
	{
		// this MMU does not allow odd tsize values
		return false;
	}

	UInt32 mas0 = FSL_BOOKE_MAS0(tlb, esel, 0);
	UInt32 mas1 = FSL_BOOKE_MAS1(1, iprot, 0, ts, tsize);
	UInt32 mas2 = FSL_BOOKE_MAS2(epn, wimge);
	UInt32 mas3 = FSL_BOOKE_MAS3(rpn, 0, perms);
	UInt32 mas7 = FSL_BOOKE_MAS7(rpn);

	hal_write_tlb(mas0, mas1, mas2, mas3, mas7);

	return true;
}

/// @brief Flush TLB
EXTERN_C void hal_flush_tlb()
{
	asm volatile("isync;tlbwe;msync;isync");
}
