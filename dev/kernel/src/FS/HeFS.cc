/* -------------------------------------------

	Copyright (C) 2024-2025, Amlal EL Mahrouss, all rights reserved.

------------------------------------------- */

#ifdef __FSKIT_INCLUDES_HeFS__

#include <modules/AHCI/AHCI.h>
#include <modules/ATA/ATA.h>
#include <modules/Flash/Flash.h>
#include <FSKit/HeFS.h>
#include <KernelKit/KPC.h>
#include <NewKit/Crc32.h>
#include <NewKit/KernelPanic.h>
#include <NewKit/KString.h>
#include <NewKit/Utils.h>
#include <FirmwareKit/EPM.h>
#include <KernelKit/ProcessScheduler.h>
#include <KernelKit/User.h>

#endif // ifdef __FSKIT_INCLUDES_HeFS__
