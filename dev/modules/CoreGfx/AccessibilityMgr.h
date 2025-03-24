/* -------------------------------------------

	Copyright Amlal EL Mahrouss.

------------------------------------------- */

#ifndef GFX_MGR_ACCESSIBILITY_H
#define GFX_MGR_ACCESSIBILITY_H

#include <NewKit/NewKit.h>
#include <KernelKit/KPC.h>
#include <modules/CoreGfx/FBMgr.h>
#include <modules/CoreGfx/MathMgr.h>
#include <ArchKit/ArchKit.h>

namespace FB
{
	using namespace NeOS;

	/// @brief common User interface class.
	class UIAccessibilty final
	{
		explicit UIAccessibilty() = default;
		~UIAccessibilty()		  = default;

	public:
		NE_COPY_DELETE(UIAccessibilty);

		static Int64 Width() noexcept
		{
			return kHandoverHeader->f_GOP.f_Width;
		}

		static Int64 Height() noexcept
		{
			return kHandoverHeader->f_GOP.f_Height;
		}
	};
} // namespace FB

#endif // !GFX_MGR_ACCESSIBILITY_H_
