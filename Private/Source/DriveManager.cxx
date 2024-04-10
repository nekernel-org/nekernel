/* -------------------------------------------

    Copyright Mahrouss Logic

------------------------------------------- */

#include <KernelKit/DebugOutput.hpp>
#include <KernelKit/DriveManager.hxx>
#include <Builtins/ATA/ATA.hxx>
#include <Builtins/AHCI/AHCI.hxx>
#include <NewKit/Utils.hpp>

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

#ifdef __AHCI__
    drv_std_read(pckt->fLba, (Char*)pckt->fPacketContent, 1, pckt->fPacketSize);
#elif defined(__ATA_PIO__) || defined(__ATA_DMA__)
    drv_std_read(pckt->fLba, kATAIO, kATAMaster, (Char*)pckt->fPacketContent, 1, pckt->fPacketSize);
#endif


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

#ifdef __AHCI__
    drv_std_write(pckt->fLba,(Char*)pckt->fPacketContent, 1, pckt->fPacketSize);
#elif defined(__ATA_PIO__) || defined(__ATA_DMA__)
    drv_std_write(pckt->fLba, kATAIO, kATAMaster, (Char*)pckt->fPacketContent, 1, pckt->fPacketSize);
#endif

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
const Char* ke_drive_kind(Void) { return "AHCI"; }
#endif

/// @brief Unimplemented drive.
/// @param pckt 
/// @return 
Void ke_drv_unimplemented(DriveTrait::DrivePacket* pckt) {}

/// @brief Makes a new drive.
/// @return the new drive.
DriveTrait construct_drive() noexcept {
    DriveTrait trait;

    rt_copy_memory((VoidPtr)"/Mount/Null", trait.fName, rt_string_len("/Mount/Null"));
    trait.fKind = kInvalidDrive;

    trait.fInput = ke_drv_unimplemented;
    trait.fOutput = ke_drv_unimplemented;
    trait.fVerify = ke_drv_unimplemented;

    return trait;
}

/// @brief Fetches the main drive.
/// @return the new drive.
DriveTrait construct_main_drive() noexcept {
    DriveTrait trait;

    rt_copy_memory((VoidPtr)"/Mount/Disk/0", trait.fName, rt_string_len("/Mount/Disk/0"));
    trait.fKind = kMassStorage | kEPMDrive;

    trait.fInput = ke_drv_input;
    trait.fOutput = ke_drv_output;
    trait.fVerify = ke_drv_check_disk;

    return trait;
}
}  // namespace NewOS
