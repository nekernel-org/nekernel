/* -------------------------------------------

    Copyright Mahrouss Logic

------------------------------------------- */

#ifndef _INC_PERMISSION_SEL_HPP
#define _INC_PERMISSION_SEL_HPP

#include <CompilerKit/CompilerKit.hxx>
#include <NewKit/Defines.hpp>

// kernel mode user.
#define kMachineUser "Machine"

// user mode users.
#define kSuperUser "Admin"
#define kGuestUser "Guest"

// hash 'user@host:password' -> base64 encoded data
// use this data to then fetch specific data.

namespace NewOS {
enum class RingKind {
  kRingUser = 3,
  kRingDriver = 2,
  kRingKernel = 0,
  kRingUnknown = -1,
  kRingCount = 4,
};

class PermissionSelector final {
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
}  // namespace NewOS

#endif /* ifndef _INC_PERMISSION_SEL_HPP */
