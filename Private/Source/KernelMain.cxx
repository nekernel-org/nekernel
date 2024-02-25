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
#include <KernelKit/Rsrc/HCore.hxx>
#include <KernelKit/Rsrc/Util.hxx>
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

    DrawResource(HCoreLogo, HandoverHeader, HCORELOGO_WIDTH, HCORELOGO_HEIGHT,
                 10, 10);

    /**
     ** This draws the HCore resource icon..
     */

    /**
    This mounts the NewFS drive.
    */
  }
}
