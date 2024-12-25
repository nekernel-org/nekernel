/* -------------------------------------------

	Copyright Theater Quality Inc.

------------------------------------------- */

#ifndef FB_ACCESSIBILITY_H
#define FB_ACCESSIBILITY_H

#include <NewKit/NewKit.h>
#include <KernelKit/LPC.h>
#include <Modules/GfxMgr/FBMgr.h>
#include <Modules/GfxMgr/MathMgr.h>
#include <ArchKit/ArchKit.h>

namespace UI
{
	using namespace Kernel;

	/// @brief common User interface class.
	class UIAccessibilty final
	{
		explicit UIAccessibilty() = default;
		~UIAccessibilty()		  = default;

	public:
		ZKA_COPY_DELETE(UIAccessibilty);

		static Int64 Width() noexcept
		{
			return kHandoverHeader->f_GOP.f_Width;
		}

		static Int64 Height() noexcept
		{
			return kHandoverHeader->f_GOP.f_Height;
		}
	};
} // namespace CG

#endif // !FB_ACCESSIBILITY_H_
