/* -------------------------------------------

	Copyright (C) 2024, Theater Quality Inc, all rights reserved.

------------------------------------------- */

#include <BootKit/BootKit.h>
#include <Modules/FB/FB.h>
#include <Modules/FB/Text.h>
#include <FirmwareKit/EFI.h>
#include <FirmwareKit/EFI/API.h>
#include <FirmwareKit/Handover.h>
#include <KernelKit/MSDOS.h>
#include <KernelKit/PE.h>
#include <KernelKit/PEF.h>
#include <NewKit/Macros.h>
#include <NewKit/Ref.h>
#include <BootKit/Thread.h>
#include <Modules/FB/FB.h>

// Makes the compiler shut up.
#ifndef kMachineModel
#define kMachineModel "ZkaOS"
#endif // !kMachineModel

#ifndef kExpectedWidth
#define kExpectedWidth 1920
#endif

#ifndef kExpectedHeight
#define kExpectedHeight 1080
#endif

/** Graphics related. */

STATIC EfiGraphicsOutputProtocol* kGop		 = nullptr;
STATIC UInt16					  kGopStride = 0U;
STATIC EfiGUID					  kGopGuid;

EXTERN_C Void rt_reset_hardware();

EXTERN EfiBootServices* BS;

/**
	@brief Finds and stores the GOP object.
*/
STATIC Bool boot_init_fb() noexcept
{
	kGopGuid = EfiGUID(EFI_GRAPHICS_OUTPUT_PROTOCOL_GUID);
	kGop	 = nullptr;

	if (BS->LocateProtocol(&kGopGuid, nullptr, (VoidPtr*)&kGop) != kEfiOk)
		return No;

	kGopStride = 4;

	for (SizeT i = 0; i < kGop->Mode->MaxMode; ++i)
	{
		EfiGraphicsOutputProtocolModeInformation* infoPtr = nullptr;
		UInt32									  sz	  = 0U;

		kGop->QueryMode(kGop, i, &sz, &infoPtr);

		if (infoPtr->HorizontalResolution == kExpectedWidth &&
			infoPtr->VerticalResolution == kExpectedHeight)
		{
			kGop->SetMode(kGop, i);
			return Yes;
		}
	}

	return No;
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

	HEL::BootInfoHeader* handover_hdr =
		new HEL::BootInfoHeader();

	UInt32				 map_key		 = 0;
	UInt32				 size_struct_ptr = sizeof(EfiMemoryDescriptor);
	EfiMemoryDescriptor* struct_ptr		 = nullptr;
	UInt32				 sz_desc		 = sizeof(EfiMemoryDescriptor);
	UInt32				 rev_desc		 = 0;

#ifdef ZBA_USE_FB
	if (!boot_init_fb())
		return 1; ///! Init the GOP.

	for (SizeT index_vt = 0; index_vt < SystemTable->NumberOfTableEntries;
		 ++index_vt)
	{
		Char* vendor_table = reinterpret_cast<Char*>(
			SystemTable->ConfigurationTable[index_vt].VendorTable);

		// ACPI's 'RSD PTR', which contains the ACPI SDT (MADT, FACP...)
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
#endif // ZBA_USE_FB

	// ------------------------------------------- //
	// Grab MP services, extended to runtime.	   //
	// ------------------------------------------- //

	auto				   guid_mp = EfiGUID(EFI_MP_SERVICES_PROTOCOL_GUID);
	EfiMpServicesProtocol* mp	   = nullptr;

	BS->LocateProtocol(&guid_mp, nullptr, reinterpret_cast<VoidPtr*>(&mp));

	handover_hdr->f_HardwareTables.f_MpPtr = reinterpret_cast<VoidPtr>(mp);

	kHandoverHeader = handover_hdr;

	cg_init();

	CG::CGDrawBackground();

	CGDrawBitMapInRegion(zka_disk, ZKA_DISK_HEIGHT, ZKA_DISK_WIDTH, (kHandoverHeader->f_GOP.f_Width - ZKA_DISK_WIDTH) / 2, (kHandoverHeader->f_GOP.f_Height - ZKA_DISK_HEIGHT) / 2);

	cg_fini();

	UInt32 cnt_enabled	= 0;
	UInt32 cnt_disabled = 0;

	mp->GetNumberOfProcessors(mp, &cnt_disabled, &cnt_enabled);

#ifdef ZBA_USE_FB
	CGDrawString("BootZ (c) 2024 Theater Quality Inc.", 10, 10, RGB(0xFF, 0xFF, 0xFF));
	CGDrawString((cnt_enabled > 1) ? "Multi processor configuration detected." : "Single processor configuration detected.", 20, 10, RGB(0xFF, 0xFF, 0xFF));
#endif // ZBA_USE_FB

	handover_hdr->f_HardwareTables.f_MultiProcessingEnabled = cnt_enabled > 1;
	// Fill handover header now.

	Boot::BDiskFormatFactory<BootDeviceATA> partition_factory;

	// ---------------------------------------------------- //
	// The following checks for an exisiting partition
	// inside the disk, if it doesn't have one,
	// format the disk.
	// ---------------------------------------------------- //

#ifdef ZKA_AUTO_FORMAT
	if (!partition_factory.IsPartitionValid())
	{
		CG::CGDrawBackground();

		CGDrawBitMapInRegion(zka_no_disk, ZKA_NO_DISK_HEIGHT, ZKA_NO_DISK_WIDTH, (kHandoverHeader->f_GOP.f_Width - ZKA_NO_DISK_WIDTH) / 2, (kHandoverHeader->f_GOP.f_Height - ZKA_NO_DISK_HEIGHT) / 2);

		cg_fini();

		CGDrawString("Formatting EPM disk...", 30, 10, RGB(0xFF, 0xFF, 0xFF));

		Boot::BDiskFormatFactory<BootDeviceATA>::BFileDescriptor root;

		root.fFileName[0] = kNeFSRoot[0];
		root.fFileName[1] = 0;

		root.fKind = kNeFSCatalogKindDir;

		partition_factory.Format("FileSystem (A:)\0", &root, 1);
		
		CG::CGDrawBackground();

		CGDrawBitMapInRegion(zka_has_disk, ZKA_HAS_DISK_HEIGHT, ZKA_HAS_DISK_WIDTH, (kHandoverHeader->f_GOP.f_Width - ZKA_HAS_DISK_WIDTH) / 2, (kHandoverHeader->f_GOP.f_Height - ZKA_HAS_DISK_HEIGHT) / 2);

		cg_fini();
	}
#endif // ZKA_AUTO_FORMAT

	BS->GetMemoryMap(&size_struct_ptr, struct_ptr, &map_key, &sz_desc, &rev_desc);

	struct_ptr = new EfiMemoryDescriptor[sz_desc];

	BS->GetMemoryMap(&size_struct_ptr, struct_ptr, &map_key, &sz_desc, &rev_desc);

	auto kDefaultMemoryMap = 0; // Grab any usable entries.

	//-----------------------------------------------------------//
	// A simple loop which finds a usable memory region for us.
	//-----------------------------------------------------------//

	SizeT lookup_index = 0UL;

	for (; struct_ptr[lookup_index].Kind != EfiMemoryType::EfiConventionalMemory; ++lookup_index)
	{
		ZKA_UNUSED(0);
	}

	kDefaultMemoryMap = lookup_index;

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
		syschk_thread->SetName("System Check (BootZ EFI Driver)");
	}

	syschk_thread->Start(handover_hdr, NO);

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
		kernel_thread->SetName("Minimal OS Kernel.");

		handover_hdr->f_KernelImage = reader_kernel.Blob();
	}
	else
	{
#ifdef ZBA_USE_FB
		CGDrawString("BootZ: Please recover your kernel image.", 30, 10, RGB(0xFF, 0xFF, 0xFF));
#endif // ZBA_USE_FB

		EFI::Stop();
	}

	Boot::BFileReader chime_wav(L"zka\\startup.wav", ImageHandle);
	Boot::BFileReader ttf_font(L"zka\\urbanist.ttf", ImageHandle);

	chime_wav.ReadAll(0);
	ttf_font.ReadAll(0);

	if (chime_wav.Blob() &&
		ttf_font.Blob())
	{
		handover_hdr->f_StartupChime   = chime_wav.Blob();
		handover_hdr->f_ChimeSz		   = chime_wav.Size();
		handover_hdr->f_KernelImage	   = reader_kernel.Blob();
		handover_hdr->f_KernelSz	   = reader_kernel.Size();
		handover_hdr->f_TTFallbackFont = ttf_font.Blob();
		handover_hdr->f_FontSz		   = ttf_font.Size();
	}
	else
	{
#ifdef ZBA_USE_FB
		CGDrawString("BootZ: OS resources are not present, please reinstall the OS.", 30, 10, RGB(0xFF, 0xFF, 0xFF));
#endif // ZBA_USE_FB

		EFI::Stop();
	}

	EFI::ExitBootServices(map_key, ImageHandle);

	// ---------------------------------------------------- //
	// Finally load the OS kernel.
	// ---------------------------------------------------- //

	kernel_thread->Start(handover_hdr, YES);

	CANT_REACH();
}
