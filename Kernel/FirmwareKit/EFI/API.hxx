/* -------------------------------------------

	Copyright Zeta Electronics Corporation

------------------------------------------- */

#ifndef __EFI_API__
#define __EFI_API__

#include <FirmwareKit/EFI/EFI.hxx>
#include <FirmwareKit/Handover.hxx>
#include <KernelKit/MSDOS.hpp>
#include <KernelKit/PE.hxx>

#ifdef __NEWBOOT__
// forward decl.
class BTextWriter;

#define __BOOTKIT_NO_INCLUDE__ 1

#include <BootKit/BootKit.hxx>
#include <BootKit/Rsrc/NewBootFatal.rsrc>
#include <BootKit/Vendor/Qr.hxx>
#include <Builtins/GX/GX>
#endif // ifdef __NEWBOOT__

inline EfiSystemTable*	ST = nullptr;
inline EfiBootServices* BS = nullptr;

EXTERN_C void rt_cli();
EXTERN_C void rt_hlt();

namespace EFI
{
	/// @brief Halt and clear interrupts.
	/// @return
	inline Void Stop() noexcept
	{
		while (1)
		{
			rt_hlt();
			rt_cli();
		}
	}

	/**
@brief Exit EFI API to let the OS load correctly.
Bascially frees everything we have in the EFI side.
*/
	inline void ExitBootServices(UInt64 MapKey, EfiHandlePtr ImageHandle) noexcept
	{
		if (!ST)
			return;

		ST->BootServices->ExitBootServices(ImageHandle, MapKey);
	}

	enum
	{
		kPartEPM,
		kPartGPT,
		kPartMBR,
		kPartCnt,
	};

	inline UInt32 Platform() noexcept
	{
		return kPEMachineAMD64;
	}

	/***
	 * @brief Throw an error, stop execution as well.
	 * @param ErrorCode error code to be print.
	 * @param Reason reason to be print.
	 */
	inline void ThrowError(const EfiCharType* ErrorCode,
						   const EfiCharType* Reason) noexcept
	{
#ifdef __DEBUG__
		ST->ConOut->OutputString(ST->ConOut, L"\r*** STOP ***\r");

		ST->ConOut->OutputString(ST->ConOut, L"*** Error: ");
		ST->ConOut->OutputString(ST->ConOut, ErrorCode);

		ST->ConOut->OutputString(ST->ConOut, L", Reason: ");
		ST->ConOut->OutputString(ST->ConOut, Reason);

		ST->ConOut->OutputString(ST->ConOut, L" ***\r");
#endif // ifdef __DEBUG__

#ifdef __NEWBOOT__
		GXInit();

		GXDrawImg(NewBootFatal, NEWBOOTFATAL_HEIGHT, NEWBOOTFATAL_WIDTH,
						(kHandoverHeader->f_GOP.f_Width - NEWBOOTFATAL_WIDTH) / 2,
						(kHandoverHeader->f_GOP.f_Height - NEWBOOTFATAL_HEIGHT) / 2);

		GXFini();

		/// Show the QR code now.

		constexpr auto ver = 4;
		auto		   ecc = qr::Ecc::H;
		auto		   str = "https://el-mahrouss-logic.com/";
		auto		   len = StrLen("https://el-mahrouss-logic.com/");

		qr::Qr<ver>	   encoder;
		qr::QrDelegate encoderDelegate;

		encoder.encode(str, len, ecc, 0); // Manual mask 0

		/// tell delegate to draw encoded QR.
		encoderDelegate.draw<ver>(encoder, (kHandoverHeader->f_GOP.f_Width - encoder.side_size()) - 20,
								  (kHandoverHeader->f_GOP.f_Height - encoder.side_size()) / 2);

#endif // ifdef __NEWBOOT__

		EFI::Stop();
	}
} // namespace EFI

inline void InitEFI(EfiSystemTable* SystemTable) noexcept
{
	if (!SystemTable)
		return;

	ST = SystemTable;
	BS = ST->BootServices;

	ST->ConOut->ClearScreen(SystemTable->ConOut);
	ST->ConOut->SetAttribute(SystemTable->ConOut, kEFIYellow);

	ST->BootServices->SetWatchdogTimer(0, 0, 0, nullptr);
	ST->ConOut->EnableCursor(ST->ConOut, false);
}

#ifdef __BOOTLOADER__

#include <BootKit/Platform.hxx>

#endif // ifdef __BOOTLOADER__

#define kNewOSSubsystem 17

#endif /* ifndef __EFI_API__ */
