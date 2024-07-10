/* -------------------------------------------

	Copyright Zeta Electronics Corporation

------------------------------------------- */

#include <BootKit/BootKit.hxx>
#include <BootKit/Rsrc/NewBoot.rsrc>
#include <Modules/CoreCG/CoreCG.hxx>
#include <FirmwareKit/EFI.hxx>
#include <FirmwareKit/EFI/API.hxx>
#include <FirmwareKit/Handover.hxx>
#include <KernelKit/MSDOS.hxx>
#include <KernelKit/PE.hxx>
#include <KernelKit/PEF.hpp>
#include <NewKit/Macros.hpp>
#include <NewKit/Ref.hpp>
#include <BootKit/ProgramLoader.hxx>
#include <cstring>

#include <BootKit/Vendor/Support.hxx>
#include <BootKit/STB.hxx>

/// make the compiler shut up.
#ifndef kMachineModel
#define kMachineModel "Zeta SSD"
#endif // !kMachineModel

#ifndef cExpectedWidth
#define cExpectedWidth 436
#endif

#ifndef cExpectedHeight
#define cExpectedHeight 644
#endif

/** Graphics related. */

STATIC EfiGraphicsOutputProtocol* kGop	  = nullptr;
STATIC UInt16					  kStride = 0U;
STATIC EfiGUID					  kGopGuid;

EXTERN_C Void hal_init_platform(HEL::HandoverInformationHeader* HIH);

/**
	@brief Finds and stores the GOP.
*/

STATIC Void InitVideoFB() noexcept
{
	kGopGuid = EfiGUID(EFI_GRAPHICS_OUTPUT_PROTOCOL_GUID);
	kGop	 = nullptr;

	extern EfiBootServices* BS;

	BS->LocateProtocol(&kGopGuid, nullptr, (VoidPtr*)&kGop);

	for (size_t i = 0; i < kGop->Mode->MaxMode; ++i)
	{
		EfiGraphicsOutputProtocolModeInformation* infoPtr = nullptr;

		if (infoPtr->HorizontalResolution == cExpectedWidth &&
			infoPtr->VerticalResolution == cExpectedHeight)
		{
			kGop->SetMode(kGop, i);
			break;
		}
	}

	kStride = 4;
}

/// @brief check the BootDevice if suitable.
STATIC Bool CheckBootDevice(BootDeviceATA& ataDev)
{
	if (ataDev.Leak().mErr)
		return false;
	return true;
}

/// @brief Main EFI entrypoint.
/// @param ImageHandle Handle of this image.
/// @param SystemTable The system table of it.
/// @return nothing, never returns.
EFI_EXTERN_C EFI_API Int Main(EfiHandlePtr	  ImageHandle,
							  EfiSystemTable* SystemTable)
{
	InitEFI(SystemTable); ///! Init the EFI library.
	InitVideoFB();		  ///! Init the GOP.

	BTextWriter writer;

	/// Splash screen stuff

	writer.Write(L"Zeta Electronics Corporation (R) newosldr: ")
		.Write(BVersionString::The())
		.Write("\r");

#ifndef __DEBUG__
	writer.Write(L"\rnewosldr: AMD64 is only supported in debug mode.\r");

	EFI::Stop();

	CANT_REACH();
#endif

	UInt32*				 MapKey		= new UInt32();
	UInt32*				 SizePtr	= new UInt32();
	EfiMemoryDescriptor* Descriptor = nullptr;
	UInt32*				 SzDesc		= new UInt32();
	UInt32*				 RevDesc	= new UInt32();

	*MapKey	 = 0;
	*SizePtr = 0;

	HEL::HandoverInformationHeader* handoverHdrPtr =
		new HEL::HandoverInformationHeader();

	for (SizeT indexVT = 0; indexVT < SystemTable->NumberOfTableEntries;
		 ++indexVT)
	{
		Char* vendorTable = reinterpret_cast<Char*>(
			SystemTable->ConfigurationTable[indexVT].VendorTable);

		/// ACPI's 'RSD PTR', which contains hardware tables (MADT, FACP...)
		if (vendorTable[0] == 'R' && vendorTable[1] == 'S' &&
			vendorTable[2] == 'D' && vendorTable[3] == ' ' &&
			vendorTable[4] == 'P' && vendorTable[5] == 'T' &&
			vendorTable[6] == 'R' && vendorTable[7] == ' ')
		{
			writer.Write(L"newosldr: Found ACPI RSD PTR!\r");
			handoverHdrPtr->f_HardwareTables.f_RsdPtr = (VoidPtr)vendorTable;

			break;
		}
	}

	handoverHdrPtr->f_Magic	  = kHandoverMagic;
	handoverHdrPtr->f_Version = kHandoverVersion;

	BCopyMem(handoverHdrPtr->f_FirmwareVendorName, SystemTable->FirmwareVendor,
			 handoverHdrPtr->f_FirmwareVendorLen);

	handoverHdrPtr->f_GOP.f_The			 = kGop->Mode->FrameBufferBase;
	handoverHdrPtr->f_GOP.f_Width		 = kGop->Mode->Info->VerticalResolution;
	handoverHdrPtr->f_GOP.f_Height		 = kGop->Mode->Info->HorizontalResolution;
	handoverHdrPtr->f_GOP.f_PixelPerLine = kGop->Mode->Info->PixelsPerScanLine;
	handoverHdrPtr->f_GOP.f_PixelFormat	 = kGop->Mode->Info->PixelFormat;
	handoverHdrPtr->f_GOP.f_Size		 = kGop->Mode->FrameBufferSize;

	///! Finally draw bootloader screen.

	kHandoverHeader = handoverHdrPtr;

	CGInit();

	CGDrawInRegion(cCGClearClr, handoverHdrPtr->f_GOP.f_Height,
				   handoverHdrPtr->f_GOP.f_Width, 0, 0);

	CGFini();

	CGDrawBitMapInRegion(NewBoot, NEWBOOT_HEIGHT, NEWBOOT_WIDTH,
						 (handoverHdrPtr->f_GOP.f_Width - NEWBOOT_WIDTH) / 2,
						 (handoverHdrPtr->f_GOP.f_Height - NEWBOOT_HEIGHT) / 2);

	CGFini();

	BS->GetMemoryMap(SizePtr, Descriptor, MapKey, SzDesc, RevDesc);

	Descriptor = new EfiMemoryDescriptor[*SzDesc];
	BS->GetMemoryMap(SizePtr, Descriptor, MapKey, SzDesc, RevDesc);

	auto cDefaultMemoryMap = 0; // The sixth entry.

	//-----------------------------------------------------------//
	// A simple loop which finds a usable memory region for us.
	//-----------------------------------------------------------//

	SizeT lookIndex = 0UL;

	for (; Descriptor[lookIndex].Kind != EfiMemoryType::EfiConventionalMemory; ++lookIndex)
	{
		;
	}

	cDefaultMemoryMap = lookIndex;

	//-----------------------------------------------------------//
	// Update handover file specific table and phyiscal start field.
	//-----------------------------------------------------------//

	handoverHdrPtr->f_PhysicalStart =
		(VoidPtr)Descriptor[cDefaultMemoryMap].PhysicalStart;

	handoverHdrPtr->f_FirmwareSpecific[HEL::kHandoverSpecificAttrib] =
		Descriptor[cDefaultMemoryMap].Attribute;
	handoverHdrPtr->f_FirmwareSpecific[HEL::kHandoverSpecificKind] =
		Descriptor[cDefaultMemoryMap].Kind;
	handoverHdrPtr->f_FirmwareSpecific[HEL::kHandoverSpecificMemoryEfi] =
		(UIntPtr)Descriptor;

	handoverHdrPtr->f_VirtualStart =
		(VoidPtr)Descriptor[cDefaultMemoryMap].VirtualStart;
	handoverHdrPtr->f_VirtualSize =
		Descriptor[cDefaultMemoryMap].NumberOfPages; /* # of pages */

	handoverHdrPtr->f_FirmwareVendorLen = BStrLen(SystemTable->FirmwareVendor);

	// ---------------------------------------------------- //
	// The following checks for an exisiting partition
	// inside the disk, if it doesn't have one,
	// format the disk.
	// ---------------------------------------------------- //

	BDiskFormatFactory<BootDeviceATA> diskFormatter;

	// ---------------------------------------------------- //
	// if not formated yet, then format it with the following folders:
	// /, /Boot, /Applications.
	// ---------------------------------------------------- //
	if (!diskFormatter.IsPartitionValid())
	{
		BDiskFormatFactory<BootDeviceATA>::BFileDescriptor rootDesc{0};

		CopyMem(rootDesc.fFileName, kNewFSRoot, StrLen(kNewFSRoot));
		rootDesc.fKind = kNewFSCatalogKindDir;

		diskFormatter.Format(kMachineModel, &rootDesc, 1);
	}

#ifdef __NEWOS_CAN_PATCH__

	BFileReader readerKernel(L"newoskrnl.exe", ImageHandle);

	/// TODO: BFileReader::GetSize(...);
	constexpr auto cKernelSz = 275101;
	readerKernel.ReadAll(cKernelSz, 4096);

	Boot::ProgramLoader* loader = nullptr;

	if (readerKernel.Blob())
	{
		loader = new Boot::ProgramLoader(readerKernel.Blob());
		loader->SetName("'newoskrnl.exe'");
	}

#endif // ifdef __NEWOS_CAN_PATCH__

	EFI::ExitBootServices(*MapKey, ImageHandle);

	// ---------------------------------------------------- //
	// Fallback to builtin kernel.
	//
	// ---------------------------------------------------- //
#ifdef __NEWOS_CAN_PATCH__

	if (loader)
		loader->Start(handoverHdrPtr);
#endif // ifdef __NEWOS_CAN_PATCH__

	hal_init_platform(handoverHdrPtr);

	EFI::Stop();

	CANT_REACH();
}
