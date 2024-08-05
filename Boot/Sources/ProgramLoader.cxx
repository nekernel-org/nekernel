/* -------------------------------------------

	Copyright ZKA Technologies

------------------------------------------- */

#include <BootKit/ProgramLoader.hxx>
#include <BootKit/Support.hxx>
#include <BootKit/BootKit.hxx>

#include <KernelKit/PEF.hxx>
#include <KernelKit/PE.hxx>
#include <KernelKit/MSDOS.hxx>
#include <CFKit/LoaderUtils.hxx>

EXTERN_C
{
#include <string.h>
}

extern EfiBootServices* BS;

namespace Boot
{
	EXTERN_C Int32 rt_jump_to_address(HEL::HandoverProc baseCode, HEL::HandoverInformationHeader* handover, Char* stackPointer);

	ProgramLoader::ProgramLoader(VoidPtr blob)
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
			writer.Write("newosldr: PE32+ executable detected.\r");

			ExecHeaderPtr		  hdrPtr = ldr_find_exec_header(firstBytes);
			ExecOptionalHeaderPtr optHdr = ldr_find_opt_exec_header(firstBytes);

			auto numSecs = hdrPtr->mNumberOfSections;

			writer.Write("newosldr: Major Linker Ver: ").Write(optHdr->mMajorLinkerVersion).Write("\r");
			writer.Write("newosldr: Minor Linker Ver: ").Write(optHdr->mMinorLinkerVersion).Write("\r");
			writer.Write("newosldr: Major Subsystem Ver: ").Write(optHdr->mMajorSubsystemVersion).Write("\r");
			writer.Write("newosldr: Minor Subsystem Ver: ").Write(optHdr->mMinorSubsystemVersion).Write("\r");
			writer.Write("newosldr: Magic: ").Write(hdrPtr->mSignature).Write("\r");

			constexpr auto cPageSize = 512;

			EfiPhysicalAddress loadStartAddress = optHdr->mImageBase;
			loadStartAddress += optHdr->mBaseOfData;

			writer.Write("newosldr: ImageBase: ").Write(loadStartAddress).Write("\r");

			auto numPages = optHdr->mSizeOfImage / cPageSize;
			BS->AllocatePages(AllocateAddress, EfiLoaderData, numPages, &loadStartAddress);

			ExecSectionHeaderPtr sectPtr = (ExecSectionHeaderPtr)(((Char*)optHdr) + hdrPtr->mSizeOfOptionalHeader);

			constexpr auto sectionForCode = ".start";

			for (SizeT sectIndex = 0; sectIndex < numSecs; ++sectIndex)
			{
				ExecSectionHeaderPtr sect = &sectPtr[sectIndex];

				if (StrCmp(sectionForCode, sect->mName) == 0)
				{
					fStartAddress = (VoidPtr)((UIntPtr)loadStartAddress + sect->mVirtualAddress);
					writer.Write("newosldr: Start Address: ").Write((UIntPtr)fStartAddress).Write("\r");
				}

				writer.Write("newosldr: offset ").Write(sect->mPointerToRawData).Write(" of ").Write(sect->mName).Write(".\r");

				SetMem((VoidPtr)(loadStartAddress + sect->mVirtualAddress), 0, sect->mSizeOfRawData);
				CopyMem((VoidPtr)(loadStartAddress + sect->mVirtualAddress), (VoidPtr)((UIntPtr)fBlob + sect->mPointerToRawData), sect->mSizeOfRawData);
			}

			// ================================ //
			// Allocate stack.
			// ================================ //
			fStackPtr = new Char[optHdr->mSizeOfStackReserve];
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
			writer.Write("newosldr: PEF executable detected.\r");
		}
		else
		{
			writer.Write("newosldr: Invalid executable.\r");
		}
	}

	/// @note handover header has to be valid!
	Void ProgramLoader::Start(HEL::HandoverInformationHeader* handover)
	{
		BTextWriter writer;

		if (!handover)
		{
			writer.Write("newosldr: Exec format error.\r");
			return;
		}

		HEL::HandoverProc err_fn = [](HEL::HandoverInformationHeader* rcx) -> void {
			BTextWriter writer;
			writer.Write("newosldr: Exec format error, Thread has been aborted.\r");

			EFI::ThrowError(L"Exec-Format-Error", L"Format doesn't match (Thread aborted).");
		};

		if (!fStartAddress)
		{
			err_fn(handover);
		}

		reinterpret_cast<HEL::HandoverProc>(fStartAddress)(handover);
		err_fn(handover);
	}

	const Char* ProgramLoader::GetName()
	{
		return fBlobName;
	}

	Void ProgramLoader::SetName(const Char* name)
	{
		CopyMem(fBlobName, name, StrLen(name));
	}

	bool ProgramLoader::IsValid()
	{
		return fStartAddress != nullptr;
	}
} // namespace Boot
