/*
 *	========================================================
 *
 *	NewBoot
 * 	Copyright Mahrouss Logic, all rights reserved.
 *
 * 	========================================================
 */

#include <BootKit/BootKit.hxx>
#include <EFIKit/Api.hxx>
#include <EFIKit/EFI.hxx>

/////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  @brief External EFI code for completeness.
//
/////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace Detail {
const EfiDevicePathProtocol mUefiDevicePathLibEndDevicePath = {
    END_DEVICE_PATH_TYPE,
    END_ENTIRE_DEVICE_PATH_SUBTYPE,
    {sizeof(EfiDevicePathProtocol), 0}};

EfiDevicePathProtocol *UefiDevicePathLibAppendDevicePath(
    const EfiDevicePathProtocol *FirstDevicePath,
    const EfiDevicePathProtocol *SecondDevicePath) {
  SizeT Size;
  SizeT Size1;
  SizeT Size2;
  EfiDevicePathProtocol *NewDevicePath;
  EfiDevicePathProtocol *DevicePath2;

  //
  // If there's only 1 path, just duplicate it.
  //
  if (FirstDevicePath == nullptr) {
    return DuplicateDevicePath((SecondDevicePath != nullptr)
                                   ? SecondDevicePath
                                   : &mUefiDevicePathLibEndDevicePath);
  }

  if (SecondDevicePath == nullptr) {
    return DuplicateDevicePath(FirstDevicePath);
  }

  if (!IsDevicePathValid(FirstDevicePath, 0) ||
      !IsDevicePathValid(SecondDevicePath, 0)) {
    return nullptr;
  }

  //
  // Allocate space for the combined device path. It only has one end node of
  // length EFI_DEVICE_PATH_PROTOCOL.
  //
  Size1 = GetDevicePathSize(FirstDevicePath);
  Size2 = GetDevicePathSize(SecondDevicePath);
  Size = Size1 + Size2 - sizeof(EfiDevicePathProtocol);

  BS->AllocatePool(EfiLoaderData, Size, (VoidPtr *)&NewDevicePath);

  if (NewDevicePath != NULL) {
    BCopyMem((CharacterType *)NewDevicePath, (CharacterType *)FirstDevicePath,
             Size1);
    //
    // Over write FirstDevicePath EndNode and do the copy
    //
    DevicePath2 =
        (EfiDevicePathProtocol *)((Char *)NewDevicePath +
                                  (Size1 - sizeof(EfiDevicePathProtocol)));
    BCopyMem((CharacterType *)DevicePath2, (CharacterType *)SecondDevicePath,
             Size2);
  }

  return NewDevicePath;
}

EfiDevicePathProtocol *AppendDevicePath(
    EfiDevicePathProtocol *FirstDevicePath,
    EfiDevicePathProtocol *SecondDevicePath) {
  return UefiDevicePathLibAppendDevicePath(FirstDevicePath, SecondDevicePath);
}

EfiDevicePathProtocol *EFI_API DevicePathFromHandle(EfiHandlePtr Handle) {
  EfiDevicePathProtocol *DevicePath;
  UInt64 Status;

  EfiGUID guid = EfiGUID(EFI_DEVICE_PATH_PROTOCOL_GUID);

  Status = BS->HandleProtocol(Handle, &guid, (VoidPtr *)&DevicePath);
  if (Status != kEfiOk) {
    DevicePath = nullptr;
  }

  return DevicePath;
}

UInt16 ReadUnaligned16(UInt16 *Buffer) {
  // ASSERT (Buffer != NULL);
  return *Buffer;
}

UInt16 WriteUnaligned16(UInt16 *Buffer, UInt16 Value) {
  // ASSERT (Buffer != NULL);

  return *Buffer = Value;
}

UInt16 EFI_API SetDevicePathNodeLength(Void *Node, UInt32 Length) {
  // ASSERT(Node != nullptr);
  // ASSERT((Length >= sizeof(EfiDevicePathProtocol)) && (Length < KIB(64)));
  return WriteUnaligned16(
      (UInt16 *)&((EfiDevicePathProtocol *)(Node))->Length[0],
      (UInt16)(Length));
}

Void EFI_API SetDevicePathEndNode(Void *Node) {
  // ASSERT(Node != NULL);
  BCopyMem((CharacterType *)Node,
           (CharacterType *)&mUefiDevicePathLibEndDevicePath,
           sizeof(mUefiDevicePathLibEndDevicePath));
}

UInt32 EFI_API DevicePathNodeLength(const Void *Node) {
  // ASSERT(Node != NULL);
  return ReadUnaligned16(
      (UInt16 *)&((EfiDevicePathProtocol *)(Node))->Length[0]);
}

EfiDevicePathProtocol *EFI_API NextDevicePathNode(Void *Node) {
  // ASSERT (Node != NULL);
  return (EfiDevicePathProtocol *)((UInt8 *)(Node) +
                                   DevicePathNodeLength(Node));
}

EfiDevicePathProtocol *EFI_API FileDevicePath(EfiHandlePtr Device,
                                              CharacterType *FileName) {
  SizeT Size;
  EfiFileDevicePathProtocol *FilePath;
  EfiDevicePathProtocol *DevicePath;
  EfiDevicePathProtocol *FileDevicePath;

  DevicePath = nullptr;

  Size = BStrLen(FileName);
  BS->AllocatePool(
      EfiLoaderData,
      Size + sizeof(EfiFileDevicePathProtocol) + sizeof(EfiDevicePathProtocol),
      (VoidPtr *)&FileDevicePath);

  if (FileDevicePath != nullptr) {
    FilePath = (EfiFileDevicePathProtocol *)FileDevicePath;
    FilePath->Proto.Type = kEFIMediaDevicePath;
    FilePath->Proto.SubType = kEFIMediaDevicePath;

    BCopyMem(FilePath->Path, FileName, Size);

    SetDevicePathNodeLength(&FilePath->Proto,
                            Size + sizeof(EfiFileDevicePathProtocol));

    SetDevicePathEndNode(NextDevicePathNode(&FilePath->Proto));

    if (Device != nullptr) {
      DevicePath = DevicePathFromHandle(Device);
    }

    DevicePath = AppendDevicePath(DevicePath, FileDevicePath);
    BS->FreePool(FileDevicePath);
  }

  return DevicePath;
}
}  // namespace Detail
