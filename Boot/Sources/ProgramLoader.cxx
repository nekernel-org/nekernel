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
			writer.Write("newosldr: Windows executable detected.\r");

			ExecHeaderPtr		  hdrPtr = ldr_find_exec_header(firstBytes);
			ExecOptionalHeaderPtr optHdr = ldr_find_opt_exec_header(firstBytes);

			// ================================ //
			// Allocate stack.
			// ================================ //
			fStackPtr = new Char[optHdr->mSizeOfStackReserve];

			writer.Write("newosldr: Major Linker Ver: ").Write(optHdr->mMajorLinkerVersion).Write("\r");
			writer.Write("newosldr: Minor Linker Ver: ").Write(optHdr->mMinorLinkerVersion).Write("\r");
			writer.Write("newosldr: Major Subsystem Ver: ").Write(optHdr->mMajorSubsystemVersion).Write("\r");
			writer.Write("newosldr: Minor Subsystem Ver: ").Write(optHdr->mMinorSubsystemVersion).Write("\r");
			writer.Write("newosldr: Magic: ").Write(optHdr->mMagic).Write("\r");

			ExecSectionHeaderPtr sectPtr = (ExecSectionHeaderPtr)((UIntPtr)firstBytes + ((DosHeaderPtr)firstBytes)->eLfanew + hdrPtr->mSizeOfOptionalHeader + sizeof(ExecHeader) + sizeof(UInt32));

			constexpr auto cMaxSectionsOfKernel = 10;

			for (SizeT sectIndex = 0; sectIndex < cMaxSectionsOfKernel; ++sectIndex)
			{
				ExecSectionHeaderPtr sect = &sectPtr[sectIndex];

				EfiPhysicalAddress address_to_alloc = sect->mVirtualAddress;

				// if this is a code header, then we can look for the entrypoint.
				if (sect->mCharacteristics & eUserSection)
				{
					if (!fStartAddress)
					{
						fStartAddress = (VoidPtr)((VoidPtr)((UIntPtr)fBlob + 184 + (sect->mVirtualAddress - optHdr->mAddressOfEntryPoint)));

						writer.Write("newosldr: Start Address set: ").Write((UIntPtr)fStartAddress).Write("\r");
					}
				}
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

			EFI::ThrowError(L"Exec-Format-Error", L"Format doesn't match (Thread aborted.)");
		};

		if (!fStartAddress)
		{
			err_fn(handover);
		}

		rt_jump_to_address(reinterpret_cast<HEL::HandoverProc>(fStartAddress), handover, fStackPtr);
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
