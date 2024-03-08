/* -------------------------------------------

    Copyright Mahrouss Logic

------------------------------------------- */

#pragma once

#include <ArchKit/ArchKit.hpp>
#include <KernelKit/FileManager.hpp>
#include <KernelKit/PermissionSelector.hxx>
#include <NewKit/LockDelegate.hpp>
#include <NewKit/MutableArray.hpp>
#include <NewKit/UserHeap.hpp>

/// kernel namespace
namespace HCore {
/// \brief Processs Team (contains multiple processes inside it.)
/// Equivalent to a process batch
class ProcessTeam final {
public:
    explicit ProcessTeam() = default;
    ~ProcessTeam() = default;

    HCORE_COPY_DEFAULT(ProcessTeam);

    MutableArray<Ref<Process>>& AsArray() { return mProcessList; }
    Ref<Process>& AsRef() { return mCurrentProcess; }

public:
    MutableArray<Ref<Process>> mProcessList;
    Ref<Process> mCurrentProcess;

};
} // namespace HCore