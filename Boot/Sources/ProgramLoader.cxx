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

			ExecHeaderPtr		  hdrPtr = (ldr_find_exec_header(firstBytes));
			ExecOptionalHeaderPtr optHdr = (ldr_find_opt_exec_header(firstBytes));

			// Parse PE32+
			fStartAddress = (VoidPtr)(optHdr->mAddressOfEntryPoint);
			fStackPtr	  = new Char[optHdr->mSizeOfStackReserve];

			writer.Write("newosldr: Major Linker: ").Write(optHdr->mMajorLinkerVersion).Write("\r");
			writer.Write("newosldr: Minor Linker: ").Write(optHdr->mMinorLinkerVersion).Write("\r");
			writer.Write("newosldr: Major Subsystem: ").Write(optHdr->mMajorSubsystemVersion).Write("\r");
			writer.Write("newosldr: Minor Subsystem: ").Write(optHdr->mMinorSubsystemVersion).Write("\r");
			writer.Write("newosldr: Magic: ").Write(optHdr->mMagic).Write("\r");
			writer.Write("newosldr: StartAddress: ").Write((UIntPtr)optHdr->mImageBase + optHdr->mBaseOfCode + optHdr->mAddressOfEntryPoint).Write("\r");
		}
		else if (firstBytes[0] == kPefMagic[0] &&
				 firstBytes[1] == kPefMagic[1] &&
				 firstBytes[2] == kPefMagic[2] &&
				 firstBytes[3] == kPefMagic[3])
		{
			// Parse Non FAT PEF.
			fStartAddress = nullptr;
			writer.Write("newosldr: PEF executable detected.\r");
		}
		else
		{
			writer.Write("newosldr: Exec format error.\r");
		}
	}

	/// @note handover header has to be valid!
	Void ProgramLoader::Start(HEL::HandoverInformationHeader* handover)
	{
		BTextWriter writer;

		if (!handover ||
			((Char*)fStartAddress)[0] == 0x0)
		{
			writer.Write("newosldr: Exec format error.\r");
			return;
		}

		writer.Write("newosldr: Trying to run: ").Write(fBlobName).Write("\r");

		if (!fStartAddress)
		{
			HEL::HandoverProc fn = [](HEL::HandoverInformationHeader* rcx) -> void {
				BTextWriter writer;
				writer.Write("newosldr: Exec format error, Thread has been aborted.\r");

				EFI::ThrowError(L"Exec-Format-Error", L"Format doesn't match (Thread aborted.)");
			};

			rt_jump_to_address(fn, handover, fStackPtr);

			return;
		}

		HEL::HandoverProc start = reinterpret_cast<HEL::HandoverProc>((UIntPtr)fStartAddress);

		rt_jump_to_address(start, handover, fStackPtr);
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
