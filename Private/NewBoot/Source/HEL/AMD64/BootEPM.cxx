/* -------------------------------------------

    Copyright Mahrouss Logic

------------------------------------------- */

#include <BootKit/BootKit.hxx>
#include <FSKit/NewFS.hxx>

#define kSwapSize MIB(16)

// {310E1FC7-2060-425D-BE7B-75A37CC679BC}
STATIC const BlockGUID kEPMGuid = {
    0x310e1fc7,
    0x2060,
    0x425d,
    {0xbe, 0x7b, 0x75, 0xa3, 0x7c, 0xc6, 0x79, 0xbc}};

/// @brief Write epm partition to disk.
/// @param namePart partition name
/// @param namePartLength length of name
/// @param ataInterface  disk interface, here ATA.
/// @return 
EXTERN_C Boolean boot_write_epm_partition(const Char* namePart, SizeT namePartLength,
                                 BootDeviceATA* ataInterface) {
  if (namePartLength > kEPMNameLength || !namePart) return No;
  if (!ataInterface) return No;

  ataInterface->Leak().mBase = kEPMStartPartitionBlk;
  ataInterface->Leak().mSize = kATASectorSize;

  Char buf[512] = {0};

  ataInterface->Read(buf, 1);

  BTextWriter writer;

  writer.Write(L"NewBoot.exe: Checking for a NewFS partition...\r\n");

  for (SizeT i = 0; i < kEPMMagicLength; i++) {
    if (buf[i] != kEPMMagic[i]) {
      writer.Write(L"NewBoot.exe: Writing a NewFS partition...\r\n");

      BootBlockType* bootBlock = (BootBlockType*)buf;

      bootBlock->Version = kEPMNewOS;
      bootBlock->NumBlocks = 2;

      for (SizeT i = 0; i < kEPMNameLength; i++) {
        bootBlock->Magic[i] = kEPMMagic[i];
      }

      for (SizeT i = 0; i < namePartLength; i++) {
        bootBlock->Name[i] = namePart[i];
      }

      bootBlock->SectorStart =
          sizeof(BootBlockType) + (sizeof(PartitionBlockType) * kEPMMaxBlks);

      bootBlock->SectorSz = kATASectorSize;

      bootBlock->Uuid = kEPMGuid;

      PartitionBlock* partBlock = (PartitionBlock*)(buf + sizeof(BootBlock));

      char* fsName = "NewFS";
      int fsNameLength = 6;

      for (SizeT i = 0; i < fsNameLength; i++) {
        partBlock->Fs[i] = fsName[i];
      }

      partBlock->Magic = kEPMNewOS;

      char* partName = "System HD";
      int partNameLength = 10;

      for (SizeT i = 0; i < partNameLength; i++) {
        partBlock->Name[i] = partName[i];
      }

      partBlock->SectorSz = kATASectorSize;
      partBlock->SectorStart = kEPMStartPartitionBlk + kSwapSize;
      partBlock->Version = kNewFSVersionInteger;
      partBlock->Kind = kNewFSPartitionTypeStandard;
      partBlock->SectorEnd = 0; /// grows on the disk.

      PartitionBlock* swapBlock = (PartitionBlock*)(buf + sizeof(BootBlock) + sizeof(PartitionBlock));

      for (SizeT i = 0; i < fsNameLength; i++) {
        swapBlock->Fs[i] = fsName[i];
      }

      swapBlock->Magic = kEPMNewOS;

      partName = "Swap HD";
      partNameLength = 8;

      for (SizeT i = 0; i < partNameLength; i++) {
        swapBlock->Name[i] = partName[i];
      }

      swapBlock->SectorSz = kATASectorSize;
      swapBlock->SectorStart = kEPMStartPartitionBlk;
      swapBlock->Version = kNewFSVersionInteger;
      swapBlock->Kind = kNewFSPartitionTypePage;
      swapBlock->SectorEnd = kSwapSize; /// 4 MIB swap partition.

      ataInterface->Write(buf, 1);

      return No;
    }
  }

  writer.Write(L"NewBoot.exe: Partition found, everything's OK.\r\n");
  return Yes;
}