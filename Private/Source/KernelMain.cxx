/*
 *	========================================================
 *
 *	HCore
 * 	Copyright Mahrouss Logic, all rights reserved.
 *
 * 	========================================================
 */

#include <ArchKit/ArchKit.hpp>
#include <FirmwareKit/Handover.hxx>
#include <KernelKit/FileManager.hpp>
#include <KernelKit/Framebuffer.hpp>
#include <KernelKit/PEFCodeManager.hxx>
#include <KernelKit/Rsrc/HCore.hxx>
#include <NewKit/Json.hpp>
#include <NewKit/KernelHeap.hpp>
#include <NewKit/UserHeap.hpp>

EXTERN_C void RuntimeMain(
    HCore::HEL::HandoverInformationHeader* HandoverHeader) {
  HCore::kcout << "HCoreKrnl: (R) Version 1.00, (C) MahroussLogic all rights "
                  "reserved.\n";

  /// Setup kernel globals.
  kKernelVirtualSize = HandoverHeader->f_VirtualSize;
  kKernelVirtualStart = HandoverHeader->f_VirtualStart;

  kKernelPhysicalSize = HandoverHeader->f_VirtualSize;
  kKernelPhysicalStart = HandoverHeader->f_VirtualStart;

  /// Init the HAL.
  MUST_PASS(HCore::ke_init_hal());

  if (HandoverHeader->f_Bootloader == 0xDD) {
    /// Mount a New partition.
    HCore::IFilesystemManager::Mount(new HCore::NewFilesystemManager());

    // Open file from first hard-drive.
    HCore::PEFLoader img("A:/System/HCoreServer.exe");

    /// Run the shell.
    if (!HCore::Utils::execute_from_image(img)) {
      HCore::ke_stop(RUNTIME_CHECK_BOOTSTRAP);
    }
  } else {
    /**
    ** This draws the background.
    */

    for (HCore::SizeT i = 0ul; i < HandoverHeader->f_GOP.f_Width; ++i) {
      for (HCore::SizeT u = 0ul; u < HandoverHeader->f_GOP.f_Height; ++u) {
        *(((volatile HCore::UInt32*)(HandoverHeader->f_GOP.f_The +
                                     4 * HandoverHeader->f_GOP.f_PixelPerLine *
                                         i +
                                     4 * u))) = RGB(20, 20, 20);
      }
    }

    /**
     ** This draws the HCore resource icon..
     */

    HCore::SizeT uA = 0;

    for (HCore::SizeT i = 10ul; i < HCORELOGO_WIDTH + 10; ++i) {
      for (HCore::SizeT u = 10ul; u < HCORELOGO_HEIGHT + 10; ++u) {
        if (HCoreLogo[uA] == 0) {
          *(((volatile HCore::UInt32*)(HandoverHeader->f_GOP.f_The +
                                       4 *
                                           HandoverHeader->f_GOP
                                               .f_PixelPerLine *
                                           i +
                                       4 * u))) |= HCoreLogo[uA];
        } else {
          *(((volatile HCore::UInt32*)(HandoverHeader->f_GOP.f_The +
                                       4 *
                                           HandoverHeader->f_GOP
                                               .f_PixelPerLine *
                                           i +
                                       4 * u))) = HCoreLogo[uA];
        }

        ++uA;
      }
    }

    /**
    This mounts the NewFS drive.
    */

    HCore::kcout << "HCoreKrnl: Setup is starting...\n";
    HCore::kcout << "HCoreKrnl: Mounting drive A: ...\n";
  }
}
