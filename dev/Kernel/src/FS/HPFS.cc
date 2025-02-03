/* -------------------------------------------

	Copyright (C) 2024-2025, Amlal EL Mahrouss, all rights reserved.

------------------------------------------- */

#ifdef __FSKIT_INCLUDES_HPFS__

#include <Mod/AHCI/AHCI.h>
#include <Mod/ATA/ATA.h>
#include <Mod/Flash/Flash.h>
#include <FSKit/HPFS.h>
#include <KernelKit/LPC.h>
#include <NewKit/Crc32.h>
#include <NewKit/KernelPanic.h>
#include <NewKit/KString.h>
#include <NewKit/Utils.h>
#include <FirmwareKit/EPM.h>
#include <KernelKit/UserProcessScheduler.h>
#include <KernelKit/User.h>

#endif // ifdef __FSKIT_INCLUDES_HPFS__
