/* -------------------------------------------

	Copyright ZKA Technologies.

------------------------------------------- */

#include <BootKit/BootKit.hxx>
#include <BootKit/Rsrc/NewBoot.rsrc>
#include <Modules/FB/FB.hxx>
#include <Modules/FB/Text.hxx>
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

EXTERN_C VoidPtr boot_read_cr3();
EXTERN_C Void	 boot_write_cr3(VoidPtr new_cr3);

EXTERN EfiBootServices* BS;

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

	HEL::HANDOVER_INFO_HEADER* handover_hdr =
		new HEL::HANDOVER_INFO_HEADER();

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
			handover_hdr->f_HardwareTables.f_VendorPtr = (VoidPtr)vendorTable;
			break;
		}
	}

	// ------------------------------------------ //
	// draw background color.
	// ------------------------------------------ //

	handover_hdr->f_GOP.f_The		   = kGop->Mode->FrameBufferBase;
	handover_hdr->f_GOP.f_Width		   = kGop->Mode->Info->VerticalResolution;
	handover_hdr->f_GOP.f_Height	   = kGop->Mode->Info->HorizontalResolution;
	handover_hdr->f_GOP.f_PixelPerLine = kGop->Mode->Info->PixelsPerScanLine;
	handover_hdr->f_GOP.f_PixelFormat  = kGop->Mode->Info->PixelFormat;
	handover_hdr->f_GOP.f_Size		   = kGop->Mode->FrameBufferSize;

	// ------------------------------------------- //
	// Grab MP services, extended to runtime.	   //
	// ------------------------------------------- //

	auto				   guid_mp = EfiGUID(EFI_MP_SERVICES_PROTOCOL_GUID);
	EfiMpServicesProtocol* mp	   = nullptr;

	BS->LocateProtocol(&guid_mp, nullptr, reinterpret_cast<VoidPtr*>(&mp));

	handover_hdr->f_HardwareTables.f_MpPtr = reinterpret_cast<VoidPtr>(mp);

	kHandoverHeader = handover_hdr;

	CGInit();
	CGDrawInRegion(CGColor(0xFF, 0x3A, 0x3A), handover_hdr->f_GOP.f_Height, handover_hdr->f_GOP.f_Width, 0, 0);
	CGFini();

	UInt32 cnt_enabled	= 0;
	UInt32 cnt_disabled = 0;

	mp->GetNumberOfProcessors(mp, &cnt_disabled, &cnt_enabled);

	CGDrawString("NEWOSLDR (C) ZKA TECHNOLOGIES.", 10, 10, RGB(0xFF, 0xFF, 0xFF));
	CGDrawString((cnt_enabled > 1) ? "MULTIPLE PROCESSORS DETECTED." : "SINGLE PROCESSOR DETECTED.", 20, 10, RGB(0xFF, 0xFF, 0xFF));

	handover_hdr->f_HardwareTables.f_MultiProcessingEnabled = cnt_enabled > 1;
	// Fill handover header now.

	Boot::BDiskFormatFactory<BootDeviceATA> checkPart;

	// ---------------------------------------------------- //
	// The following checks for an exisiting partition
	// inside the disk, if it doesn't have one,
	// format the disk.
	// ---------------------------------------------------- //

	if (!checkPart.IsPartitionValid())
	{
		Boot::BDiskFormatFactory<BootDeviceATA>::BFileDescriptor root;
		root.fFileName[0] = kNeFSRoot[0];
		root.fFileName[1] = 0;

		root.fKind = kNeFSCatalogKindDir;

		checkPart.Format("FileSystem (A:)", &root, 1);

		rt_reset_hardware();
	}

	BS->GetMemoryMap(&SizePtr, Descriptor, &MapKey, &SzDesc, &RevDesc);

	Descriptor = new EfiMemoryDescriptor[SzDesc];
	BS->GetMemoryMap(&SizePtr, Descriptor, &MapKey, &SzDesc, &RevDesc);

	auto cDefaultMemoryMap = 0; // Grab any usable entries.

	//-----------------------------------------------------------//
	// A simple loop which finds a usable memory region for us.
	//-----------------------------------------------------------//

	SizeT lookIndex = 0UL;

	for (; Descriptor[lookIndex].Kind != EfiMemoryType::EfiConventionalMemory; ++lookIndex)
	{
		ZKA_UNUSED(0);
	}

	cDefaultMemoryMap = lookIndex;

	//-----------------------------------------------------------//
	// Update handover file specific table and phyiscal start field.
	//-----------------------------------------------------------//

	handover_hdr->f_BitMapStart = nullptr;			 /* Start of bitmap. */
	handover_hdr->f_BitMapSize	= kHandoverBitMapSz; /* Size of bitmap. */

	while (BS->AllocatePool(EfiLoaderData, handover_hdr->f_BitMapSize, &handover_hdr->f_BitMapStart) != kEfiOk)
	{
		if (handover_hdr->f_BitMapStart)
		{
			BS->FreePool(handover_hdr->f_BitMapStart);
			handover_hdr->f_BitMapStart = nullptr;
		}
	}

	handover_hdr->f_FirmwareCustomTables[0] = (VoidPtr)BS;
	handover_hdr->f_FirmwareCustomTables[1] = (VoidPtr)ST;

	Boot::BFileReader readerSysChk(L"syschk.sys", ImageHandle);
	readerSysChk.ReadAll(0);

	Boot::BThread* loaderSysChk = nullptr;

	// ------------------------------------------ //
	// If we succeed in reading the blob, then execute it.
	// ------------------------------------------ //

	if (readerSysChk.Blob())
	{
		loaderSysChk = new Boot::BThread(readerSysChk.Blob());
		loaderSysChk->SetName("System Check SYS.");
	}

	loaderSysChk->Start(handover_hdr);

	// nullify these fields, to avoid being reused later.

	handover_hdr->f_FirmwareCustomTables[0] = nullptr;
	handover_hdr->f_FirmwareCustomTables[1] = nullptr;

	handover_hdr->f_FirmwareVendorLen = Boot::BStrLen(SystemTable->FirmwareVendor);

	handover_hdr->f_Magic	= kHandoverMagic;
	handover_hdr->f_Version = kHandoverVersion;

	// Provide fimware vendor name.

	Boot::BCopyMem(handover_hdr->f_FirmwareVendorName, SystemTable->FirmwareVendor,
				   handover_hdr->f_FirmwareVendorLen);

	handover_hdr->f_FirmwareVendorLen = Boot::BStrLen(SystemTable->FirmwareVendor);

	// Assign to global 'kHandoverHeader'.

	Boot::BFileReader readerKernel(L"newoskrnl.exe", ImageHandle);

	readerKernel.ReadAll(0);

	Boot::BThread* loader = nullptr;

	// ------------------------------------------ //
	// If we succeed in reading the blob, then execute it.
	// ------------------------------------------ //

	if (readerKernel.Blob())
	{
		loader = new Boot::BThread(readerKernel.Blob());
		loader->SetName("64-Bit Kernel executable.");

		handover_hdr->f_KernelImage = readerKernel.Blob();
	}
	else
	{
		CGDrawString("NEWOSLDR: PLEASE RECOVER YOUR NEWOSKRNL KERNEL IMAGE.", 30, 10, RGB(0xFF, 0xFF, 0xFF));
	}

	Boot::BFileReader chimeWav(L"zka\\startup.wav", ImageHandle);
	Boot::BFileReader readerSysDrv(L"zka\\startup.sys", ImageHandle);
	Boot::BFileReader urbanistTTF(L"zka\\urbanist.ttf", ImageHandle);

	readerSysDrv.ReadAll(0);
	chimeWav.ReadAll(0);
	urbanistTTF.ReadAll(0);

	if (readerSysDrv.Blob() &&
		chimeWav.Blob() &&
		urbanistTTF.Blob())
	{
		handover_hdr->f_StartupChime   = chimeWav.Blob();
		handover_hdr->f_ChimeSz		   = chimeWav.Size();
		handover_hdr->f_StartupImage   = readerSysDrv.Blob();
		handover_hdr->f_StartupSz	   = readerSysDrv.Size();
		handover_hdr->f_KernelImage	   = readerKernel.Blob();
		handover_hdr->f_KernelSz	   = readerKernel.Size();
		handover_hdr->f_TTFallbackFont = urbanistTTF.Blob();
		handover_hdr->f_FontSz		   = urbanistTTF.Size();
	}
	else
	{
		CGDrawString("NEWOSLDR: ONE OR MORE SYSTEM COMPONENTS ARE MISSING, PLEASE REFORMAT THE OS.", 30, 10, RGB(0xFF, 0xFF, 0xFF));
	}

	EFI::ExitBootServices(MapKey, ImageHandle);

	CGDrawInRegion(CGColor(0xFF, 0x3A, 0x3A), handover_hdr->f_GOP.f_Height, handover_hdr->f_GOP.f_Width, 0, 0);
	CGFini();

	// ---------------------------------------------------- //
	// Finally load Kernel, and the cr3 to it.
	// ---------------------------------------------------- //

	loader->Start(handover_hdr);

	EFI::Stop();

	CANT_REACH();
}
