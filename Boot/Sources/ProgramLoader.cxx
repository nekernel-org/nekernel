/* -------------------------------------------

	Copyright ZKA Technologies

------------------------------------------- */

#include <BootKit/ProgramLoader.hxx>
#include <BootKit/Support.hxx>
#include <BootKit/BootKit.hxx>

#include <KernelKit/PEF.hpp>

EXTERN_C
{
#include <string.h>
}

namespace Boot
{
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
			// Parse PE32+
			fStartAddress = nullptr;
			writer.Write("newosldr: MZ executable detected.\r");
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
		MUST_PASS(handover);

		BTextWriter writer;
		writer.Write("newosldr: Trying to run: ").Write(fBlobName).Write("\r");

		if (!fStartAddress)
		{
			writer.Write("newosldr: Exec format error.\r");
			return;
		}

		((HEL::HandoverProc)fStartAddress)(handover);
	}

	const Char* ProgramLoader::GetName()
	{
		return fBlobName;
	}

	Void ProgramLoader::SetName(const Char* name)
	{
		CopyMem(fBlobName, name, StrLen(name));
	}
} // namespace Boot
