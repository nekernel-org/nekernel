/* -------------------------------------------

	Copyright ZKA Technologies.

------------------------------------------- */

#ifndef __CORECG_ACCESS_HXX__
#define __CORECG_ACCESS_HXX__

#include <NewKit/NewKit.hxx>
#include <KernelKit/LPC.hxx>
#include <Modules/CoreCG/FbRenderer.hxx>
#include <Modules/CoreCG/Math.hxx>
#include <ArchKit/ArchKit.hxx>

namespace CG
{
	using namespace Kernel;

	/// @brief Accessibilty manager class.
	class CGAccessibilty final
	{
		explicit CGAccessibilty() = default;
		~CGAccessibilty()		  = default;

	public:
		ZKA_COPY_DELETE(CGAccessibilty);

		STATIC CGAccessibilty& The()
		{
			STATIC CGAccessibilty the;
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
} // namespace CG

#endif // !__CORECG_ACCESS_HXX__
