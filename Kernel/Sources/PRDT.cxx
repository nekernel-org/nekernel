/* -------------------------------------------

	Copyright ZKA Technologies

------------------------------------------- */

#include <KernelKit/DebugOutput.hxx>
#include <NewKit/String.hxx>
#include <StorageKit/PRDT.hxx>

namespace Kernel
{
	/// @brief constructs a new PRD.
	/// @param prd PRD reference.
	/// @note This doesnt construct a valid, please fill it by yourself.
	void construct_prdt(Ref<PRDT>& prd)
	{
		prd.Leak().fPhysAddress = 0x0;
		prd.Leak().fSectorCount = 0x0;
		prd.Leak().fEndBit		= 0x0;
	}
} // namespace Kernel
