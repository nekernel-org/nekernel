/*
 *	========================================================
 *
 *	HCore
 * 	Copyright 2024 Mahrouss Logic, all rights reserved.
 *
 * 	========================================================
 */

#ifndef _INC_PERMISSION_SEL_HPP
#define _INC_PERMISSION_SEL_HPP

#include <CompilerKit/CompilerKit.hpp>
#include <NewKit/Defines.hpp>

// kernel mode user.
#define kMachineUser "machine"

// user mode users.
#define kSuperUser "super"
#define kGuestUser "guest"

// hash 'user@host:password' -> base64 encoded data
// use this data to then fetch specific data.

namespace HCore
{
    enum class RingKind
    {
        kRingUser = 3,
        kRingDriver = 2,
        kRingKernel = 0,
        kUnknown = -1,
    };

    class PermissionSelector final
    {
    private:
        explicit PermissionSelector(const Int32& sel);
        explicit PermissionSelector(const RingKind& kind);

        ~PermissionSelector();

    public:
        HCORE_COPY_DEFAULT(PermissionSelector)

    public:
        bool operator==(const PermissionSelector& lhs);
        bool operator!=(const PermissionSelector& lhs);

    public:
        const RingKind& Ring() noexcept;

    private:
        RingKind fRing;

    };
}

#endif /* ifndef _INC_PERMISSION_SEL_HPP */
