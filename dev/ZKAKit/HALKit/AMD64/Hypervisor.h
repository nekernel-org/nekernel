/* -------------------------------------------

	Copyright (C) 2024, Amlal EL Mahrouss, all rights reserved.

------------------------------------------- */

#pragma once

#include <NewKit/Defines.h>

namespace Kernel
{
	MAKE_STRING_ENUM(HYPERVISOR)
	ENUM_STRING(Qemu, "TCGTCGTCGTCG");
	ENUM_STRING(KVM, " KVMKVMKVM  ");
	ENUM_STRING(VMWare, "VMwareVMware");
	ENUM_STRING(VirtualBox, "VBoxVBoxVBox");
	ENUM_STRING(Xen, "XenVMMXenVMM");
	ENUM_STRING(Microsoft, "Microsoft Hv");
	ENUM_STRING(Parallels, " prl hyperv ");
	ENUM_STRING(ParallelsAlt, " lrpepyh vr ");
	ENUM_STRING(Bhyve, "bhyve bhyve ");
	ENUM_STRING(Qnx, " QNXQVMBSQG ");
	END_STRING_ENUM()
} // namespace Kernel
