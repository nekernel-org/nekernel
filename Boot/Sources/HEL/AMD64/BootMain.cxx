/* -------------------------------------------

	Copyright Zeta Electronics Corporation

------------------------------------------- */

#include <BootKit/BootKit.hxx>
#include <BootKit/Rsrc/NewBoot.rsrc>
#include <Builtins/GX/GX>
#include <FirmwareKit/EFI.hxx>
#include <FirmwareKit/EFI/API.hxx>
#include <FirmwareKit/Handover.hxx>
#include <KernelKit/MSDOS.hpp>
#include <KernelKit/PE.hxx>
#include <KernelKit/PEF.hpp>
#include <NewKit/Macros.hpp>
#include <NewKit/Ref.hpp>
#include <cstring>

/// make the compiler shut up.
#ifndef kMachineModel
#define kMachineModel "Zeta HD"
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
		.Write(BVersionString::The()).Write("\r");

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

	GXInit();

	GXDraw(RGB(9d, 9d, 9d), handoverHdrPtr->f_GOP.f_Height,
		   handoverHdrPtr->f_GOP.f_Width, 0, 0);

	GXFini();

	GXDrawImg(NewBoot, NEWBOOT_HEIGHT, NEWBOOT_WIDTH,
			  (handoverHdrPtr->f_GOP.f_Width - NEWBOOT_WIDTH) / 2,
			  (handoverHdrPtr->f_GOP.f_Height - NEWBOOT_HEIGHT) / 2);

	GXFini();

	BS->GetMemoryMap(SizePtr, Descriptor, MapKey, SzDesc, RevDesc);

	Descriptor = new EfiMemoryDescriptor[*SzDesc];
	BS->GetMemoryMap(SizePtr, Descriptor, MapKey, SzDesc, RevDesc);

	writer.Write(L"Kernel-Desc-Count: ");
	writer.Write(*SzDesc);
	writer.Write(L"\r");

	auto cDefaultMemoryMap = 0; /// The sixth entry.

	/// A simple loop which finds a usable memory region for us.
	SizeT i = 0UL;
	for (; Descriptor[i].Kind != EfiMemoryType::EfiConventionalMemory; ++i)
	{
		;
	}

	cDefaultMemoryMap = i;

	writer.Write(L"Number-Of-Pages: ")
		.Write(Descriptor[cDefaultMemoryMap].NumberOfPages)
		.Write(L"\r");
	writer.Write(L"Virtual-Address: ")
		.Write(Descriptor[cDefaultMemoryMap].VirtualStart)
		.Write(L"\r");
	writer.Write(L"Phyiscal-Address: ")
		.Write(Descriptor[cDefaultMemoryMap].PhysicalStart)
		.Write(L"\r");
	writer.Write(L"Page-Kind: ")
		.Write(Descriptor[cDefaultMemoryMap].Kind)
		.Write(L"\r");
	writer.Write(L"Page-Attribute: ")
		.Write(Descriptor[cDefaultMemoryMap].Attribute)
		.Write(L"\r");

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

	BFileReader reader(L"SplashScreen.fmt", ImageHandle);
	reader.ReadAll(512, 16);

	if (reader.Blob())
	{
		Char* buf = (Char*)reader.Blob();

		for (SizeT i = 0; i < reader.Size(); ++i)
		{
			if (buf[i] != '\n' && buf[i] != '\r')
			{
				if (buf[i] == '*')
				{
					writer.WriteCharacter('\t');
				}
				else
				{
					writer.WriteCharacter(buf[i]);
				}
			}
			else
				writer.Write(L"\r");
		}
	}

	///
	/// The following checks for an exisiting partition
	/// inside the disk, if it doesn't have one,
	/// format the disk.
	//

	BDiskFormatFactory<BootDeviceATA> diskFormatter;

	/// if not formated yet, then format it with the following folders:
	/// /, /Boot, /Applications.
	if (!diskFormatter.IsPartitionValid())
	{
		BDiskFormatFactory<BootDeviceATA>::BFileDescriptor rootDesc{0};

		CopyMem(rootDesc.fFileName, kNewFSRoot, StrLen(kNewFSRoot));
		rootDesc.fKind = kNewFSCatalogKindDir;

		diskFormatter.Format(kMachineModel, &rootDesc, 1);
	}

	EFI::ExitBootServices(*MapKey, ImageHandle);

	/// Fallback to builtin kernel.
	hal_init_platform(handoverHdrPtr);

	EFI::Stop();

	CANT_REACH();
}
