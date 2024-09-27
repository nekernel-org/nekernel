/* -------------------------------------------

	Copyright ZKA Technologies.

------------------------------------------- */

#pragma once

#include <Modules/CoreCG/Accessibility.hxx>
#include <KernelKit/Heap.hxx>
#include <KernelKit/UserProcessScheduler.hxx>
#include <KernelKit/LPC.hxx>
#include <NewKit/Defines.hxx>
#include <NewKit/Utils.hxx>
#include <Modules/CoreCG/FbRenderer.hxx>
#include <Modules/CoreCG/Rsrc/WndControls.rsrc>
#include <Modules/CoreCG/TextRenderer.hxx>

namespace CG
{
	using namespace Kernel;

	class ICGBackground;

	/// \brief Draw background (either image or solid color)
	inline Void CGDrawDesktopBackground(UInt32* raw_bmp = nullptr, SizeT width = 0, SizeT height = 0) noexcept
	{
		CGInit();

		if (!raw_bmp)
		{
			const auto cColorBackground = CGColor(0x45, 0x00, 0x06);

			CGDrawInRegion(cColorBackground, CG::CGAccessibilty::The().Height(), CG::CGAccessibilty::The().Width(),
						   0, 0);
		}
		else
		{
			CGDrawBitMapInRegion(raw_bmp, height, width,
								 0, 0);
		}

		CGFini();
	}
} // namespace CG
