/* -------------------------------------------

	Copyright ZKA Technologies.

------------------------------------------- */

#pragma once

#include <KernelKit/PE.hxx>
#include <KernelKit/MSDOS.hxx>
#include <FirmwareKit/Handover.hxx>

namespace Boot
{
	using namespace Kernel;

	class Thread;

	/// @brief Program loader class
	/// @package nl.zeta.boot.api
	class Thread final
	{
	public:
		explicit Thread() = delete;
		~Thread()		 = default;

		explicit Thread(Kernel::VoidPtr blob);

		Thread& operator=(const Thread&) = default;
		Thread(const Thread&)			   = default;

		void		Start(HEL::HandoverInformationHeader* handover);
		const char* GetName();
		void		SetName(const char* name);
		bool 		IsValid();

	private:
		Char	fBlobName[255] = { "Boot Thread" };
		VoidPtr fStartAddress{nullptr};
		VoidPtr fBlob{nullptr};
	};
} // namespace Boot
