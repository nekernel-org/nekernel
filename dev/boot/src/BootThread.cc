/* -------------------------------------------

	Copyright (C) 2024-2025, Amlal EL Mahrouss, all rights reserved.

------------------------------------------- */

#include <BootKit/BootThread.h>
#include <BootKit/Support.h>
#include <BootKit/BootKit.h>
#include <FirmwareKit/EFI/API.h>

#include <KernelKit/PEF.h>
#include <KernelKit/PE.h>
#include <KernelKit/MSDOS.h>
#include <CFKit/Utils.h>
#include <modules/CoreGfx/TextMgr.h>

/// @brief External boot services symbol.
EXTERN EfiBootServices* BS;

/// @note BootThread doesn't parse the symbols so doesn't nullify them, .bss is though.

namespace Boot
{
	EXTERN_C Int32 rt_jump_to_address(VoidPtr code, HEL::BootInfoHeader* handover, UInt8* stack);

	BootThread::BootThread(VoidPtr blob)
		: fBlob(blob), fStartAddress(nullptr)
	{
		// detect the format.
		const Char* blob_bytes = reinterpret_cast<char*>(fBlob);

		BootTextWriter writer;

		if (!blob_bytes)
		{
			// failed to provide a valid pointer.
			return;
		}

		if (blob_bytes[0] == kMagMz0 &&
			blob_bytes[1] == kMagMz1)
		{
			LDR_EXEC_HEADER_PTR		header_ptr	   = CF::ldr_find_exec_header(blob_bytes);
			LDR_OPTIONAL_HEADER_PTR opt_header_ptr = CF::ldr_find_opt_exec_header(blob_bytes);

			if (!header_ptr || !opt_header_ptr)
				return;

#ifdef __NE_AMD64__
			if (header_ptr->Machine != kPeMachineAMD64 ||
				header_ptr->Signature != kPeSignature)
			{
				writer.Write("BootZ: Not a PE32+ executable.\r");
				return;
			}
#elif defined(__NE_ARM64__)
			if (header_ptr->Machine != kPeMachineARM64 ||
				header_ptr->Signature != kPeSignature)
			{
				writer.Write("BootZ: Not a PE32+ executable.\r");
				return;
			}
#endif // __NE_AMD64__ || __NE_ARM64__

			writer.Write("BootZ: PE32+ executable detected (NeKernel Subsystem).\r");

			auto numSecs = header_ptr->NumberOfSections;

			writer.Write("BootZ: Major Linker Ver: ").Write(opt_header_ptr->MajorLinkerVersion).Write("\r");
			writer.Write("BootZ: Minor Linker Ver: ").Write(opt_header_ptr->MinorLinkerVersion).Write("\r");
			writer.Write("BootZ: Major Subsystem Ver: ").Write(opt_header_ptr->MajorSubsystemVersion).Write("\r");
			writer.Write("BootZ: Minor Subsystem Ver: ").Write(opt_header_ptr->MinorSubsystemVersion).Write("\r");
			writer.Write("BootZ: Magic: ").Write(header_ptr->Signature).Write("\r");

			constexpr auto cPageSize = 512;

			EfiPhysicalAddress loadStartAddress = opt_header_ptr->ImageBase;
			loadStartAddress += opt_header_ptr->BaseOfData;

			writer.Write("BootZ: Image base: ").Write(loadStartAddress).Write("\r");

			auto numPages = opt_header_ptr->SizeOfImage / cPageSize;
			BS->AllocatePages(AllocateAddress, EfiLoaderData, numPages, &loadStartAddress);

			LDR_SECTION_HEADER_PTR sectPtr = (LDR_SECTION_HEADER_PTR)(((Char*)opt_header_ptr) + header_ptr->SizeOfOptionalHeader);

			constexpr auto sectionForCode	= ".text";
			constexpr auto sectionForNewLdr = ".ldr";
			constexpr auto sectionForBSS	= ".bss";

			for (SizeT sectIndex = 0; sectIndex < numSecs; ++sectIndex)
			{
				LDR_SECTION_HEADER_PTR sect = &sectPtr[sectIndex];

				SetMem((VoidPtr)(loadStartAddress + sect->VirtualAddress), 0, sect->SizeOfRawData);

				if (StrCmp(sectionForCode, sect->Name) == 0)
				{
					fStartAddress = (VoidPtr)((UIntPtr)loadStartAddress + opt_header_ptr->AddressOfEntryPoint);
					writer.Write("BootZ: Executable entry address: ").Write((UIntPtr)fStartAddress).Write("\r");
				}
				else if (StrCmp(sectionForNewLdr, sect->Name) == 0)
				{
					struct HANDOVER_INFORMATION_STUB
					{
						UInt64 HandoverMagic;
						UInt32 HandoverType;
						UInt32 HandoverPad;
						UInt32 HandoverArch;
					}* handover_struc = (struct HANDOVER_INFORMATION_STUB*)((UIntPtr)fBlob + sect->PointerToRawData);

					if (handover_struc->HandoverMagic != kHandoverMagic &&
						handover_struc->HandoverType != HEL::kTypeKernel)
					{
#ifdef __NE_AMD64__
						if (handover_struc->HandoverArch != HEL::kArchAMD64)
						{
							fb_render_string("BootZ: Not an handover header, bad CPU...", 40, 10, RGB(0xFF, 0xFF, 0xFF));
						}
#elif defined(__NE_ARM64__)
						if (handover_struc->HandoverArch != HEL::kArchARM64)
						{
							fb_render_string("BootZ: Not an handover header, bad CPU...", 40, 10, RGB(0xFF, 0xFF, 0xFF));
						}
#endif

						fb_render_string("BootZ: Not an handover header...", 40, 10, RGB(0xFF, 0xFF, 0xFF));
						::Boot::Stop();
					}
				}

				writer.Write("BootZ: Raw offset: ").Write(sect->PointerToRawData).Write(" of ").Write(sect->Name).Write("\r");

				CopyMem((VoidPtr)(loadStartAddress + sect->VirtualAddress), (VoidPtr)((UIntPtr)fBlob + sect->PointerToRawData), sect->SizeOfRawData);
			}
		}
		else if (blob_bytes[0] == kPefMagic[0] &&
				 blob_bytes[1] == kPefMagic[1] &&
				 blob_bytes[2] == kPefMagic[2] &&
				 blob_bytes[3] == kPefMagic[3])
		{
			//  =========================================  //
			//  PEF executable has been detected.
			//  =========================================  //

			fStartAddress = nullptr;
			writer.Write("BootZ: PEF executable detected, won't load it.\r");
			writer.Write("BootZ: note: PEF executables aren't loadable by default.\r");
		}
		else
		{
			writer.Write("BootZ: Invalid Executable.\r");
		}

		fStack = new UInt8[mib_cast(16)];
	}

	/// @note handover header has to be valid!
	Int32 BootThread::Start(HEL::BootInfoHeader* handover, Bool own_stack)
	{
		HEL::HandoverProc err_fn = [](HEL::BootInfoHeader* rcx) -> Int32 {
			fb_render_string("BootZ: Invalid Boot Image...", 50, 10, RGB(0xFF, 0xFF, 0xFF));
			::Boot::Stop();

			return NO;
		};

		if (!fStartAddress)
		{
			err_fn(handover);
		}

		fHandover = handover;

		if (own_stack)
		{
			rt_jump_to_address(fStartAddress, fHandover, &fStack[mib_cast(16) - 1]);
		}
		else
		{
			if (fStack)
				delete[] fStack;

			fStack = nullptr;

			return reinterpret_cast<HEL::HandoverProc>(fStartAddress)(fHandover);
		}

		return kEfiOk;
	}

	const Char* BootThread::GetName()
	{
		return fBlobName;
	}

	Void BootThread::SetName(const Char* name)
	{
		CopyMem(fBlobName, name, StrLen(name));
	}

	bool BootThread::IsValid()
	{
		return fStartAddress != nullptr;
	}
} // namespace Boot
