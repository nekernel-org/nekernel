/* -------------------------------------------

	Copyright (C) 2024, TQ B.V, all rights reserved.

------------------------------------------- */

#ifdef __FSKIT_INCLUDES_HPFS__

#include <Modules/AHCI/AHCI.h>
#include <Modules/ATA/ATA.h>
#include <Modules/Flash/Flash.h>
#include <FSKit/HPFS.h>
#include <KernelKit/LPC.h>
#include <NewKit/Crc32.h>
#include <NewKit/Stop.h>
#include <NewKit/KString.h>
#include <NewKit/Utils.h>
#include <FirmwareKit/EPM.h>
#include <KernelKit/UserProcessScheduler.h>
#include <KernelKit/User.h>

#endif // ifdef __FSKIT_INCLUDES_HPFS__
