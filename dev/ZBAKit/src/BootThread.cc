/* -------------------------------------------

	Copyright (C) 2024, Amlal EL Mahrouss, all rights reserved.

------------------------------------------- */

#include <BootKit/Thread.h>
#include <BootKit/Support.h>
#include <BootKit/BootKit.h>
#include <FirmwareKit/EFI/API.h>

#include <KernelKit/PEF.h>
#include <KernelKit/PE.h>
#include <KernelKit/MSDOS.h>
#include <CFKit/Utils.h>
#include <Modules/FB/Text.h>

/// @brief External boot services symbol.
EXTERN EfiBootServices* BS;

/// @note BThread doesn't parse the symbols so doesn't nullify them, .bss is though.

namespace Boot
{
	EXTERN_C Void rt_jump_to_address(VoidPtr code, HEL::BootInfoHeader* handover, UInt8* stack);

	BThread::BThread(VoidPtr blob)
		: fBlob(blob), fStartAddress(nullptr)
	{
		// detect the format.
		const Char* blob_bytes = reinterpret_cast<char*>(fBlob);

		BTextWriter writer;

		if (!blob_bytes)
		{
			// failed to provide a valid pointer.
			return;
		}

		if (blob_bytes[0] == kMagMz0 &&
			blob_bytes[1] == kMagMz1)
		{
			LDR_EXEC_HEADER_PTR		header_ptr	   = CFKit::ldr_find_exec_header(blob_bytes);
			LDR_OPTIONAL_HEADER_PTR opt_header_ptr = CFKit::ldr_find_opt_exec_header(blob_bytes);

			if (!header_ptr || !opt_header_ptr)
				return;

#ifdef __ZKA_AMD64__
			if (header_ptr->mMachine != kPeMachineAMD64 ||
				header_ptr->mSignature != kPeSignature)
			{
				writer.Write("ZBA: Not a PE32+ executable.\r");
				return;
			}
#elif defined(__ZKA_ARM64__)
			if (header_ptr->mMachine != kPeMachineAMD64 ||
				header_ptr->mSignature != kPeSignature)
			{
				writer.Write("ZBA: Not a PE32+ executable.\r");
				return;
			}
#endif // __ZKA_AMD64__ || __ZKA_ARM64__

			if (opt_header_ptr->mSubsystem != kZKASubsystem)
			{
				writer.Write("ZBA: Not a ZKA Subsystem executable.\r");
				return;
			}

			writer.Write("ZBA: PE32+ executable detected (ZKA Subsystem).\r");

			auto numSecs = header_ptr->mNumberOfSections;

			writer.Write("ZBA: Major Linker Ver: ").Write(opt_header_ptr->mMajorLinkerVersion).Write("\r");
			writer.Write("ZBA: Minor Linker Ver: ").Write(opt_header_ptr->mMinorLinkerVersion).Write("\r");
			writer.Write("ZBA: Major Subsystem Ver: ").Write(opt_header_ptr->mMajorSubsystemVersion).Write("\r");
			writer.Write("ZBA: Minor Subsystem Ver: ").Write(opt_header_ptr->mMinorSubsystemVersion).Write("\r");
			writer.Write("ZBA: Magic: ").Write(header_ptr->mSignature).Write("\r");

			constexpr auto cPageSize = 512;

			EfiPhysicalAddress loadStartAddress = opt_header_ptr->mImageBase;
			loadStartAddress += opt_header_ptr->mBaseOfData;

			writer.Write("ZBA: ImageBase: ").Write(loadStartAddress).Write("\r");

			auto numPages = opt_header_ptr->mSizeOfImage / cPageSize;
			BS->AllocatePages(AllocateAddress, EfiLoaderData, numPages, &loadStartAddress);

			LDR_SECTION_HEADER_PTR sectPtr = (LDR_SECTION_HEADER_PTR)(((Char*)opt_header_ptr) + header_ptr->mSizeOfOptionalHeader);

			constexpr auto sectionForCode	= ".text";
			constexpr auto sectionForNewLdr = ".ldr";
			constexpr auto sectionForBSS	= ".bss";

			for (SizeT sectIndex = 0; sectIndex < numSecs; ++sectIndex)
			{
				LDR_SECTION_HEADER_PTR sect = &sectPtr[sectIndex];

				SetMem((VoidPtr)(loadStartAddress + sect->mVirtualAddress), 0, sect->mSizeOfRawData);

				if (StrCmp(sectionForCode, sect->mName) == 0)
				{
					fStartAddress = (VoidPtr)((UIntPtr)loadStartAddress + opt_header_ptr->mAddressOfEntryPoint);
					writer.Write("ZBA: Executable entry address: ").Write((UIntPtr)fStartAddress).Write("\r");
				}
				else if (StrCmp(sectionForNewLdr, sect->mName) == 0)
				{
					struct HANDOVER_INFORMATION_STUB
					{
						UInt64 HandoverMagic;
						UInt32 HandoverType;
						UInt32 HandoverPad;
						UInt32 HandoverArch;
					}* handover_struc = (struct HANDOVER_INFORMATION_STUB*)((UIntPtr)fBlob + sect->mPointerToRawData);

					if (handover_struc->HandoverMagic != kHandoverMagic &&
						handover_struc->HandoverType != HEL::kTypeKernel)
					{
#ifdef __ZKA_AMD64__
						if (handover_struc->HandoverArch != HEL::kArchAMD64)
						{
							CGDrawString("ZBA: NOT AN HANDOVER IMAGE, BAD ARCHITECTURE...", 40, 10, RGB(0xFF, 0xFF, 0xFF));
							::EFI::Stop();
						}
#endif

#ifdef __ZKA_ARM64__
						if (handover_struc->HandoverArch != HEL::kArchARM64)
						{
							CGDrawString("ZBA: NOT AN HANDOVER IMAGE, BAD ARCHITECTURE...", 40, 10, RGB(0xFF, 0xFF, 0xFF));
							::EFI::Stop();
						}
#endif
						CGDrawString("ZBA: NOT AN HANDOVER IMAGE...", 40, 10, RGB(0xFF, 0xFF, 0xFF));

						::EFI::Stop();
					}
				}

				writer.Write("ZBA: Raw offset: ").Write(sect->mPointerToRawData).Write(" of ").Write(sect->mName).Write("\r");

				CopyMem((VoidPtr)(loadStartAddress + sect->mVirtualAddress), (VoidPtr)((UIntPtr)fBlob + sect->mPointerToRawData), sect->mSizeOfRawData);
			}
		}
		else if (blob_bytes[0] == kPefMagic[0] &&
				 blob_bytes[1] == kPefMagic[1] &&
				 blob_bytes[2] == kPefMagic[2] &&
				 blob_bytes[3] == kPefMagic[3])
		{
			//  =========================================  //
			//  PEF executable detected.
			//  =========================================  //

			fStartAddress = nullptr;
			writer.Write("ZBA: PEF executable detected, won't load it.\r");
			writer.Write("ZBA: note: PEF executables aren't loadable by default.\r");
		}
		else
		{
			writer.Write("ZBA: INVALID EXECUTABLE.\r");
		}

		fStack = new UInt8[mib_cast(8)];
	}

	/// @note handover header has to be valid!
	Void BThread::Start(HEL::BootInfoHeader* handover, Bool own_stack)
	{
		HEL::HandoverProc err_fn = [](HEL::BootInfoHeader* rcx) -> void {
			CGDrawString("ZBA: INVALID IMAGE! ABORTING...", 50, 10, RGB(0xFF, 0xFF, 0xFF));
			::EFI::Stop();
		};

		if (!fStartAddress)
		{
			err_fn(handover);
		}

		fHandover = handover;

		if (own_stack)
			rt_jump_to_address(fStartAddress, fHandover, &fStack[mib_cast(8) - 1]);
		else
		{
			delete [] fStack;
			reinterpret_cast<HEL::HandoverProc>(fStartAddress)(fHandover);
		}
	}

	const Char* BThread::GetName()
	{
		return fBlobName;
	}

	Void BThread::SetName(const Char* name)
	{
		CopyMem(fBlobName, name, StrLen(name));
	}

	bool BThread::IsValid()
	{
		return fStartAddress != nullptr;
	}
} // namespace Boot
