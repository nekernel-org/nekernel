/* -------------------------------------------

	Copyright ZKA Technologies

------------------------------------------- */

#pragma once

#include <KernelKit/PE.hxx>
#include <KernelKit/MSDOS.hxx>
#include <FirmwareKit/Handover.hxx>

namespace Boot
{
	using namespace Kernel;

	class KernelLoader;

	/// @brief Program loader class
	/// @package nl.zeta.boot.api
	class KernelLoader final
	{
	public:
		explicit KernelLoader() = delete;
		~KernelLoader()		 = default;

		explicit KernelLoader(Kernel::VoidPtr blob);

		KernelLoader& operator=(const KernelLoader&) = default;
		KernelLoader(const KernelLoader&)			   = default;

		void		Start(HEL::HandoverInformationHeader* handover);
		const char* GetName();
		void		SetName(const char* name);
		bool 		IsValid();

	private:
		Char	fBlobName[255];
		Char*   fHeapForProgram{nullptr};
		VoidPtr fStartAddress{nullptr};
		VoidPtr fBlob{nullptr};
	};
} // namespace Boot
