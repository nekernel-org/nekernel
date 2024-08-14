/* -------------------------------------------

	Copyright ZKA Technologies.

------------------------------------------- */

#ifndef __CORECG_ACCESS_HXX__
#define __CORECG_ACCESS_HXX__

#include <NewKit/NewKit.hxx>
#include <KernelKit/LPC.hxx>
#include <Modules/CoreCG/FbRenderer.hxx>
#include <Modules/CoreCG/Lerp.hxx>
#include <ArchKit/ArchKit.hxx>

namespace Kernel
{
	inline Bool cKTSyncCall = false;

	inline float cDeviceWidthAlert	= 150;
	inline float cDeviceHeightAlert = 141;

	/// @brief common user interface class.
	class UIAccessibilty final
	{
		explicit UIAccessibilty() = default;

	public:
		NEWOS_COPY_DELETE(UIAccessibilty);

		static UIAccessibilty& The()
		{
			static UIAccessibilty the;
			return the;
		}

		Int64 Width() noexcept
		{
			return kHandoverHeader->f_GOP.f_Width;
		}

		Int64 Height() noexcept
		{
			return kHandoverHeader->f_GOP.f_Height;
		}
	};
} // namespace Kernel

#endif // !__CORECG_ACCESS_HXX__
