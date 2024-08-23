/* -------------------------------------------

	Copyright ZKA Technologies.

------------------------------------------- */

#include <BootKit/BootKit.hxx>
#include <BootKit/Rsrc/NewBoot.rsrc>
#include <Modules/CoreCG/FbRenderer.hxx>
#include <Modules/CoreCG/TextRenderer.hxx>
#include <FirmwareKit/EFI.hxx>
#include <FirmwareKit/EFI/API.hxx>
#include <FirmwareKit/Handover.hxx>
#include <KernelKit/MSDOS.hxx>
#include <KernelKit/PE.hxx>
#include <KernelKit/PEF.hxx>
#include <NewKit/Macros.hxx>
#include <NewKit/Ref.hxx>
#include <BootKit/Thread.hxx>
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

EXTERN_C Void rt_reset_hardware();

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

	UInt32				 MapKey		= 0;
	UInt32				 SizePtr	= sizeof(EfiMemoryDescriptor);
	EfiMemoryDescriptor* Descriptor = nullptr;
	UInt32				 SzDesc		= sizeof(EfiMemoryDescriptor);
	UInt32				 RevDesc	= 0;

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
			handoverHdrPtr->f_HardwareTables.f_VendorPtr = (VoidPtr)vendorTable;
			break;
		}
	}

	// ------------------------------------------ //
	// draw background color.
	// ------------------------------------------ //

	handoverHdrPtr->f_GOP.f_The			 = kGop->Mode->FrameBufferBase;
	handoverHdrPtr->f_GOP.f_Width		 = kGop->Mode->Info->VerticalResolution;
	handoverHdrPtr->f_GOP.f_Height		 = kGop->Mode->Info->HorizontalResolution;
	handoverHdrPtr->f_GOP.f_PixelPerLine = kGop->Mode->Info->PixelsPerScanLine;
	handoverHdrPtr->f_GOP.f_PixelFormat	 = kGop->Mode->Info->PixelFormat;
	handoverHdrPtr->f_GOP.f_Size		 = kGop->Mode->FrameBufferSize;

	auto	guid_mp = EfiGUID(EFI_MP_SERVICES_PROTOCOL_GUID);
	VoidPtr mp		= nullptr;

	extern EfiBootServices* BS;

	BS->LocateProtocol(&guid_mp, nullptr, &mp);

	handoverHdrPtr->f_HardwareTables.f_MPPtr = mp;

	kHandoverHeader = handoverHdrPtr;

	CGInit();
	CGDrawInRegion(CGColor(0xff, 0x3a, 0x3a), handoverHdrPtr->f_GOP.f_Height, handoverHdrPtr->f_GOP.f_Width, 0, 0);
	CGFini();

	cg_write_text("NEWOSLDR (C) ZKA TECHNOLOGIES.", 10, 10, RGB(0xFF, 0xFF, 0xFF));
	cg_write_text(BVersionString::The(), 20, 10, RGB(0xFF, 0xFF, 0xFF));

	// Fill handover header now.

	BS->GetMemoryMap(&SizePtr, Descriptor, &MapKey, &SzDesc, &RevDesc);

	Descriptor = new EfiMemoryDescriptor[SzDesc];
	BS->GetMemoryMap(&SizePtr, Descriptor, &MapKey, &SzDesc, &RevDesc);

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

	handoverHdrPtr->f_HeapStart = nullptr;

	while (BS->AllocatePool(EfiLoaderCode, kHandoverHeapSz, &handoverHdrPtr->f_HeapStart) != kEfiOk)
		;

	handoverHdrPtr->f_VirtualSize =
		Descriptor[cDefaultMemoryMap].NumberOfPages; /* # of pages */

	handoverHdrPtr->f_FirmwareVendorLen = BStrLen(SystemTable->FirmwareVendor);

	handoverHdrPtr->f_Magic	  = kHandoverMagic;
	handoverHdrPtr->f_Version = kHandoverVersion;

	// Provide fimware vendor name.

	BCopyMem(handoverHdrPtr->f_FirmwareVendorName, SystemTable->FirmwareVendor,
			 handoverHdrPtr->f_FirmwareVendorLen);

	handoverHdrPtr->f_FirmwareVendorLen = BStrLen(SystemTable->FirmwareVendor);

	// Assign to global 'kHandoverHeader'.

	BDiskFormatFactory<BootDeviceATA> checkPart;

	if (!checkPart.IsPartitionValid())
	{
		BDiskFormatFactory<BootDeviceATA>::BFileDescriptor root;
		root.fFileName[0] = kNewFSRoot[0];
		root.fFileName[1] = 0;

		root.fKind = kNewFSCatalogKindDir;

		checkPart.Format("ZKA (A:)", &root, 1);

		EFI::Stop();
	}

	// ---------------------------------------------------- //
	// The following checks for an exisiting partition
	// inside the disk, if it doesn't have one,
	// format the disk.
	// ---------------------------------------------------- //

	BFileReader readerBootScr(L"bootscr.sys", ImageHandle);
	readerBootScr.ReadAll(0);

	Boot::BThread* loaderBootScr = nullptr;

	// ------------------------------------------ //
	// If we succeed in reading the blob, then execute it.
	// ------------------------------------------ //

	if (readerBootScr.Blob())
	{
		loaderBootScr = new Boot::BThread(readerBootScr.Blob());
		loaderBootScr->SetName("64-bit Boot Screen DLL.");
	}

	loaderBootScr->Start(handoverHdrPtr);

	BFileReader readerKernel(L"newoskrnl.dll", ImageHandle);

	readerKernel.ReadAll(0);

	Boot::BThread* loader = nullptr;

	// ------------------------------------------ //
	// If we succeed in reading the blob, then execute it.
	// ------------------------------------------ //

	if (readerKernel.Blob())
	{
		loader = new Boot::BThread(readerKernel.Blob());
		loader->SetName("64-bit Kernel DLL.");
	}

	EFI::ExitBootServices(MapKey, ImageHandle);

	// ---------------------------------------------------- //
	// finally load kernel.
	// ---------------------------------------------------- //

	loader->Start(handoverHdrPtr);

	EFI::Stop();

	CANT_REACH();
}
