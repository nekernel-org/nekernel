/* -------------------------------------------

	Copyright ZKA Web Services Co.

------------------------------------------- */

#include <BootKit/BootKit.hxx>
#include <modules/FB/FB.hxx>
#include <modules/FB/Text.hxx>
#include <FirmwareKit/EFI.hxx>
#include <FirmwareKit/EFI/API.hxx>
#include <FirmwareKit/Handover.hxx>
#include <KernelKit/MSDOS.hxx>
#include <KernelKit/PE.hxx>
#include <KernelKit/PEF.hxx>
#include <NewKit/Macros.hxx>
#include <NewKit/Ref.hxx>
#include <BootKit/Thread.hxx>

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

STATIC EfiGraphicsOutputProtocol* kGop		 = nullptr;
STATIC UInt16					  kGopStride = 0U;
STATIC EfiGUID					  kGopGuid;

EXTERN_C Void rt_reset_hardware();

/**
	@brief Finds and stores the GOP.
*/

EXTERN EfiBootServices* BS;

STATIC Void boot_init_fb() noexcept
{
	kGopGuid = EfiGUID(EFI_GRAPHICS_OUTPUT_PROTOCOL_GUID);
	kGop	 = nullptr;

	BS->LocateProtocol(&kGopGuid, nullptr, (VoidPtr*)&kGop);

	kGopStride = 4;

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

EXTERN_C VoidPtr boot_read_cr3();
EXTERN_C Void	 boot_write_cr3(VoidPtr new_cr3);

EXTERN EfiBootServices* BS;

/// @brief Main EFI entrypoint.
/// @param ImageHandle Handle of this image.
/// @param SystemTable The system table of it.
/// @return nothing, never returns.
EFI_EXTERN_C EFI_API Int32 Main(EfiHandlePtr	ImageHandle,
								EfiSystemTable* SystemTable)
{
	InitEFI(SystemTable); ///! Init the EFI library.

	HEL::HANDOVER_INFO_HEADER* handover_hdr =
		new HEL::HANDOVER_INFO_HEADER();

	UInt32				 map_key		 = 0;
	UInt32				 size_struct_ptr = sizeof(EfiMemoryDescriptor);
	EfiMemoryDescriptor* struct_ptr		 = nullptr;
	UInt32				 sz_desc		 = sizeof(EfiMemoryDescriptor);
	UInt32				 rev_desc		 = 0;

#ifdef __ZKA_USE_FB__
	boot_init_fb(); ///! Init the GOP.

	for (SizeT indexVT = 0; indexVT < SystemTable->NumberOfTableEntries;
		 ++indexVT)
	{
		Char* vendor_table = reinterpret_cast<Char*>(
			SystemTable->ConfigurationTable[indexVT].VendorTable);

		/// ACPI's 'RSD PTR', which contains hardware tables (MADT, FACP...)
		if (vendor_table[0] == 'R' && vendor_table[1] == 'S' &&
			vendor_table[2] == 'D' && vendor_table[3] == ' ' &&
			vendor_table[4] == 'P' && vendor_table[5] == 'T' &&
			vendor_table[6] == 'R' && vendor_table[7] == ' ')
		{
			handover_hdr->f_HardwareTables.f_VendorPtr = (VoidPtr)vendor_table;
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
#endif // __ZKA_USE_FB__

	// ------------------------------------------- //
	// Grab MP services, extended to runtime.	   //
	// ------------------------------------------- //

	auto				   guid_mp = EfiGUID(EFI_MP_SERVICES_PROTOCOL_GUID);
	EfiMpServicesProtocol* mp	   = nullptr;

	BS->LocateProtocol(&guid_mp, nullptr, reinterpret_cast<VoidPtr*>(&mp));

	handover_hdr->f_HardwareTables.f_MpPtr = reinterpret_cast<VoidPtr>(mp);

	kHandoverHeader = handover_hdr;

#ifdef __ZKA_USE_FB__
	CGInit();
	CGDrawInRegion(CGColor(0xFF, 0x3A, 0x3A), handover_hdr->f_GOP.f_Height, handover_hdr->f_GOP.f_Width, 0, 0);
	CGFini();
#endif // __ZKA_USE_FB__

	UInt32 cnt_enabled	= 0;
	UInt32 cnt_disabled = 0;

	mp->GetNumberOfProcessors(mp, &cnt_disabled, &cnt_enabled);

#ifdef __ZKA_USE_FB__
	CGDrawString("NEWOSLDR (C) ZKA WEB SERVICES.", 10, 10, RGB(0xFF, 0xFF, 0xFF));
	CGDrawString((cnt_enabled > 1) ? "MULTIPLE PROCESSORS DETECTED." : "SINGLE PROCESSOR DETECTED.", 20, 10, RGB(0xFF, 0xFF, 0xFF));
#endif // __ZKA_USE_FB__

	handover_hdr->f_HardwareTables.f_MultiProcessingEnabled = cnt_enabled > 1;
	// Fill handover header now.

	Boot::BDiskFormatFactory<BootDeviceATA> partition_factory;

	// ---------------------------------------------------- //
	// The following checks for an exisiting partition
	// inside the disk, if it doesn't have one,
	// format the disk.
	// ---------------------------------------------------- //

	if (!partition_factory.IsPartitionValid())
	{
		Boot::BDiskFormatFactory<BootDeviceATA>::BFileDescriptor root;
		root.fFileName[0] = kNeFSRoot[0];
		root.fFileName[1] = 0;

		root.fKind = kNeFSCatalogKindDir;

		partition_factory.Format("FileSystem (A:)", &root, 1);

		rt_reset_hardware();
	}

	BS->GetMemoryMap(&size_struct_ptr, struct_ptr, &map_key, &sz_desc, &rev_desc);

	struct_ptr = new EfiMemoryDescriptor[sz_desc];
	BS->GetMemoryMap(&size_struct_ptr, struct_ptr, &map_key, &sz_desc, &rev_desc);

	auto cDefaultMemoryMap = 0; // Grab any usable entries.

	//-----------------------------------------------------------//
	// A simple loop which finds a usable memory region for us.
	//-----------------------------------------------------------//

	SizeT lookup_index = 0UL;

	for (; struct_ptr[lookup_index].Kind != EfiMemoryType::EfiConventionalMemory; ++lookup_index)
	{
		ZKA_UNUSED(0);
	}

	cDefaultMemoryMap = lookup_index;

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

	Boot::BFileReader reader_syschk(L"syschk.sys", ImageHandle);
	reader_syschk.ReadAll(0);

	Boot::BThread* syschk_thread = nullptr;

	// ------------------------------------------ //
	// If we succeed in reading the blob, then execute it.
	// ------------------------------------------ //

	if (reader_syschk.Blob())
	{
		syschk_thread = new Boot::BThread(reader_syschk.Blob());
		syschk_thread->SetName("System Check.");
	}

	syschk_thread->Start(handover_hdr);

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

	Boot::BFileReader reader_kernel(L"minoskrnl.exe", ImageHandle);

	reader_kernel.ReadAll(0);

	Boot::BThread* kernel_thread = nullptr;

	// ------------------------------------------ //
	// If we succeed in reading the blob, then execute it.
	// ------------------------------------------ //

	if (reader_kernel.Blob())
	{
		kernel_thread = new Boot::BThread(reader_kernel.Blob());
		kernel_thread->SetName("Minimal Kernel.");

		handover_hdr->f_KernelImage = reader_kernel.Blob();
	}
	else
	{
#ifdef __ZKA_USE_FB__
		CGDrawString("NEWOSLDR: PLEASE RECOVER YOUR KERNEL IMAGE.", 30, 10, RGB(0xFF, 0xFF, 0xFF));
#endif // __ZKA_USE_FB__
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
		handover_hdr->f_KernelImage	   = reader_kernel.Blob();
		handover_hdr->f_KernelSz	   = reader_kernel.Size();
		handover_hdr->f_TTFallbackFont = urbanistTTF.Blob();
		handover_hdr->f_FontSz		   = urbanistTTF.Size();
	}
	else
	{
#ifdef __ZKA_USE_FB__
		CGDrawString("NEWOSLDR: ONE OR MORE SYSTEM COMPONENTS ARE MISSING, PLEASE REINSTALL THE OS.", 30, 10, RGB(0xFF, 0xFF, 0xFF));
#endif // __ZKA_USE_FB__
	}

	EFI::ExitBootServices(map_key, ImageHandle);

#ifdef __ZKA_USE_FB__
	CGDrawInRegion(CGColor(0xFF, 0x3A, 0x3A), handover_hdr->f_GOP.f_Height, handover_hdr->f_GOP.f_Width, 0, 0);
	CGFini();
#endif // __ZKA_USE_FB__

	// ---------------------------------------------------- //
	// Finally load Kernel, and the cr3 to it.
	// ---------------------------------------------------- //

	kernel_thread->Start(handover_hdr);

	CANT_REACH();
}
