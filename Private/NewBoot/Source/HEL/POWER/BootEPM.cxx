/* -------------------------------------------

    Copyright Mahrouss Logic

------------------------------------------- */

#include <BootKit/BootKit.hxx>
#include <FSKit/NewFS.hxx>

#define kEPMSectorSize (1024U)
#define kEPMSwapSize MIB(16)

// {310E1FC7-2060-425D-BE7B-75A37CC679BC}
STATIC const BlockGUID kEPMGuid = {
    0x310e1fc7,
    0x2060,
    0x425d,
    {0xbe, 0x7b, 0x75, 0xa3, 0x7c, 0xc6, 0x79, 0xbc}};

/// @brief Write epm partition to disk.
/// @param namePart partition name
/// @param namePartLength length of name
/// @param bootDev disk interface.
/// @return
EXTERN_C Boolean boot_write_epm_partition(const Char* namePart, SizeT namePartLength,
                                 BootDevice* bootDev) {
  if (namePartLength > kEPMNameLength || !namePart) return No;
  if (!bootDev) return No;

  bootDev->Leak().mBase = kEPMStartPartitionBlk;
  bootDev->Leak().mSize = kEPMSectorSize;

  Char buf[kEPMSectorSize] = {0};

  bootDev->Read(buf, 1);

  BTextWriter writer;

  writer.Write(L"NewBoot: Checking for an EPM partition...\r\n");

  for (SizeT index = 0; index < kEPMMagicLength; ++index) {
    if (buf[index] != kEPMMagic[index]) {
      writer.Write(L"NewBoot: Writing an EPM partition...\r\n");

      BootBlockType* bootBlock = (BootBlockType*)buf;

      bootBlock->Version = kEPMRevision;
      bootBlock->NumBlocks = 2;

      for (SizeT i = 0; i < kEPMNameLength; ++i) {
        bootBlock->Magic[i] = kEPMMagic[i];
      }

      for (SizeT i = 0; i < namePartLength; ++i) {
        bootBlock->Name[i] = namePart[i];
      }

      bootBlock->LbaStart =
          sizeof(BootBlockType) + (sizeof(PartitionBlockType) * kEPMMaxBlks);

      bootBlock->SectorSz = kEPMSectorSize;

      bootBlock->Uuid = kEPMGuid;

      PartitionBlock* partBlock = (PartitionBlock*)(buf + sizeof(BootBlock));

      char* fsName = "NewFS";
      int fsNameLength = 6;

      for (SizeT i = 0; i < fsNameLength; ++i) {
        partBlock->Fs[i] = fsName[i];
      }

      partBlock->Version = kEPMNewOS;

      char* partName = "System HD";
      int partNameLength = 10;

      for (SizeT i = 0; i < partNameLength; ++i) {
        partBlock->Name[i] = partName[i];
      }

      partBlock->SectorSz = kEPMSectorSize;
      partBlock->LbaStart = kEPMStartPartitionBlk + kEPMSwapSize;
      partBlock->Version = kNewFSVersionInteger;
      partBlock->Kind = kNewFSPartitionTypeStandard;
      partBlock->LbaEnd = 0UL; ///! grows on the disk.

      PartitionBlock* swapBlock = (PartitionBlock*)(buf + sizeof(BootBlock) + sizeof(PartitionBlock));

      for (SizeT i = 0; i < fsNameLength; ++i) {
        swapBlock->Fs[i] = fsName[i];
      }

      swapBlock->Version = kEPMNewOS;

      partName = "Swap HD";
      partNameLength = 8;

      for (SizeT i = 0; i < partNameLength; ++i) {
        swapBlock->Name[i] = partName[i];
      }

      swapBlock->SectorSz = kEPMSectorSize;
      swapBlock->LbaStart = kEPMStartPartitionBlk;
      swapBlock->Version = kNewFSVersionInteger;
      swapBlock->Kind = kNewFSPartitionTypePage;
      swapBlock->LbaEnd = kEPMSwapSize; /// 4 MIB swap partition.

      bootDev->Write(buf, 1);

      return No;
    }
  }

  writer.Write(L"NewBoot: Partition found, everything's OK.\r\n");
  return Yes;
}
