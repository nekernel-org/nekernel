/* -------------------------------------------

	Copyright ZKA Technologies

------------------------------------------- */

#include <BootKit/BootKit.hxx>
#include <BootKit/Rsrc/NewBoot.rsrc>
#include <Modules/CoreCG/FbRenderer.hxx>
#include <FirmwareKit/EFI.hxx>
#include <FirmwareKit/EFI/API.hxx>
#include <FirmwareKit/Handover.hxx>
#include <KernelKit/MSDOS.hxx>
#include <KernelKit/PE.hxx>
#include <KernelKit/PEF.hxx>
#include <NewKit/Macros.hxx>
#include <NewKit/Ref.hxx>
#include <BootKit/ProgramLoader.hxx>
#include <Modules/CoreCG/TextRenderer.hxx>
#include <cstring>

// make the compiler shut up.
#ifndef kMachineModel
#define kMachineModel "ZKA SSD"
#endif // !kMachineModel

#ifndef cExpectedWidth
#define cExpectedWidth 1280
#endif

#ifndef cExpectedHeight
#define cExpectedHeight 720
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

	kStride = 4;

	for (SizeT i = 0; i < kGop->Mode->MaxMode; ++i)
	{
		EfiGraphicsOutputProtocolModeInformation* infoPtr = nullptr;
		UInt32									  sz	  = 0U;

		kGop->QueryMode(kGop, i, &sz, &infoPtr);

		if (infoPtr->HorizontalResolution == cExpectedWidth &&
			infoPtr->VerticalResolution == cExpectedHeight)
		{
			kGop->SetMode(kGop, i);
			break;
		}
	}
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

	writer.Write(L"ZKA Technologies (R) newosldr: ")
		.Write(BVersionString::The())
		.Write("\r");

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
			writer.Write(L"newosldr: Filling rsdptr...\r");
			handoverHdrPtr->f_HardwareTables.f_VendorPtr = (VoidPtr)vendorTable;

			break;
		}
	}

	// Fill handover header now.

	handoverHdrPtr->f_Magic	  = kHandoverMagic;
	handoverHdrPtr->f_Version = kHandoverVersion;

	// Provide fimware vendor name.

	BCopyMem(handoverHdrPtr->f_FirmwareVendorName, SystemTable->FirmwareVendor,
			 handoverHdrPtr->f_FirmwareVendorLen);

	handoverHdrPtr->f_GOP.f_The			 = kGop->Mode->FrameBufferBase;
	handoverHdrPtr->f_GOP.f_Width		 = kGop->Mode->Info->VerticalResolution;
	handoverHdrPtr->f_GOP.f_Height		 = kGop->Mode->Info->HorizontalResolution;
	handoverHdrPtr->f_GOP.f_PixelPerLine = kGop->Mode->Info->PixelsPerScanLine;
	handoverHdrPtr->f_GOP.f_PixelFormat	 = kGop->Mode->Info->PixelFormat;
	handoverHdrPtr->f_GOP.f_Size		 = kGop->Mode->FrameBufferSize;

	// Assign to global 'kHandoverHeader'.

	kHandoverHeader = handoverHdrPtr;

	// check if we are running in the PC platform. If so abort.
#if defined(__NEWOS_AMD64__) && !defined(__DEBUG__)
	writer.Write(L"\rnewosldr: AMD64 support is not official.\r");
	EFI::ThrowError(L"Beta-Software", L"Beta Software.");
#endif

	// get memory map.

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

	// ------------------------------------------ //
	// draw background color.
	// ------------------------------------------ //

	CGInit();
	CGDrawInRegion(CGColor(0xFF, 0xFF, 0xFF), handoverHdrPtr->f_GOP.f_Height, handoverHdrPtr->f_GOP.f_Width, 0, 0);
	CGFini();

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

	BFileReader readerKernel(L"newoskrnl.exe", ImageHandle);

	readerKernel.ReadAll(0);

	Boot::ProgramLoader* loader = nullptr;

	// ------------------------------------------ //
	// If we succeed in reading the blob, then execute it.
	// ------------------------------------------ //

	if (readerKernel.Blob())
	{
		loader = new Boot::ProgramLoader(readerKernel.Blob());
		loader->SetName("\"newoskrnl.exe\" (64-bit SMP)");
	}

	writer.Write("newosldr: Running: ").Write(loader->GetName()).Write("\r");

	CopyMem(handoverHdrPtr->f_CommandLine[0], "/SMP", StrLen("/SMP"));

	EFI::ExitBootServices(*MapKey, ImageHandle);

	// ---------------------------------------------------- //
	// Call kernel.
	// ---------------------------------------------------- //

	cg_write_text((UInt8*)"NEWOSKRNL", 0, 0, RGB(0x10, 0x10, 0x10));

	loader->Start(handoverHdrPtr);

	EFI::Stop();

	CANT_REACH();
}
