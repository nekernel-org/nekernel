/* -------------------------------------------

	Copyright ZKA Technologies

------------------------------------------- */

#ifndef __EFI_API__
#define __EFI_API__

#include <FirmwareKit/EFI/EFI.hxx>
#include <FirmwareKit/Handover.hxx>
#include <KernelKit/MSDOS.hxx>
#include <KernelKit/PE.hxx>

#ifdef __NEWBOOT__
// forward decl.
class BTextWriter;

#define cWebsiteMacro "https://zka-mobile.com/help"

#define __BOOTKIT_NO_INCLUDE__ 1

#include <BootKit/BootKit.hxx>
#include <BootKit/Rsrc/NewBootFatal.rsrc>
#include <BootKit/Vendor/Qr.hxx>
#include <Modules/CoreCG/CoreCG.hxx>
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
		ST->ConOut->OutputString(ST->ConOut, L"\r*** STOP ***\r");

		ST->ConOut->OutputString(ST->ConOut, L"*** Error: ");
		ST->ConOut->OutputString(ST->ConOut, ErrorCode);

		ST->ConOut->OutputString(ST->ConOut, L", Reason: ");
		ST->ConOut->OutputString(ST->ConOut, Reason);

		ST->ConOut->OutputString(ST->ConOut, L" ***\r");

#ifdef __NEWBOOT__
		// Show the QR code now.

		constexpr auto ver = 4;
		auto		   ecc = qr::Ecc::H;
		auto		   input = cWebsiteMacro;
		auto		   len = StrLen(cWebsiteMacro);

		qr::Qr<ver>	   encoder;
		qr::QrDelegate encoderDelegate;

		encoder.encode(input, len, ecc, 0); // Manual mask 0

		constexpr auto whereX = 10;
		constexpr auto whereY = 10;

		/// tell delegate to draw encoded QR.
		encoderDelegate.draw<ver>(encoder, whereX,
								  whereY);

		EFI::Stop();
#endif // ifdef __NEWBOOT__
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
