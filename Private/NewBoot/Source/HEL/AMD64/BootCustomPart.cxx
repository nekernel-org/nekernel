/* -------------------------------------------

    Copyright Mahrouss Logic

------------------------------------------- */

#include <BootKit/BootKit.hxx>
#include <FSKit/NewFS.hxx>

/// @brief Writes a NewOS partition on top of EPM
/// @param ataInterface The ATA interface.
/// @return 
EXTERN_C Boolean boot_write_newos_partition(BootDeviceATA* ataInterface) {
    if (!ataInterface) return No;

    ataInterface->Leak().mBase = 0;
    ataInterface->Leak().mSize = kATASectorSize;

    Char newOSHeader[kATASectorSize] = {
        /// signature of our system partition.
        'N', 'e', 'w', '!', 
        /// version of our os partition
        (Char)0x10,
        
        /// to retrieve the header size add these two fileds. (divided into parts.)

        /// header size (pt 1) 
        (Char)0x100, 
        /// header size (pt 2)
        (Char)0x100,
        /// Reserved
        (Char)0x00,
        /// Current LBA.
        (Char)0x00,
        /// First usable LBA.
        (Char)kEPMStartPartitionBlk,
    };

    ataInterface->Write(newOSHeader, 1);

    return Yes;
}