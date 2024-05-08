/* -------------------------------------------

    Copyright SoftwareLabs

------------------------------------------- */

#include <HALKit/POWER/ppc-cpu.h>
#include <HALKit/POWER/ppc-mmu.h>

#include <HALKit/POWER/Processor.hpp>
#include <KernelKit/DebugOutput.hpp>

/// @note refer to our SoC documentation.

using namespace NewOS;

/// @brief Write directly to the specific TLB.
/// @param mas0
/// @param mas1
/// @param mas2
/// @param mas3
/// @param mas7
static void hal_write_tlb(uint32_t mas0, uint32_t mas1, uint32_t mas2, uint32_t mas3, uint32_t mas7)
{
	mtspr(MAS0, mas0);
	mtspr(MAS1, mas1);
	mtspr(MAS2, mas2);
	mtspr(MAS3, mas3);
	mtspr(MAS7, mas7);

	hal_flush_tlb();
}

void hal_set_tlb(uint8_t tlb, uint32_t epn, uint64_t rpn, uint8_t perms, uint8_t wimge, uint8_t ts, uint8_t esel, uint8_t tsize, uint8_t iprot)
{
	if ((mfspr(SPRN_MMUCFG) & MMUCFG_MAVN) == MMUCFG_MAVN_V1 && (tsize & 1))
	{
		// this mmu-version does not allow odd tsize values
		return;
	}
	uint32_t mas0 = FSL_BOOKE_MAS0(tlb, esel, 0);
	uint32_t mas1 = FSL_BOOKE_MAS1(1, iprot, 0, ts, tsize);
	uint32_t mas2 = FSL_BOOKE_MAS2(epn, wimge);
	uint32_t mas3 = FSL_BOOKE_MAS3(rpn, 0, perms);
	uint32_t mas7 = FSL_BOOKE_MAS7(rpn);

	hal_write_tlb(mas0, mas1, mas2, mas3, mas7);
}

/// @brief Flush system TLB.
EXTERN_C void hal_flush_tlb()
{
	asm volatile("isync;tlbwe;msync;isync");
}
