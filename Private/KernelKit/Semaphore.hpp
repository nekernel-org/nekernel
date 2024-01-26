/*
 *	========================================================
 *
 *	hCore
 * 	Copyright 2024 Mahrouss Logic, all rights reserved.
 *
 * 	========================================================
 */

#pragma once

#include <NewKit/Defines.hpp>
#include <CompilerKit/Compiler.hpp>

namespace hCore
{
	class Process;

    typedef Process* ProcessPtr;
	
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
        bool Lock(Process* process);
        bool LockOrWait(Process* process, const Int64& seconds);

    public:
        HCORE_COPY_DEFAULT(Semaphore);

    private:
        ProcessPtr fLockingProcess{ nullptr };

    };
}
