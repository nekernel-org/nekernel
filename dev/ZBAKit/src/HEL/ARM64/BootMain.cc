/* -------------------------------------------

	Copyright (C) 2024, Theater Quality Inc, all rights reserved.

------------------------------------------- */

#include <FirmwareKit/EFI/API.h>
#include <FirmwareKit/EFI.h>
#include <BootKit/Thread.h>
#include <BootKit/BootKit.h>

#ifndef kExpectedWidth
#define kExpectedWidth 844
#endif

#ifndef kExpectedHeight
#define kExpectedHeight 390
#endif

EXTERN EfiBootServices* BS;

STATIC EfiGraphicsOutputProtocol* kGop		 = nullptr;
STATIC UInt16					  kGopStride = 0U;
STATIC EfiGUID					  kGopGuid;

/// @brief Main EFI entrypoint.
/// @param ImageHandle Handle of this image.
/// @param SystemTable The system table of it.
/// @return nothing, never returns.
EFI_EXTERN_C EFI_API Int32 Main(EfiHandlePtr	ImageHandle,
								EfiSystemTable* SystemTable)
{
	InitEFI(SystemTable);

	kGopGuid = EfiGUID(EFI_GRAPHICS_OUTPUT_PROTOCOL_GUID);
	kGop	 = nullptr;

	BS->LocateProtocol(&kGopGuid, nullptr, (VoidPtr*)&kGop);

	kGopStride = 4;

	Boot::BTextWriter writer;

	for (SizeT i = 0; i < kGop->Mode->MaxMode; ++i)
	{
		EfiGraphicsOutputProtocolModeInformation* infoPtr = nullptr;
		UInt32									  sz	  = 0U;

		kGop->QueryMode(kGop, i, &sz, &infoPtr);

		writer.Write(infoPtr->HorizontalResolution);
		writer.Write(infoPtr->VerticalResolution);
		writer.Write("\r");

		if (infoPtr->HorizontalResolution == kExpectedWidth &&
			infoPtr->VerticalResolution == kExpectedHeight)
		{
			kGop->SetMode(kGop, i);
			break;
		}
	}

	Boot::BFileReader reader_kernel(L"minoskrnl.exe", ImageHandle);

	reader_kernel.ReadAll(0);

	if (reader_kernel.Blob())
	{
		auto kernel_thread = Boot::BThread(reader_kernel.Blob());

		if (kernel_thread.IsValid())
			kernel_thread.Start(nullptr, YES);
	}

	CANT_REACH();
}
