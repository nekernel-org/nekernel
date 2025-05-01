/* -------------------------------------------

  Copyright (C) 2024-2025, Amlal El Mahrouss, all rights reserved.

------------------------------------------- */

#include <KernelKit/DebugOutput.h>
#include <KernelKit/MemoryMgr.h>
#include <KernelKit/PEFCodeMgr.h>
#include <KernelKit/ProcessScheduler.h>
#include <NewKit/Defines.h>
#include <NewKit/KString.h>
#include <NewKit/KernelPanic.h>
#include <NewKit/OwnPtr.h>
#include "KernelKit/UserProcessScheduler.h"

/// @brief PEF stack size symbol.
#define kPefStackSizeSymbol "__PEFSizeOfReserveStack"
#define kPefHeapSizeSymbol "__PEFSizeOfReserveHeap"
#define kPefNameSymbol "__PEFProgramName"

namespace Kernel {
namespace Detail {
  /***********************************************************************************/
  /// @brief Get the PEF platform signature according to the compiled architecture.
  /***********************************************************************************/
  UInt32 ldr_get_platform(void) noexcept {
#if defined(__NE_32X0__)
    return kPefArch32x0;
#elif defined(__NE_64X0__)
    return kPefArch64x0;
#elif defined(__NE_AMD64__)
    return kPefArchAMD64;
#elif defined(__NE_PPC64__)
    return kPefArchPowerPC;
#elif defined(__NE_ARM64__)
    return kPefArchARM64;
#else
    return kPefArchInvalid;
#endif  // __32x0__ || __64x0__ || __x86_64__
  }
}  // namespace Detail

/***********************************************************************************/
/// @brief PEF loader constructor w/ blob.
/// @param blob file blob.
/***********************************************************************************/
PEFLoader::PEFLoader(const VoidPtr blob) : fCachedBlob(blob) {
  MUST_PASS(fCachedBlob);
  fBad = false;
}

/***********************************************************************************/
/// @brief PEF loader constructor.
/// @param path the filesystem path.
/***********************************************************************************/
PEFLoader::PEFLoader(const Char* path) : fCachedBlob(nullptr), fFatBinary(false), fBad(false) {
  fFile.New(const_cast<Char*>(path), kRestrictRB);
  fPath = KStringBuilder::Construct(path).Leak();

  auto kPefHeader = "PEF_CONTAINER";

  fCachedBlob = fFile->Read(kPefHeader, mib_cast(16));

  PEFContainer* container = reinterpret_cast<PEFContainer*>(fCachedBlob);

  if (container->Cpu == Detail::ldr_get_platform() && container->Magic[0] == kPefMagic[0] &&
      container->Magic[1] == kPefMagic[1] && container->Magic[2] == kPefMagic[2] &&
      container->Magic[3] == kPefMagic[3] && container->Magic[4] == kPefMagic[4] &&
      container->Abi == kPefAbi) {
    return;
  } else if (container->Magic[4] == kPefMagic[0] && container->Magic[3] == kPefMagic[1] &&
             container->Magic[2] == kPefMagic[2] && container->Magic[1] == kPefMagic[3] &&
             container->Magic[0] == kPefMagic[4] && container->Abi == kPefAbi) {
    /// This is a fat binary.
    this->fFatBinary = true;
    return;
  }

  fBad = true;

  if (fCachedBlob) mm_delete_heap(fCachedBlob);

  kout << "PEFLoader: Warning: Executable format error!\r";

  fCachedBlob = nullptr;
}

/***********************************************************************************/
/// @brief PEF destructor.
/***********************************************************************************/
PEFLoader::~PEFLoader() {
  if (fCachedBlob) mm_delete_heap(fCachedBlob);

  fFile.Delete();
}

/***********************************************************************************/
/// @brief Finds the symbol according to it's name.
/// @param name name of symbol.
/// @param kind kind of symbol we want.
/***********************************************************************************/
ErrorOr<VoidPtr> PEFLoader::FindSymbol(const Char* name, Int32 kind) {
  if (!fCachedBlob || fBad || !name) return ErrorOr<VoidPtr>{kErrorInvalidData};

  PEFContainer* container = reinterpret_cast<PEFContainer*>(fCachedBlob);

  auto blob = fFile->Read(name, sizeof(PEFCommandHeader));

  PEFCommandHeader* container_header = reinterpret_cast<PEFCommandHeader*>(blob);

  constexpr auto kMangleCharacter  = '$';
  const Char*    kContainerKinds[] = {".code64", ".data64", ".zero64", nullptr};

  ErrorOr<KString> error_or_symbol;

  switch (kind) {
    case kPefCode: {
      error_or_symbol = KStringBuilder::Construct(kContainerKinds[0]);  // code symbol.
      break;
    }
    case kPefData: {
      error_or_symbol = KStringBuilder::Construct(kContainerKinds[1]);  // data symbol.
      break;
    }
    case kPefZero: {
      error_or_symbol = KStringBuilder::Construct(kContainerKinds[2]);  // block starting symbol.
      break;
    }
    default:
      return ErrorOr<VoidPtr>{kErrorInvalidData};
      ;  // prevent that from the kernel's mode perspective, let that happen if it
         // were a user process.
  }

  Char* unconst_symbol = const_cast<Char*>(name);

  for (SizeT i = 0UL; i < rt_string_len(unconst_symbol, kPefNameLen); ++i) {
    if (unconst_symbol[i] == ' ') {
      unconst_symbol[i] = kMangleCharacter;
    }
  }

  error_or_symbol.Leak().Leak() += name;

  for (SizeT index = 0; index < container->Count; ++index) {
    if (KStringBuilder::Equals(container_header->Name, error_or_symbol.Leak().Leak().CData())) {
      if (container_header->Kind == kind) {
        if (container_header->Cpu != Detail::ldr_get_platform()) {
          if (!this->fFatBinary) {
            mm_delete_heap(blob);
            return ErrorOr<VoidPtr>{kErrorInvalidData};
          }
        }

        Char* container_blob_value = new Char[container_header->Size];

        rt_copy_memory((VoidPtr) ((Char*) blob + sizeof(PEFCommandHeader)), container_blob_value,
                       container_header->Size);
        mm_delete_heap(blob);

        kout << "PEFLoader: Information: Loaded stub: " << container_header->Name << "!\r";

        auto ret = HAL::mm_map_page((VoidPtr) container_header->VMAddress,
                                    (VoidPtr) HAL::mm_get_phys_address(container_blob_value),
                                    HAL::kMMFlagsPresent | HAL::kMMFlagsUser);

        if (ret != kErrorSuccess) {
          mm_delete_heap(container_blob_value);
          return ErrorOr<VoidPtr>{kErrorInvalidData};
        }

        return ErrorOr<VoidPtr>{container_blob_value};
      }
    }
  }

  mm_delete_heap(blob);
  return ErrorOr<VoidPtr>{kErrorInvalidData};
}

/// @brief Finds the executable entrypoint.
/// @return
ErrorOr<VoidPtr> PEFLoader::FindStart() {
  if (auto sym = this->FindSymbol(kPefStart, kPefCode); sym) return sym;

  return ErrorOr<VoidPtr>(kErrorExecutable);
}

/// @brief Tells if the executable is loaded or not.
/// @return Whether it's not bad and is cached.
bool PEFLoader::IsLoaded() noexcept {
  return !fBad && fCachedBlob;
}

const Char* PEFLoader::Path() {
  return fPath.Leak().CData();
}

const Char* PEFLoader::AsString() {
#ifdef __32x0__
  return "32x0 PEF.";
#elif defined(__64x0__)
  return "64x0 PEF.";
#elif defined(__x86_64__)
  return "x86_64 PEF.";
#elif defined(__aarch64__)
  return "AARCH64 PEF.";
#elif defined(__powerpc64__)
  return "POWER64 PEF.";
#else
  return "???? PEF.";
#endif  // __32x0__ || __64x0__ || __x86_64__ || __powerpc64__
}

const Char* PEFLoader::MIME() {
  return kPefApplicationMime;
}

ErrorOr<VoidPtr> PEFLoader::GetBlob() {
  return ErrorOr<VoidPtr>{this->fCachedBlob};
}

namespace Utils {
  ProcessID rtl_create_user_process(PEFLoader& exec, const Int32& process_kind) noexcept {
    auto errOrStart = exec.FindStart();

    if (errOrStart.Error() != kErrorSuccess) return kSchedInvalidPID;

    auto symname = exec.FindSymbol(kPefNameSymbol, kPefData);

    if (!symname) {
      symname = ErrorOr<VoidPtr>{(VoidPtr) rt_alloc_string("USER_PROCESS")};
    }

    auto id =
        UserProcessScheduler::The().Spawn(reinterpret_cast<const Char*>(symname.Leak().Leak()),
                                          errOrStart.Leak().Leak(), exec.GetBlob().Leak().Leak());

    mm_delete_heap(symname.Leak().Leak());

    if (id != kSchedInvalidPID) {
      auto stacksym = exec.FindSymbol(kPefStackSizeSymbol, kPefData);

      if (!symname) {
        stacksym = ErrorOr<VoidPtr>{(VoidPtr) new UIntPtr(kSchedMaxStackSz)};
      }

      if ((*(volatile UIntPtr*) stacksym.Leak().Leak()) > kSchedMaxStackSz) {
        *(volatile UIntPtr*) stacksym.Leak().Leak() = kSchedMaxStackSz;
      }

      UserProcessScheduler::The().CurrentTeam().AsArray()[id].Kind = process_kind;
      UserProcessScheduler::The().CurrentTeam().AsArray()[id].StackSize =
          *(UIntPtr*) stacksym.Leak().Leak();

      mm_delete_heap(stacksym.Leak().Leak());
    }

    return id;
  }
}  // namespace Utils
}  // namespace Kernel
