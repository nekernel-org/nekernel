/*
 *	========================================================
 *
 *	HCore
 * 	Copyright 2024 Mahrouss Logic, all rights reserved.
 *
 * 	========================================================
 */

#include <KernelKit/DebugOutput.hpp>
#include <KernelKit/DriveManager.hpp>

namespace HCore {
DriveSelector::DriveSelector() : fDrive(nullptr) {}

DriveSelector::~DriveSelector() {
  if (fDrive) {
    this->Unmount();
  }
}

DriveTraits &DriveSelector::GetMounted() {
  MUST_PASS(fDrive != nullptr);
  return *fDrive;
}

bool DriveSelector::Mount(DriveTraits *drive) {
  if (drive && drive->fReady() && fDrive == nullptr) {
    fDrive = drive;
    fDrive->fMount();

    kcout << "[Mount] drive: " << fDrive->fName << "\n";

    return true;
  }

  return false;
}

DriveTraits *DriveSelector::Unmount() {
  if (!fDrive) return nullptr;

  auto drivePointer = fDrive;

  fDrive->fUnmount();
  fDrive = nullptr;

  kcout << "[Unmount] drive: " << drivePointer->fName << "\n";

  return drivePointer;
}
}  // namespace HCore
