/* -------------------------------------------

    Copyright Mahrouss Logic

------------------------------------------- */

#include <KernelKit/DebugOutput.hpp>
#include <KernelKit/DriveManager.hxx>
#include <Builtins/ATA/Defines.hxx>

/// @file DriveManager.cxx
/// @brief Kernel drive manager.

namespace NewOS {
static UInt16 kATAIO = 0U;
static UInt8 kATAMaster = 0U;

/// @brief reads from an ATA drive.
/// @param pckt 
/// @return 
Void ke_drv_input(DriveTrait::DrivePacket* pckt) {
    if (!pckt) {
        return;
    }
    
    pckt->fPacketGood = false;

    drv_ata_read(pckt->fLba, kATAIO, kATAMaster, (Char*)pckt->fPacketContent, 1, pckt->fPacketSize);

    pckt->fPacketGood = true;
}

/// @brief Writes to an ATA drive.
/// @param pckt 
/// @return 
Void ke_drv_output(DriveTrait::DrivePacket* pckt) {
    if (!pckt) {
        return;
    }
    
    pckt->fPacketGood = false;

    drv_ata_write(pckt->fLba, kATAIO, kATAMaster, (Char*)pckt->fPacketContent, 1, pckt->fPacketSize);

    pckt->fPacketGood = true;
}

/// @brief Executes a disk check on the ATA drive.
/// @param pckt 
/// @return 
Void ke_drv_check_disk(DriveTrait::DrivePacket* pckt) {
    if (!pckt) {
        return;
    }
    
    pckt->fPacketGood = false;
}

/// @brief Gets the drive kind (ATA, SCSI, AHCI...)
/// @param  
/// @return 
#ifdef __ATA_PIO__ 
const Char* ke_drive_kind(Void) { return "ATA-PIO"; }
#endif

#ifdef __ATA_DMA__ 
const Char* ke_drive_kind(Void) { return "ATA-DMA"; }
#endif

#ifdef __AHCI__ 
const Char* ke_drive_kind(Void) { return "AHCO"; }
#endif

/// @brief Unimplemented drive.
/// @param pckt 
/// @return 
Void ke_drv_unimplemented(DriveTrait::DrivePacket* pckt) {}

/// @brief Makes a new drive.
/// @return the new drive.
DriveTrait construct_drive() noexcept {
    DriveTrait trait;

    trait.fInput = ke_drv_unimplemented;
    trait.fOutput = ke_drv_unimplemented;
    trait.fVerify = ke_drv_unimplemented;

    return trait;
}

/// @brief Fetches the main drive.
/// @return the new drive.
DriveTrait main_drive() noexcept {
    DriveTrait trait;

    trait.fInput = ke_drv_input;
    trait.fOutput = ke_drv_output;
    trait.fVerify = ke_drv_check_disk;

    return trait;
}
}  // namespace NewOS
