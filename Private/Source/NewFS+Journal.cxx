/* -------------------------------------------

    Copyright Mahrouss Logic

------------------------------------------- */

#include <KernelKit/DebugOutput.hpp>
#include <KernelKit/FileManager.hpp>

#ifdef __FSKIT_NEWFS__

//! BUGS: 0
//! @brief Journaling for NewFS.

#define kNewFSOpLog (4)

namespace HCore::Detail {
typedef Boolean (*NewFSRunnerType)(VoidPtr delegate);

/// @brief Journal thread class.
class NewFSJournalRunner final {
 private:
  NewFSRunnerType fLoadRoutine{nullptr};
  NewFSRunnerType fCacheRoutine{nullptr};
  NewFSRunnerType fUnloadRoutine{nullptr};

 public:
  explicit NewFSJournalRunner(NewFSRunnerType load_runner)
      : fLoadRoutine(load_runner) {
    MUST_PASS(fLoadRoutine);

    // load runner
    if (fLoadRoutine) fLoadRoutine(this);
  }

  /// @brief Unload journal runner
  ~NewFSJournalRunner() noexcept {
    if (fUnloadRoutine) fUnloadRoutine(this);
  }

  HCORE_COPY_DEFAULT(NewFSJournalRunner);

 public:
  Boolean Run(const Int32& operation, VoidPtr classPtr) {
    switch (operation) {
      case kNewFSOpLog: {
        if (!classPtr) {
          kcout << "HCoreKrnl.exe: Miss for classPtr at "
                   "NewFSJournalManager::Run(classPtr) "
                << __FILE__ << "\n";
          return false;
        }

        MUST_PASS(fCacheRoutine);
        return fCacheRoutine(classPtr);
      }
    };

    return false;
  }
};
}  // namespace HCore::Detail

using namespace HCore;

#endif  // ifdef __FSKIT_NEWFS__
