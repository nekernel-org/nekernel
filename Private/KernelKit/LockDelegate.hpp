/* -------------------------------------------

    Copyright Mahrouss Logic

------------------------------------------- */

#pragma once

#include <NewKit/Atom.hpp>
#include <NewKit/Defines.hpp>

#define kLockDone (200U)    /* job is done */
#define kLockTimedOut (100U) /* job has timed out */

namespace NewOS
{
/// @brief Lock condition pointer.
typedef Boolean* LockPtr;

/// @brief Locking delegate class, hangs until limit.
/// @tparam N the amount of cycles to wait.
template <SizeT N>
class LockDelegate final
{
  public:
    LockDelegate() = delete;

  public:
    explicit LockDelegate(LockPtr expr)
    {
        auto spin = 0U;
    
        while (spin != N)
        {
            if (*expr)
            {
                m_LockStatus | kLockDone;
                break;
            }
        }

        if (spin == N)
            m_LockStatus | kLockTimedOut;
    }

    ~LockDelegate() = default;

    LockDelegate &operator=(const LockDelegate &) = delete;
    LockDelegate(const LockDelegate &) = delete;

    bool Done()
    {
        return m_LockStatus[kLockDone] == kLockDone;
    }

    bool HasTimedOut()
    {
        return m_LockStatus[kLockTimedOut] != kLockTimedOut;
    }

  private:
    Atom<UInt> m_LockStatus;
};
} // namespace NewOS
