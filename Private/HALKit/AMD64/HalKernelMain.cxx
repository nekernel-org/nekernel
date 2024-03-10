/* -------------------------------------------

    Copyright Mahrouss Logic

------------------------------------------- */

#include <ArchKit/ArchKit.hpp>
#include <Drivers/PS2/Mouse.hxx>
#include <FirmwareKit/Handover.hxx>
#include <KernelKit/FileManager.hpp>
#include <KernelKit/Framebuffer.hpp>
#include <KernelKit/PEFCodeManager.hxx>
#include <KernelKit/Rsrc/Util.hxx>
#include <NewKit/Json.hpp>
#include <NewKit/KernelHeap.hpp>
#include <NewKit/UserHeap.hpp>
#include <KernelKit/ProcessManager.hpp>
#include <KernelKit/Rsrc/Splash.rsrc>

///! @brief Disk contains HCore files.
#define kInstalledMedia 0xDD

EXTERN_C HCore::VoidPtr kInterruptVectorTable[];

EXTERN_C void RuntimeMain(
    HCore::HEL::HandoverInformationHeader* HandoverHeader) {
  kHandoverHeader = HandoverHeader;

  /// Setup kernel globals.
  kKernelVirtualSize = HandoverHeader->f_VirtualSize;
  kKernelVirtualStart = HandoverHeader->f_VirtualStart;

  kKernelPhysicalSize = HandoverHeader->f_PhysicalSize;
  kKernelPhysicalStart = HandoverHeader->f_PhysicalStart;

  STATIC HCore::HAL::Detail::HCoreGDT GDT = {
      {0, 0, 0, 0x00, 0x00, 0},  // null entry
      {0, 0, 0, 0x9a, 0xaf, 0},  // kernel code
      {0, 0, 0, 0x92, 0xaf, 0},  // kernel data
      {0, 0, 0, 0x00, 0x00, 0},  // null entry
      {0, 0, 0, 0x9a, 0xaf, 0},  // user code
      {0, 0, 0, 0x92, 0xaf, 0},  // user data
  };

  HCore::HAL::RegisterGDT gdtBase;

  gdtBase.Base = reinterpret_cast<HCore::UIntPtr>(&GDT);
  gdtBase.Limit = sizeof(HCore::HAL::Detail::HCoreGDT) - 1;

  /// Load GDT.

  HCore::HAL::GDTLoader gdt;
  gdt.Load(gdtBase);

  /// Load IDT.

  HCore::HAL::Register64 idtBase;
  idtBase.Base = (HCore::UIntPtr)kInterruptVectorTable;
  idtBase.Limit = 0;

  HCore::HAL::IDTLoader idt;
  idt.Load(idtBase);

  KeInitRsrc();

  KeDrawRsrc(MahroussLogic, MAHROUSSLOGIC_HEIGHT, MAHROUSSLOGIC_WIDTH, 
            ((kHandoverHeader->f_GOP.f_Width - MAHROUSSLOGIC_WIDTH) / 2), 
            ((kHandoverHeader->f_GOP.f_Height - MAHROUSSLOGIC_HEIGHT) / 2));

  KeClearRsrc();

  /// START POST

  HCore::HAL::Detail::_ke_power_on_self_test();
  
  /// END POST

  /// We already have an install of HCore.
  if (HandoverHeader->f_Bootloader == kInstalledMedia) {
    /// Mounts a NewFS block.
    HCore::IFilesystemManager::Mount(new HCore::NewFilesystemManager());
    
    /// TODO: Parse system configuration.
  } else {
    HCore::kcout << "HCoreKrnl: Running setup...\r\n";

    //HCore::Forms::SetupWizardControl wizard;
    //wizard.ShowDialog();
  }

  HCore::ke_stop(RUNTIME_CHECK_BOOTSTRAP);
}
