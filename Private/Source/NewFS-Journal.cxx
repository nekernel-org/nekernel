/*
 *	========================================================
 *
 *	hCore
 * 	Copyright 2024 Mahrouss Logic, all rights reserved.
 *
 * 	========================================================
 */

#include <FSKit/NewFS.hxx>
#include <KernelKit/DebugOutput.hpp>

//! @brief Journaling for NewFS.

#define kOpCache (4)

namespace hCore {
typedef Boolean (*NewFSRunner)(VoidPtr delegate);

class NewFSJournalRunner final {
 public:
  NewFSRunner fLoadRoutine{nullptr};
  NewFSRunner fCacheRoutine{nullptr};
  NewFSRunner fUnloadRoutine{nullptr};

  explicit NewFSJournalRunner(NewFSRunner load_runner)
      : fLoadRoutine(load_runner) {
    MUST_PASS(fLoadRoutine);

    fLoadRoutine(this);
  }

  ~NewFSJournalRunner() noexcept {
    MUST_PASS(fUnloadRoutine);

    fUnloadRoutine(this);
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
}  // namespace hCore
