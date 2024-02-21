/*
 *	========================================================
 *
 *	HCore
 * 	Copyright Mahrouss Logic, all rights reserved.
 *
 * 	========================================================
 */

#include <ArchKit/ArchKit.hpp>
#include <EFIKit/Handover.hxx>
#include <KernelKit/FileManager.hpp>
#include <KernelKit/PEFCodeManager.hxx>
#include <NewKit/Json.hpp>
#include <NewKit/KernelHeap.hpp>
#include <NewKit/UserHeap.hpp>

EXTERN_C void RuntimeMain(
    HCore::HEL::HandoverInformationHeader* HandoverHeader) {
  /// Setup kernel globals.
  kKernelVirtualSize = HandoverHeader->f_VirtualSize;
  kKernelVirtualStart = HandoverHeader->f_VirtualStart;

  kKernelPhysicalSize = HandoverHeader->f_VirtualSize;
  kKernelPhysicalStart = HandoverHeader->f_VirtualStart;

  /// Setup base page.
  HCore::HAL::hal_page_base((HCore::UIntPtr)kKernelVirtualStart);

  /// Init memory managers.
  HCore::ke_init_ke_heap();
  HCore::ke_init_heap();

  /// Init the HAL.
  MUST_PASS(HCore::ke_init_hal());

  /// Mount a New partition.
  HCore::IFilesystemManager::Mount(new HCore::NewFilesystemManager());
  HCore::PEFLoader img("/System/HCoreShell.exe");

  /// Run the shell.
  if (!HCore::Utils::execute_from_image(img)) {
    HCore::ke_stop(RUNTIME_CHECK_BOOTSTRAP);
  }
}
