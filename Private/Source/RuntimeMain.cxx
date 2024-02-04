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

extern "C" void RuntimeMain(
    HCore::HEL::HandoverInformationHeader* HandoverHeader) {
  kKernelVirtualSize = HandoverHeader->f_VirtualSize;
  kKernelVirtualStart = HandoverHeader->f_VirtualStart;

  kKernelPhysicalSize = HandoverHeader->f_VirtualSize;
  kKernelPhysicalStart = HandoverHeader->f_VirtualStart;

  HCore::HAL::hal_page_base((HCore::UIntPtr)kKernelVirtualStart);

  HCore::ke_init_ke_heap();
  HCore::ke_init_heap();

  MUST_PASS(HCore::ke_init_hal());

  HCore::IFilesystemManager::Mount(new HCore::NewFilesystemManager());
  HCore::PEFLoader img("/System/HCoreShell.exe");

  if (!HCore::Utils::execute_from_image(img)) {
    HCore::ke_stop(RUNTIME_CHECK_BOOTSTRAP);
  }
}
