/* -------------------------------------------

  Copyright (C) 2024-2025, Amlal El Mahrouss, all rights reserved.

------------------------------------------- */

/**
 * @file BootAHCI.cc
 * @author Amlal El Mahrouss (amlal@nekernel.org)
 * @brief SATA support for BootZ.
 * @version 0.1
 * @date 2024-02-02
 *
 * @copyright Copyright (c) Amlal El Mahrouss
 *
 */

#include <BootKit/HW/SATA.h>
#include <BootKit/Platform.h>
#include <BootKit/Protocol.h>

#include <BootKit/BootKit.h>
#include <FirmwareKit/EFI.h>

#if defined(__AHCI__) && defined(__SYSCHK__)

using namespace Boot;

/***
 *
 *
 * @brief SATA Device class.
 *
 *
 */

/**
 * @brief ATA Device constructor.
 * @param void none.
 */
BootDeviceSATA::BootDeviceSATA() noexcept {
  UInt16 pi = 0u;
  drv_std_init(pi);
}

/**
  @brief Read Buf from disk
  @param Sz Sector size
  @param Buf buffer
*/
BootDeviceSATA& BootDeviceSATA::Read(CharacterTypeASCII* Buf, SizeT SectorSz) {
  NE_UNUSED(Buf);
  NE_UNUSED(SectorSz);

  drv_std_read(mTrait.mBase / SectorSz, Buf, SectorSz, mTrait.mSize);

  return *this;
}

/**
  @brief Write Buf into disk
  @param Sz Sector size
  @param Buf buffer
*/
BootDeviceSATA& BootDeviceSATA::Write(CharacterTypeASCII* Buf, SizeT SectorSz) {
  NE_UNUSED(Buf);
  NE_UNUSED(SectorSz);

  drv_std_write(mTrait.mBase / SectorSz, Buf, SectorSz, mTrait.mSize);

  return *this;
}

/**
 * @brief ATA trait getter.
 * @return BootDeviceSATA::ATATrait& the drive config.
 */
BootDeviceSATA::SATATrait& BootDeviceSATA::Leak() {
  return mTrait;
}

#endif