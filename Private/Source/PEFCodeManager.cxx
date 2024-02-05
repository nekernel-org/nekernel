/*
 *	========================================================
 *
 *	HCore
 * 	Copyright Mahrouss Logic, all rights reserved.
 *
 * 	========================================================
 */

#include <KernelKit/DebugOutput.hpp>
#include <KernelKit/FileManager.hpp>
#include <KernelKit/PEFCodeManager.hxx>
#include <KernelKit/ProcessManager.hpp>
#include <NewKit/Defines.hpp>
#include <NewKit/ErrorID.hpp>
#include <NewKit/KernelHeap.hpp>
#include <NewKit/OwnPtr.hpp>
#include <NewKit/RuntimeCheck.hpp>
#include <NewKit/String.hpp>

namespace HCore {
namespace Detail {
UInt32 rt_get_pef_platform(void) noexcept {
#ifdef __32x0__
  return kPefArch32x0;
#elif defined(__64x0__)
  return kPefArch64x0;
#elif defined(__x86_64__)
  return kPefArchAMD64;
#else
  return kPefArchInvalid;
#endif  // __POWER || __x86_64__
}
}  // namespace Detail

PEFLoader::PEFLoader(const VoidPtr blob) : fCachedBlob(nullptr) {
  fCachedBlob = blob;
  fBad = false;

  MUST_PASS(fCachedBlob);
}

PEFLoader::PEFLoader(const char *path) : fCachedBlob(nullptr), fBad(false) {
  OwnPtr<FileStream<char>> file;

  file.New(const_cast<Char *>(path), kRestrictRB);

  if (StringBuilder::Equals(file->MIME(), this->MIME())) {
    fPath = StringBuilder::Construct(path).Leak();

    fCachedBlob = file->ReadAll();

    PEFContainer *container = reinterpret_cast<PEFContainer *>(fCachedBlob);

    auto fFree = [&]() -> void {
      kcout << "CodeManager: Warning: Executable format error!\n";
      fBad = true;

      ke_delete_ke_heap(fCachedBlob);

      fCachedBlob = nullptr;
    };

    if (container->Cpu == Detail::rt_get_pef_platform() &&
        container->Magic[0] == kPefMagic[0] &&
        container->Magic[1] == kPefMagic[1] &&
        container->Magic[2] == kPefMagic[2] && container->Abi == kPefAbi) {
      if (container->Kind != kPefKindObject &&
          container->Kind != kPefKindDebug) {
        kcout << "CodeManager: Info: Good executable. can proceed.\n";
        return;
      }
    }

    fFree();
  }
}

PEFLoader::~PEFLoader() {
  if (fCachedBlob) ke_delete_ke_heap(fCachedBlob);
}

VoidPtr PEFLoader::FindSymbol(const char *name, Int32 kind) {
  if (!fCachedBlob || fBad) return nullptr;

  PEFContainer *container = reinterpret_cast<PEFContainer *>(fCachedBlob);

  PEFCommandHeader *container_header = reinterpret_cast<PEFCommandHeader *>(
      (UIntPtr)fCachedBlob + sizeof(PEFContainer));

  for (SizeT index = 0; index < container->Count; ++index) {
    kcout << "Iterating over container at index: "
          << StringBuilder::FromInt("%", index)
          << ", name: " << container_header->Name << "\n";

    if (StringBuilder::Equals(container_header->Name, name)) {
      kcout << "Found potential container, checking for validity.\n";

      if (container_header->Kind == kind)
        return static_cast<UIntPtr *>(fCachedBlob) + container_header->Offset;

      continue;
    }
  }

  return nullptr;
}

ErrorOr<VoidPtr> PEFLoader::LoadStart() {
  if (auto sym = this->FindSymbol("__start", kPefCode); sym)
    return ErrorOr<VoidPtr>(sym);

  return ErrorOr<VoidPtr>(H_EXEC_ERROR);
}

bool PEFLoader::IsLoaded() noexcept { return !fBad && fCachedBlob; }

namespace Utils {
bool execute_from_image(PEFLoader &exec) noexcept {
  auto errOrStart = exec.LoadStart();

  if (errOrStart.Error() != 0) return false;

  Process proc(errOrStart.Leak().Leak());
  Ref<Process> refProc = proc;

  return ProcessManager::Shared().Leak().Add(refProc);
}
}  // namespace Utils

const char *PEFLoader::Path() { return fPath.Leak().CData(); }

const char *PEFLoader::Format() { return "PEF"; }

const char *PEFLoader::MIME() { return "application/x-exec"; }
}  // namespace HCore
