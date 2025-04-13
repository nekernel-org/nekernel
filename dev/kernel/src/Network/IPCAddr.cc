/* -------------------------------------------

   Copyright (C) 2024-2025, Amlal El Mahrouss, all rights reserved.

   ------------------------------------------- */

#include <NetworkKit/IPC.h>
#include <KernelKit/KPC.h>
#include <KernelKit/ProcessScheduler.h>

namespace Kernel
{
	bool IPC_ADDR::operator==(const IPC_ADDR& addr) noexcept
	{
		return addr.UserProcessID == this->UserProcessID && addr.ProcessTeam == this->ProcessTeam;
	}

	bool IPC_ADDR::operator==(IPC_ADDR& addr) noexcept
	{
		return addr.UserProcessID == this->UserProcessID && addr.ProcessTeam == this->ProcessTeam;
	}

	bool IPC_ADDR::operator!=(const IPC_ADDR& addr) noexcept
	{
		return addr.UserProcessID != this->UserProcessID || addr.ProcessTeam != this->ProcessTeam;
	}

	bool IPC_ADDR::operator!=(IPC_ADDR& addr) noexcept
	{
		return addr.UserProcessID != this->UserProcessID || addr.ProcessTeam != this->ProcessTeam;
	}
} // namespace Kernel
