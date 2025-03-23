/* -------------------------------------------

   Copyright (C) 2024-2025, Amlal EL Mahrouss, all rights reserved.

   ------------------------------------------- */

#include <NetworkKit/IPC.h>
#include <KernelKit/LPC.h>
#include <KernelKit/UserProcessScheduler.h>

namespace NeOS
{
	bool IPC_ADDR::operator==(const IPC_ADDR& addr) noexcept
	{
		return addr.UserProcessID == this->UserProcessID && addr.UserProcessTeam == this->UserProcessTeam;
	}

	bool IPC_ADDR::operator==(IPC_ADDR& addr) noexcept
	{
		return addr.UserProcessID == this->UserProcessID && addr.UserProcessTeam == this->UserProcessTeam;
	}

	bool IPC_ADDR::operator!=(const IPC_ADDR& addr) noexcept
	{
		return addr.UserProcessID != this->UserProcessID || addr.UserProcessTeam != this->UserProcessTeam;
	}

	bool IPC_ADDR::operator!=(IPC_ADDR& addr) noexcept
	{
		return addr.UserProcessID != this->UserProcessID || addr.UserProcessTeam != this->UserProcessTeam;
	}
} // namespace NeOS
