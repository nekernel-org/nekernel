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
		const Char* firstBytes = reinterpret_cast<char*>(fBlob);

		BTextWriter writer;

		if (!firstBytes)
		{
			// failed to provide a valid pointer.
			return;
		}

		if (firstBytes[0] == kMagMz0 &&
			firstBytes[1] == kMagMz1)
		{
			LDR_EXEC_HEADER_PTR		hdrPtr = ldr_find_exec_header(firstBytes);
			LDR_OPTIONAL_HEADER_PTR optHdr = ldr_find_opt_exec_header(firstBytes);

			if (hdrPtr->mMachine != kPeMachineAMD64 ||
				hdrPtr->mSignature != kPeMagic)
			{
				writer.Write("NEWOSLDR: Not a PE32+ executable.\r");
				return;
			}

			if (optHdr->mSubsystem != kZKASubsystem)
			{
				writer.Write("NEWOSLDR: Not a ZKA Subsystem executable.\r");
				return;
			}

			writer.Write("NEWOSLDR: PE32+ executable detected (ZKA Subsystem).\r");

			auto numSecs = hdrPtr->mNumberOfSections;

			writer.Write("NEWOSLDR: Major Linker Ver: ").Write(optHdr->mMajorLinkerVersion).Write("\r");
			writer.Write("NEWOSLDR: Minor Linker Ver: ").Write(optHdr->mMinorLinkerVersion).Write("\r");
			writer.Write("NEWOSLDR: Major Subsystem Ver: ").Write(optHdr->mMajorSubsystemVersion).Write("\r");
			writer.Write("NEWOSLDR: Minor Subsystem Ver: ").Write(optHdr->mMinorSubsystemVersion).Write("\r");
			writer.Write("NEWOSLDR: Magic: ").Write(hdrPtr->mSignature).Write("\r");

			constexpr auto cPageSize = 512;

			EfiPhysicalAddress loadStartAddress = optHdr->mImageBase;
			loadStartAddress += optHdr->mBaseOfData;

			writer.Write("NEWOSLDR: ImageBase: ").Write(loadStartAddress).Write("\r");

			auto numPages = optHdr->mSizeOfImage / cPageSize;
			BS->AllocatePages(AllocateAddress, EfiLoaderData, numPages, &loadStartAddress);

			LDR_SECTION_HEADER_PTR sectPtr = (LDR_SECTION_HEADER_PTR)(((Char*)optHdr) + hdrPtr->mSizeOfOptionalHeader);

			constexpr auto sectionForCode	= ".text";
			constexpr auto sectionForNewLdr = ".ldr";
			constexpr auto sectionForBSS	= ".bss";

			for (SizeT sectIndex = 0; sectIndex < numSecs; ++sectIndex)
			{
				LDR_SECTION_HEADER_PTR sect = &sectPtr[sectIndex];

				SetMem((VoidPtr)(loadStartAddress + sect->mVirtualAddress), 0, sect->mSizeOfRawData);

				if (StrCmp(sectionForCode, sect->mName) == 0)
				{
					fStartAddress = (VoidPtr)((UIntPtr)loadStartAddress + optHdr->mAddressOfEntryPoint);
					writer.Write("NEWOSLDR: Entrypoint of DLL: ").Write((UIntPtr)fStartAddress).Write("\r");
				}
				else if (StrCmp(sectionForNewLdr, sect->mName) == 0)
				{
					struct HANDOVER_INFORMATION_STUB
					{
						UInt64 HandoverMagic;
						UInt32 HandoverType;
					}* structHandover = (struct HANDOVER_INFORMATION_STUB*)((UIntPtr)fBlob + sect->mPointerToRawData);

					if (structHandover->HandoverMagic != kHandoverMagic &&
						structHandover->HandoverType != HEL::kTypeKernel)
					{
						writer.Write("NEWOSLDR: Entrypoint of EXE: ").Write((UIntPtr)fStartAddress).Write("\r");
						CGDrawString("NEWOSLDR: NOT AN HANDOVER IMAGE...", 40, 10, RGB(0xFF, 0xFF, 0xFF));
					}
				}

				writer.Write("NEWOSLDR: offset ").Write(sect->mPointerToRawData).Write(" of ").Write(sect->mName).Write("\r");

				CopyMem((VoidPtr)(loadStartAddress + sect->mVirtualAddress), (VoidPtr)((UIntPtr)fBlob + sect->mPointerToRawData), sect->mSizeOfRawData);
			}
		}
		else if (firstBytes[0] == kPefMagic[0] &&
				 firstBytes[1] == kPefMagic[1] &&
				 firstBytes[2] == kPefMagic[2] &&
				 firstBytes[3] == kPefMagic[3])
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
		BTextWriter writer;

		if (!handover)
		{
			writer.Write("NEWOSLDR: Exec format error.\r");
			return;
		}

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
