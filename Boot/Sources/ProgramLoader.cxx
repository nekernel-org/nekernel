/* -------------------------------------------

	Copyright Zeta Electronics Corporation

------------------------------------------- */

#include <BootKit/ProgramLoader.hxx>
#include <BootKit/Vendor/Support.hxx>
#include <BootKit/BootKit.hxx>

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

		if (firstBytes[0] == 'M' &&
			firstBytes[1] == 'Z')
		{
			// Parse PE32+
			fStartAddress = nullptr;
			writer.Write("newosldr: MZ executable detected.\r");
		}
		else if (firstBytes[0] == 'J' &&
				 firstBytes[1] == 'o' &&
				 firstBytes[2] == 'y' &&
				 firstBytes[3] == '!')
		{
			// Parse Non FAT PEF.
			fStartAddress = nullptr;
			writer.Write("newosldr: PEF executable detected.\r");
		}
		else
		{
			// probably a binary blob.
			fStartAddress = fBlob;
		}
	}

	Void ProgramLoader::Start(HEL::HandoverInformationHeader* handover)
	{
		BTextWriter writer;
		writer.Write("newosldr: running: ").Write(fBlobName).Write("\r");

		if (!fStartAddress)
		{
			writer.Write("newosldr: exec error.\r");
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
