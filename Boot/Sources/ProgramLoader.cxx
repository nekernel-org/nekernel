/* -------------------------------------------

	Copyright Zeta Electronics Corporation

------------------------------------------- */

#include <BootKit/ProgramLoader.hxx>
#include <BootKit/Vendor/Support.hxx>
#include <BootKit/BootKit.hxx>
#include <string.h>

namespace Boot
{
	ProgramLoader::ProgramLoader(VoidPtr blob)
		: fBlob(blob), fStartAddress(nullptr)
	{
        // detect the format.
        const char* firstBytes = reinterpret_cast<char*>(fBlob);

        BTextWriter writer;
        writer.WriteCharacter(firstBytes[0]).WriteCharacter(firstBytes[1]).WriteCharacter('\r').WriteCharacter('\n');

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
        }
        else if (firstBytes[0] == 'J' &&
            firstBytes[1] == 'o' &&
            firstBytes[2] == 'y' &&
            firstBytes[3] == '!')
        {
            // Parse Non FAT PEF.
            fStartAddress = nullptr;
        }
        else
        {
            // probably a binary blob.
            fStartAddress = fBlob;
        }
    }

	Void ProgramLoader::Start(HEL::HandoverInformationHeader* handover)
	{
        if (!fStartAddress) return;

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