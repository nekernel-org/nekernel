/* -------------------------------------------

	Copyright Zeta Electronics Corporation

------------------------------------------- */

#pragma once

#include <KernelKit/PE.hxx>
#include <KernelKit/MSDOS.hxx>
#include <FirmwareKit/Handover.hxx>

namespace Boot
{
    using namespace Kernel;

	class ProgramLoader;

	/// @brief Program loader class
    /// @package nl.zeta.boot.api
	class ProgramLoader final
	{
	public:
		explicit ProgramLoader() = delete;
		~ProgramLoader()		 = default;

		explicit ProgramLoader(Kernel::VoidPtr blob);

		ProgramLoader& operator=(const ProgramLoader&) = default;
		ProgramLoader(const ProgramLoader&)			   = default;

		void		Start(HEL::HandoverInformationHeader* handover);
		const char* GetName();
		void		SetName(const char* name);

	private:
        Char fBlobName[255];
		VoidPtr fStartAddress{nullptr};
		VoidPtr fBlob{nullptr};
	};
} // namespace Kernel