/* -------------------------------------------

	Copyright ZKA Web Services Co.

------------------------------------------- */

/**
 * @file ATA-DMA.cc
 * @author Amlal El Mahrouss (amlalelmahrouss@icloud.com)
 * @brief ATA driver (DMA mode).
 * @version 0.1
 * @date 2024-02-02
 *
 * @copyright Copyright (c) ZKA Web Services Co
 *
 */

#include <StorageKit/PRDT.h>

#include <Modules/ATA/ATA.h>
#include <ArchKit/ArchKit.h>

using namespace Kernel;

EXTERN_C Int32 kPRDTTransferStatus;
STATIC PRDT	   kPRDT;

#ifdef __ATA_DMA__

#ifdef __ATA_PIO__
#error !!! You cant have both PIO and DMA enabled! !!!
#endif /* ifdef __ATA_PIO__ */

#ifdef __AHCI__
#error !!! You cant have both ATA and AHCI enabled! !!!
#endif /* ifdef __AHCI__ */

#endif /* ifdef __ATA_DMA__ */
