/* -------------------------------------------

	Copyright (C) 2024-2025, Amlal El Mahrouss, all rights reserved.

------------------------------------------- */

#include <BootKit/BootThread.h>
#include <BootKit/Support.h>
#include <BootKit/BootKit.h>
#include <FirmwareKit/EFI/API.h>

#include <KernelKit/PEF.h>
#include <KernelKit/PE.h>
#include <KernelKit/MSDOS.h>
#include <CFKit/Utils.h>
#include <modules/CoreGfx/TextGfx.h>

/// @brief External boot services symbol.
EXTERN EfiBootServices* BS;

/// @note BootThread doesn't parse the symbols so doesn't nullify them, .bss is though.

namespace Boot
{
	EXTERN_C Int32 rt_jump_to_address(VoidPtr code, HEL::BootInfoHeader* handover, UInt8* stack);

	BootThread::BootThread(VoidPtr blob)
		: fStartAddress(nullptr), fBlob(blob)
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

			EfiPhysicalAddress loadStartAddress = opt_header_ptr->ImageBase;
			loadStartAddress += opt_header_ptr->BaseOfData;

			writer.Write("BootZ: Image base: ").Write(loadStartAddress).Write("\r");

			fStack = new UInt8[mib_cast(16)];

			if (!fStack)
			{
				writer.Write("BootZ: Unable to allocate stack.\r");
				return;
			}

			LDR_SECTION_HEADER_PTR sectPtr = (LDR_SECTION_HEADER_PTR)(((Char*)opt_header_ptr) + header_ptr->SizeOfOptionalHeader);

			constexpr auto sectionForCode  = ".text";
			constexpr auto sectionForBootZ = ".ldr";

			for (SizeT sectIndex = 0; sectIndex < numSecs; ++sectIndex)
			{
				LDR_SECTION_HEADER_PTR sect = &sectPtr[sectIndex];

				auto sz = sect->VirtualSize;

				if (sect->SizeOfRawData > 0)
					sz = sect->SizeOfRawData;

				SetMem((VoidPtr)(loadStartAddress + sect->VirtualAddress), 0, sz);

				if (sect->SizeOfRawData > 0)
				{
					CopyMem((VoidPtr)(loadStartAddress + sect->VirtualAddress),
							(VoidPtr)((UIntPtr)fBlob + sect->PointerToRawData),
							sect->SizeOfRawData);
				}

				if (StrCmp(sectionForCode, sect->Name) == 0)
				{
					fStartAddress = (VoidPtr)((UIntPtr)loadStartAddress + opt_header_ptr->AddressOfEntryPoint);
					writer.Write("BootZ: Executable entry address: ").Write((UIntPtr)fStartAddress).Write("\r");
				}
				else if (StrCmp(sectionForBootZ, sect->Name) == 0)
				{
					struct HANDOVER_INFORMATION_STUB
					{
						UInt64 HandoverMagic;
						UInt32 HandoverType;
						UInt32 HandoverPad;
						UInt32 HandoverArch;
					}* handover_struc = (struct HANDOVER_INFORMATION_STUB*)((UIntPtr)fBlob + sect->PointerToRawData);

					if (handover_struc->HandoverMagic != kHandoverMagic)
					{
#ifdef __NE_AMD64__
						if (handover_struc->HandoverArch != HEL::kArchAMD64)
						{
							writer.Write("BootZ: Not an handover header, bad CPU...\r");
						}
#elif defined(__NE_ARM64__)
						if (handover_struc->HandoverArch != HEL::kArchARM64)
						{
							writer.Write("BootZ: Not an handover header, bad CPU...\r");
						}
#endif

						writer.Write("BootZ: Not an handover header...\r");
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
			writer.Write("BootZ: note: PEF executables aren't supported for now.\r");
		}
		else
		{
			writer.Write("BootZ: Invalid Executable.\r");
		}
	}

	/// @note handover header has to be valid!
	Int32 BootThread::Start(HEL::BootInfoHeader* handover, Bool own_stack)
	{
		fHandover = handover;

		if (!fStartAddress)
		{
			return kEfiFail;
		}

		if (!fHandover)
		{
			return kEfiFail;
		}

		BootTextWriter writer;

		writer.Write("BootZ: Starting: ").Write(fBlobName).Write("\r");
		writer.Write("BootZ: Handover address: ").Write((UIntPtr)fHandover).Write("\r");

		if (own_stack)
		{
			writer.Write("BootZ: Using it's own stack.\r");
			writer.Write("BootZ: Stack address: ").Write((UIntPtr)&fStack[mib_cast(16) - 1]).Write("\r");
			writer.Write("BootZ: Stack size: ").Write(mib_cast(16)).Write("\r");

			auto ret = rt_jump_to_address(fStartAddress, fHandover, &fStack[mib_cast(16) - 1]);

			// we don't need the stack anymore.

			delete[] fStack;
			fStack = nullptr;

			return ret;
		}
		else
		{
			writer.Write("BootZ: Using Bootloader's stack.\r");

			return reinterpret_cast<HEL::HandoverProc>(fStartAddress)(fHandover);
		}

		return kEfiFail;
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