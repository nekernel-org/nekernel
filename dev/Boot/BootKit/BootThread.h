/* -------------------------------------------

	Copyright (C) 2024-2025, Amlal EL Mahrouss, all rights reserved.

------------------------------------------- */

#pragma once

#include <KernelKit/PE.h>
#include <KernelKit/MSDOS.h>
#include <FirmwareKit/Handover.h>

namespace Boot
{
	using namespace Kernel;

	class BootThread;

	/// @brief Bootloader Thread class.
	class BootThread final
	{
	public:
		explicit BootThread() = delete;
		~BootThread()		  = default;

		explicit BootThread(Kernel::VoidPtr blob);

		BootThread& operator=(const BootThread&) = default;
		BootThread(const BootThread&)			 = default;

		Int32		Start(HEL::BootInfoHeader* handover, BOOL is_own_stack);
		void		SetName(const char* name);
		const char* GetName();
		bool		IsValid();

	private:
		Char				 fBlobName[255] = {"BootThread"};
		VoidPtr				 fStartAddress{nullptr};
		VoidPtr				 fBlob{nullptr};
		UInt8*				 fStack{nullptr};
		HEL::BootInfoHeader* fHandover{nullptr};
	};
} // namespace Boot
