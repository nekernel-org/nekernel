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

	class BThread;

	/// @brief Program loader class
	/// @package nl.zeta.boot.api
	class BThread final
	{
	public:
		explicit BThread() = delete;
		~BThread()		   = default;

		explicit BThread(Kernel::VoidPtr blob);

		BThread& operator=(const BThread&) = default;
		BThread(const BThread&)			   = default;

		void		Start(HEL::HANDOVER_INFO_HEADER* handover);
		const char* GetName();
		void		SetName(const char* name);
		bool		IsValid();

	private:
		Char	fBlobName[255] = {"BootThread"};
		VoidPtr fStartAddress{nullptr};
		VoidPtr fBlob{nullptr};
	};
} // namespace Boot
