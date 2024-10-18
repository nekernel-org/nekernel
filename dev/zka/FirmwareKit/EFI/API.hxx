/* -------------------------------------------

	Copyright ZKA Web Services Co.

------------------------------------------- */

#ifndef __EFI_API__
#define __EFI_API__

#include <FirmwareKit/EFI/EFI.hxx>
#include <FirmwareKit/Handover.hxx>
#include <KernelKit/MSDOS.hxx>
#include <KernelKit/PE.hxx>

#define kZKASubsystem 17

#ifdef __NEWOSLDR__
// forward decl.
class BTextWriter;

#define __BOOTKIT_NO_INCLUDE__ 1

#include <BootKit/BootKit.hxx>
#include <modules/FB/FB.hxx>
#endif // ifdef __NEWOSLDR__

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
		return kPeMachineAMD64;
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

#ifdef __NEWOSLDR__

#include <BootKit/Platform.hxx>

#endif // ifdef __NEWOSLDR__

#endif /* ifndef __EFI_API__ */
