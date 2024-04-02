/* -------------------------------------------

    Copyright Mahrouss Logic

------------------------------------------- */

#pragma once

#include <NewKit/Defines.hpp>
#include <CompilerKit/CompilerKit.hxx>

namespace NewOS
{
	class ProcessHeader;

    typedef ProcessHeader* ProcessHeaderRef;
	
    /// @brief Access control class, which locks a task until one is done.
    class Semaphore final
    {
    public:
        explicit Semaphore() = default;
        ~Semaphore() = default;

    public:
        bool IsLocked() const;
        bool Unlock() noexcept;

    public:
        void Sync() noexcept;

    public:
        bool Lock(ProcessHeader* process);
        bool LockOrWait(ProcessHeader* process, const Int64& seconds);

    public:
        HCORE_COPY_DEFAULT(Semaphore);

    private:
        ProcessHeaderRef fLockingProcess{ nullptr };

    };
}
