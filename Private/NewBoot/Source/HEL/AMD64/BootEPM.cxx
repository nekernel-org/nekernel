/* -------------------------------------------

    Copyright Mahrouss Logic

------------------------------------------- */

#include <BootKit/BootKit.hxx>

// {310E1FC7-2060-425D-BE7B-75A37CC679BC}
STATIC const BlockGUID kEPMGuid = {
    0x310e1fc7,
    0x2060,
    0x425d,
    {0xbe, 0x7b, 0x75, 0xa3, 0x7c, 0xc6, 0x79, 0xbc}};

Void boot_try_write_partition_map(const Char* namePart, SizeT namePartLength,
                              BDeviceATA* ataInterface) {
  if (namePartLength > kEPMNameLength || !namePart) return;
  if (!ataInterface) return;

  ataInterface->Leak().mBase = kEPMStartPartition;
  ataInterface->Leak().mSize = 512;

  Char buf[512] = {0};

  ataInterface->Read(buf, 1);

  BTextWriter writer;

  writer.Write(L"NewBoot: Reading EPM boot block..\r\n");

  for (SizeT i = 0; i < kEPMMagicLength; i++) {
    if (buf[i] != kEPMMagic[i]) {
      writer.Write(L"NewBoot: Writing EPM boot block..\r\n");

      BootBlockType* bootBlock = (BootBlockType*)buf;

      bootBlock->Version = kEPMHCore;
      bootBlock->NumBlocks = kEPMMaxBlks;

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

      ataInterface->Write(buf, 1);

      return;
    }
  }
}