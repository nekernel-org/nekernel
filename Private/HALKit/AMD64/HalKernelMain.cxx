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
#include <KernelKit/Rsrc/Splash.hxx>
#include <KernelKit/Rsrc/Util.hxx>
#include <NewKit/Json.hpp>
#include <NewKit/KernelHeap.hpp>
#include <NewKit/UserHeap.hpp>

///! @brief Disk contains HCore files.
#define kInstalledMedia 0xDD

EXTERN_C HCore::VoidPtr kInterruptVectorTable[];

namespace Detail {
using namespace HCore;

EXTERN_C void _ke_power_on_self_test(void);

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

  kKernelPhysicalSize = HandoverHeader->f_PhysicalSize;
  kKernelPhysicalStart = HandoverHeader->f_PhysicalStart;

  static Detail::HC_GDT GDT = {
      {0, 0, 0, 0x00, 0x00, 0},  // null entry
      {0, 0, 0, 0x9a, 0xaf, 0},  // kernel code
      {0, 0, 0, 0x92, 0xaf, 0},  // kernel data
      {0, 0, 0, 0x00, 0x00, 0},  // null entry
      {0, 0, 0, 0x9a, 0xaf, 0},  // user code
      {0, 0, 0, 0x92, 0xaf, 0},  // user data
  };

  HCore::HAL::RegisterGDT gdtBase;

  gdtBase.Base = (HCore::UIntPtr)&GDT;
  gdtBase.Limit = sizeof(Detail::HC_GDT) - 1;

  /// load gdt and far jump to rax.

  HCore::HAL::GDTLoader gdt;
  gdt.Load(gdtBase);

  /// load idt buffer

  HCore::HAL::Register64 idtBase;
  idtBase.Base = (HCore::UIntPtr)kInterruptVectorTable;
  idtBase.Limit = 0;

  HCore::HAL::IDTLoader idt;
  idt.Load(idtBase);

  KeInitRsrc();
  KeDrawRsrc(MahroussLogic, MAHROUSSLOGIC_HEIGHT, MAHROUSSLOGIC_WIDTH, ((kHandoverHeader->f_GOP.f_Width - MAHROUSSLOGIC_WIDTH) / 2), ((kHandoverHeader->f_GOP.f_Height - MAHROUSSLOGIC_HEIGHT) / 2));
  KeClearRsrc();

  Detail::_ke_power_on_self_test();

  HCore::kcout << "HCoreKrnl: POST done, everything is OK...\r\n";

  if (HandoverHeader->f_Bootloader == kInstalledMedia) {
    /// Mounts a NewFS block.
    HCore::IFilesystemManager::Mount(new HCore::NewFilesystemManager());

    // Open file from first hard-drive.
    HCore::PEFLoader img("A:/System/HCoreServer.exe");

    /// Run the server executive.
    HCore::Utils::execute_from_image(img);
  } else {
    /**
    ** This does the POST.
    */

    /**
    This mounts the NewFS drive.
    */
  }

  HCore::ke_stop(RUNTIME_CHECK_BOOTSTRAP);
}
