/*
 *	========================================================
 *
 *	HCore
 * 	Copyright Mahrouss Logic, all rights reserved.
 *
 * 	========================================================
 */

#include <ArchKit/ArchKit.hpp>
#include <Drivers/PS2/Mouse.hxx>
#include <FirmwareKit/Handover.hxx>
#include <KernelKit/FileManager.hpp>
#include <KernelKit/Framebuffer.hpp>
#include <KernelKit/PEFCodeManager.hxx>
#include <KernelKit/Rsrc/Award.hxx>
#include <KernelKit/Rsrc/HCore.hxx>
#include <KernelKit/Rsrc/Util.hxx>
#include <NewKit/Json.hpp>
#include <NewKit/KernelHeap.hpp>
#include <NewKit/UserHeap.hpp>

namespace Detail {
using namespace HCore;

EXTERN_C void PowerOnSelfTest(void);

/**
    @brief Global descriptor table entry, either null, code or data.
*/
struct PACKED HC_GDT_ENTRY final {
  UInt16 Limit0;
  UInt16 Base0;
  UInt8 Base1;
  UInt8 AccessByte;
  UInt8 Limit1_Flags;
  UInt8 Base2;
};

struct PACKED ALIGN(0x1000) HC_GDT final {
  HC_GDT_ENTRY Null;
  HC_GDT_ENTRY KernCode;
  HC_GDT_ENTRY KernData;
  HC_GDT_ENTRY UserNull;
  HC_GDT_ENTRY UserCode;
  HC_GDT_ENTRY UserData;
};
}  // namespace Detail

EXTERN_C void RuntimeMain(
    HCore::HEL::HandoverInformationHeader* HandoverHeader) {
  kHandoverHeader = HandoverHeader;

  /// Setup kernel globals.
  kKernelVirtualSize = HandoverHeader->f_VirtualSize;
  kKernelVirtualStart = HandoverHeader->f_VirtualStart;

  kKernelPhysicalSize = HandoverHeader->f_VirtualSize;
  kKernelPhysicalStart = HandoverHeader->f_VirtualStart;

  static Detail::HC_GDT GDT = {
      {0, 0, 0, 0x00, 0x00, 0},  // null entry
      {0, 0, 0, 0x9a, 0xa0, 0},  // kernel code
      {0, 0, 0, 0x92, 0xa0, 0},  // kernel data
      {0, 0, 0, 0x00, 0x00, 0},  // null entry
      {0, 0, 0, 0x9a, 0xa0, 0},  // user code
      {0, 0, 0, 0x92, 0xa0, 0},  // user data
  };

  HCore::HAL::Register64 gdtBase;

  gdtBase.Base = (HCore::UIntPtr)&GDT;
  gdtBase.Limit = sizeof(Detail::HC_GDT) - 1;

  HCore::HAL::GDTLoader gdt;
  gdt.Load(gdtBase);

  HCore::UIntPtr* kInterruptVectorTable = new HCore::UIntPtr[kKernelIdtSize];
  
  HCore::HAL::Register64 idtBase;
  idtBase.Base = (HCore::UIntPtr)kInterruptVectorTable;
  idtBase.Limit = kKernelIdtSize * sizeof(HCore::Detail::AMD64::InterruptDescriptorAMD64) - 1;

  HCore::HAL::IDTLoader idt;
  idt.Load(idtBase);

  if (HandoverHeader->f_Bootloader == 0xDD) {
    /// Mounts a NewFS partition.
    HCore::IFilesystemManager::Mount(new HCore::NewFilesystemManager());

    // Open file from first hard-drive.
    HCore::PEFLoader img("A:/System/HCoreServer.exe");

    /// Run the shell.
    if (!HCore::Utils::execute_from_image(img)) {
      HCore::ke_stop(RUNTIME_CHECK_BOOTSTRAP);
    }
  } else {
    /**
    ** This does the POST.
    */

    Detail::PowerOnSelfTest();

    /**
     ** This draws the HCore resource icon..
     */

    /**
    This mounts the NewFS drive.
    */

    HCore::ke_stop(RUNTIME_CHECK_BOOTSTRAP);
  }
}
