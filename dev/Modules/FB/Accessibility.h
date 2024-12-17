/* -------------------------------------------

	Copyright ZKA Technologies.

------------------------------------------- */

#ifndef FB_ACCESSIBILITY_H
#define FB_ACCESSIBILITY_H

#include <NewKit/NewKit.h>
#include <KernelKit/LPC.h>
#include <Modules/FB/FB.h>
#include <Modules/FB/Math.h>
#include <ArchKit/ArchKit.h>

namespace CG
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
