/*
 * Created on Thu Oct 17 08:00:42 CEST 2024
 *
 * Copyright (c) 2024-2025 Amlal El Mahrouss
 */

#include <Framework.h>
#include <Steps.h>

#include <user/ProcessCodes.h>

#include <CoreFoundation.fwrk/headers/Array.h>

/// @brief This program makes a framework/app/steps directory for NeKernel OS.

static const Char* kStepsName    = "Steps";
static const Char* kStepsAuthor  = "John Doe";
static const Char* kStepsCompany = "Company, Inc";

SInt32 _NeMain(SInt32 argc, Char* argv[]) {
  CF::CFArray<const char*, 256U> files;

  auto ext = kFKExtension;

  for (SInt32 i = 2UL; i < argc; ++i) {
    if (MmStrCmp(argv[i], "-h") == 0) {
      PrintOut(nullptr, "%s", "make_app: Framework/Application Creation Tool.\n");
      PrintOut(nullptr, "%s", "make_app: © 2024-2025 Amlal El Mahrouss, All rights reserved.\n");

      PrintOut(nullptr, "%s", "make_app: -a: Application Directory.\n");
      PrintOut(nullptr, "%s", "make_app: -s: Steps (Setup pages) Directory.\n");
      PrintOut(nullptr, "%s", "make_app: -f: Framework Directory.\n");

      return kErrorSuccess;
    }

    if (MmStrCmp(argv[i], "--author") == 0) {
      MmCopyMemory(const_cast<Char*>(kStepsAuthor), const_cast<Char*>(argv[i + 1]),
                   MmStrLen(argv[i + 1]));
      continue;
    }

    if (MmStrCmp(argv[i], "--company") == 0) {
      MmCopyMemory(const_cast<Char*>(kStepsCompany), const_cast<Char*>(argv[i + 1]),
                   MmStrLen(argv[i + 1]));
      continue;
    }

    if (MmStrCmp(argv[i], "--name") == 0) {
      MmCopyMemory(const_cast<Char*>(kStepsName), const_cast<Char*>(argv[i + 1]),
                   MmStrLen(argv[i + 1]));
      continue;
    }

    if (MmStrCmp(argv[i], "-a") == 0) {
      ext = kAppExtension;
      continue;
    } else if (MmStrCmp(argv[i], "-s") == 0) {
      ext = kStepsExtension;
      continue;
    } else if (MmStrCmp(argv[i], "-f") == 0) {
      ext = kFKExtension;
      continue;
    }

    files[i] = argv[i];
  }

  auto path = argv[1];

  if (FsCreateDir(path)) {
    FsCreateDir(StrFmt("{}{}", path, kRootDirectory));
    FsCreateDir(StrFmt("{}{}", path, kExecDirectory));

    if (MmStrCmp(ext, kStepsExtension) == 0) {
      FsCreateFile(StrFmt("{}{}{}{}", path, kRootDirectory, "_setup"));

      auto handle =
          IoOpenFile(StrFmt("{}{}{}{}", path, kRootDirectory, "_setup", kStepsExtension), nullptr);

      struct STEPS_COMMON_RECORD record;

      MmFillMemory(&record, sizeof(STEPS_COMMON_RECORD), 0);

      MmCopyMemory(record.name, const_cast<Char*>(kStepsName), kStepsStrLen);
      MmCopyMemory(record.author, const_cast<Char*>(kStepsAuthor), kStepsStrLen);
      MmCopyMemory(record.company, const_cast<Char*>(kStepsCompany), kStepsStrLen);

      MmCopyMemory(record.magic, const_cast<Char*>(kStepsMagic), kStepsMagicLen);

      record.version = kStepsVersion;

      IoWriteFile(handle, (void*) &record, sizeof(STEPS_COMMON_RECORD));
      IoCloseFile(handle);

      handle = nullptr;
    }

    for (auto i = 0UL; i < files.Count(); ++i) {
      auto& file = files[i];

      FsCopy(path, StrFmt("{}{}", path, file));
    }

    return kErrorSuccess;
  }

  return kErrorInternal;
}
