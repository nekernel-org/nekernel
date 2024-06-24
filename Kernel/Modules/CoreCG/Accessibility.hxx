/* -------------------------------------------

	Copyright Zeta Electronics Corporation

------------------------------------------- */

#pragma once

#include "CompilerKit/Detail.hxx"
#include <NewKit/NewKit.hpp>
#include <KernelKit/HError.hpp>
#include <Modules/CoreCG/CoreCG.hxx>
#include <Modules/CoreCG/Lerp.hxx>
#include <ArchKit/ArchKit.hpp>

namespace NewOS
{
	inline Bool cKTSyncCall = false;

	inline float cDeviceWidthAlert	= 150;
	inline float cDeviceHeightAlert = 141;

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

		Int32 Show(const char* text, int x, int y)
		{
			if (!text || *text == 0)
				return kErrorHeapNotPresent;

			// GXDrawText("Sleek", text, cDeviceWidthAlert, x, cDeviceHeightAlert, y);

			if (cKTSyncCall)
				return kErrorAsync;

			return kErrorSuccess;
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
} // namespace NewOS
