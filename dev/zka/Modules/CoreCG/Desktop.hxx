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
#include <Modules/CoreCG/FB.hxx>
#include <Modules/CoreCG/Rsrc/Controls.rsrc>
#include <Modules/CoreCG/Text.hxx>

namespace CG
{
	using namespace Kernel;

	class ICGDesktopBackground;

	/// \brief Desktop background class.
	class ICGDesktopBackground final
	{
		UInt32* fTheFB{nullptr};
		UInt32	fPPL{0};
		UInt32	fWidth{0};
		UInt32	fHeight{0};

	public:
		explicit ICGDesktopBackground(UInt32* fb, UInt32 ppl, UInt32 width, UInt32 height)
			: fTheFB(fb), fPPL(ppl), fWidth(width), fHeight(height)
		{
		}

		~ICGDesktopBackground() = default;

		ZKA_COPY_DEFAULT(ICGDesktopBackground);

		Void Refresh(UInt32* raw_bmp = nullptr, SizeT width = 0, SizeT height = 0) noexcept
		{
			if (this->fWidth < width ||
				this->fHeight < height)
				return;

			CGInit();

			if (!raw_bmp)
			{
				const auto cColorBackground = CGColor(0x45, 0x00, 0x06);

				for (Kernel::SizeT x_base = 0; x_base < width; ++x_base)
				{
					for (Kernel::SizeT y_base = 0; y_base < height; ++y_base)
					{
						*(((volatile Kernel::UInt32*)(fTheFB +
													  4 * fPPL *
														  x_base +
													  4 * y_base))) = cColorBackground;
					}
				}
			}
			else
			{
				Kernel::SizeT cur = 0;

				for (Kernel::SizeT y = 0; y < height; ++y)
				{
					for (Kernel::SizeT x = 0; x < width; ++x)
					{
						*(((Kernel::UInt32*)(fTheFB +
											 4 * fPPL *
												 y +
											 4 * x))) = raw_bmp[cur];

						++cur;
					}
				}
			}

			CGFini();
		}
	};

	/// \brief Draw background (either image or solid color)
	inline Void CGFillBackground(UInt32* raw_bmp = nullptr, SizeT width = 0, SizeT height = 0) noexcept
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
