/* -------------------------------------------

    Copyright Mahrouss Logic

------------------------------------------- */

#include <KernelKit/DebugOutput.hpp>
#include <KernelKit/FileManager.hpp>

#ifdef __FSKIT_NEWFS__

//! bugs: 0
//! @brief Journaling for NewFS.

#define kOpCache (4)

namespace HCore {
typedef Boolean (*NewFSRunner)(VoidPtr delegate);

class NewFSJournalRunner final {
 public:
  NewFSRunner fLoadRoutine{nullptr};
  NewFSRunner fCacheRoutine{nullptr};
  NewFSRunner fUnloadRoutine{nullptr};

  explicit NewFSJournalRunner(NewFSRunner load_runner)
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
  Boolean Run(const Int32& operation, VoidPtr class_ptr) {
    switch (operation) {
      case kOpCache: {
        if (!class_ptr) {
          kcout << "Miss for class_ptr at NewFSJournalManager::Run(class_ptr) "
                << __FILE__ << "\n";
          return false;
        }

        MUST_PASS(fCacheRoutine);
        return fCacheRoutine(class_ptr);
      }
    };

    return false;
  }
};
}  // namespace HCore

#endif  // ifdef __FSKIT_NEWFS__
