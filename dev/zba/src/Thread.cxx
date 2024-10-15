/* -------------------------------------------

	Copyright ZKA Technologies.

------------------------------------------- */

#include <BootKit/Thread.hxx>
#include <BootKit/Support.hxx>
#include <BootKit/BootKit.hxx>
#include <FirmwareKit/EFI/API.hxx>

#include <KernelKit/PEF.hxx>
#include <KernelKit/PE.hxx>
#include <KernelKit/MSDOS.hxx>
#include <CFKit/LoaderUtils.hxx>
#include <modules/FB/Text.hxx>

// External boot services symbol.
EXTERN EfiBootServices* BS;

/// @note BThread doesn't parse the symbols so doesn't nullify them, .bss is though.

namespace Boot
{
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
			LDR_EXEC_HEADER_PTR		header_ptr = ldr_find_exec_header(blob_bytes);
			LDR_OPTIONAL_HEADER_PTR opt_header_ptr = ldr_find_opt_exec_header(blob_bytes);

			if (!header_ptr || !opt_header_ptr)
			    return;

#ifdef __ZKA_AMD64__
			if (header_ptr->mMachine != kPeMachineAMD64 ||
				header_ptr->mSignature != kPeSignature)
			{
				writer.Write("NEWOSLDR: Not a PE32+ executable.\r");
				return;
			}
#elif defined(__ZKA_ARM64__)
			if (header_ptr->mMachine != kPeMachineAMD64 ||
				header_ptr->mSignature != kPeSignature)
			{
				writer.Write("NEWOSLDR: Not a PE32+ executable.\r");
				return;
			}
#endif // __ZKA_AMD64__ || __ZKA_ARM64__

			if (opt_header_ptr->mSubsystem != kZKASubsystem)
			{
				writer.Write("NEWOSLDR: Not a ZKA Subsystem executable.\r");
				return;
			}

			writer.Write("NEWOSLDR: PE32+ executable detected (ZKA Subsystem).\r");

			auto numSecs = header_ptr->mNumberOfSections;

			writer.Write("NEWOSLDR: Major Linker Ver: ").Write(opt_header_ptr->mMajorLinkerVersion).Write("\r");
			writer.Write("NEWOSLDR: Minor Linker Ver: ").Write(opt_header_ptr->mMinorLinkerVersion).Write("\r");
			writer.Write("NEWOSLDR: Major Subsystem Ver: ").Write(opt_header_ptr->mMajorSubsystemVersion).Write("\r");
			writer.Write("NEWOSLDR: Minor Subsystem Ver: ").Write(opt_header_ptr->mMinorSubsystemVersion).Write("\r");
			writer.Write("NEWOSLDR: Magic: ").Write(header_ptr->mSignature).Write("\r");

			constexpr auto cPageSize = 512;

			EfiPhysicalAddress loadStartAddress = opt_header_ptr->mImageBase;
			loadStartAddress += opt_header_ptr->mBaseOfData;

			writer.Write("NEWOSLDR: ImageBase: ").Write(loadStartAddress).Write("\r");

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
					writer.Write("NEWOSLDR: ENTRY OF EXE: ").Write((UIntPtr)fStartAddress).Write("\r");
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
							writer.Write("NEWOSLDR: ARCH OF EXE: ").Write(handover_struc->HandoverArch).Write("\r");
							writer.Write("NEWOSLDR: ENTRY OF EXE: ").Write((UIntPtr)fStartAddress).Write("\r");
							CGDrawString("NEWOSLDR: NOT AN HANDOVER IMAGE, BAD ARCHITECTURE...", 40, 10, RGB(0xFF, 0xFF, 0xFF));
						}
#endif

#ifdef __ZKA_ARM64__
						if (handover_struc->HandoverArch != HEL::kArchARM64)
						{
							writer.Write("NEWOSLDR: ARCH OF EXE: ").Write(handover_struc->HandoverArch).Write("\r");
							writer.Write("NEWOSLDR: ENTRY OF EXE: ").Write((UIntPtr)fStartAddress).Write("\r");
							CGDrawString("NEWOSLDR: NOT AN HANDOVER IMAGE, BAD ARCHITECTURE...", 40, 10, RGB(0xFF, 0xFF, 0xFF));
						}
#endif
						writer.Write("NEWOSLDR: ENTRY OF EXE: ").Write((UIntPtr)fStartAddress).Write("\r");
						CGDrawString("NEWOSLDR: NOT AN HANDOVER IMAGE...", 40, 10, RGB(0xFF, 0xFF, 0xFF));

						::EFI::Stop();
					}
				}

				writer.Write("NEWOSLDR: OFFSET ").Write(sect->mPointerToRawData).Write(" of ").Write(sect->mName).Write("\r");

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
			writer.Write("NEWOSLDR: PEF executable detected, won't load it.\r");
			writer.Write("NEWOSLDR: note: PEF executables aren't loadable by default.\r");
		}
		else
		{
			writer.Write("NEWOSLDR: Invalid executable.\r");
		}
	}

	/// @note handover header has to be valid!
	Void BThread::Start(HEL::HANDOVER_INFO_HEADER* handover)
	{
		HEL::HandoverProc err_fn = [](HEL::HANDOVER_INFO_HEADER* rcx) -> void {
			CGDrawString("NEWOSLDR: INVALID IMAGE! ABORTING...", 50, 10, RGB(0xFF, 0xFF, 0xFF));
			::EFI::Stop();
		};

		if (!fStartAddress)
		{
			err_fn(handover);
		}

		reinterpret_cast<HEL::HandoverProc>(fStartAddress)(handover);
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
