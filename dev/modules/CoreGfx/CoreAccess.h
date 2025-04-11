/* -------------------------------------------

	Copyright Amlal El Mahrouss.

------------------------------------------- */

#ifndef CORE_GFX_ACCESSIBILITY_H
#define CORE_GFX_ACCESSIBILITY_H

#include <NewKit/NewKit.h>
#include <KernelKit/KPC.h>
#include <modules/CoreGfx/CoreGfx.h>
#include <modules/CoreGfx/MathGfx.h>
#include <ArchKit/ArchKit.h>

namespace FB
{
	using namespace Kernel;

	/// @brief common User interface class.
	class FBAccessibilty final
	{
		explicit FBAccessibilty() = default;
		~FBAccessibilty()		  = default;

	public:
		NE_COPY_DELETE(FBAccessibilty);

		static UInt64 Width() noexcept
		{
			return kHandoverHeader->f_GOP.f_Width;
		}

		static UInt64 Height() noexcept
		{
			return kHandoverHeader->f_GOP.f_Height;
		}
	};
} // namespace FB

#endif // !CORE_GFX_ACCESSIBILITY_H_
