/* -------------------------------------------

 Copyright (C) 2024-2025, Amlal El Mahrouss, all rights reserved.

------------------------------------------- */

#ifndef INC_USER_H
#define INC_USER_H

/* -------------------------------------------

 Revision History:

 04/03/25: Set users directory as /libSystem/ instead of /usr/

 ------------------------------------------- */

#include <CompilerKit/CompilerKit.h>
#include <KernelKit/KPC.h>
#include <NeKit/Defines.h>
#include <NeKit/KString.h>

///! We got the MGMT, STD (%s format) and GUEST users,
///! all are used to make authorized operations.
#define kSuperUser "OS AUTHORITY/MGMT/%s"
#define kGuestUser "OS AUTHORITY/GUEST/%s"
#define kStdUser "OS AUTHORITY/STD/%s"

#define kUsersDir "/users/"

#define kMaxUserNameLen (256U)
#define kMaxUserTokenLen (256U)

namespace Kernel {
class User;

enum class UserRingKind : Int32 {
  kRingInvalid   = 0,
  kRingStdUser   = 444,
  kRingSuperUser = 666,
  kRingGuestUser = 777,
  kRingCount     = 3,
};

typedef Char* UserPublicKey;
typedef Char  UserPublicKeyType;

/// @brief System User class.
class User final {
 public:
  User() = delete;

  User(const Int32& sel, const Char* username);
  User(const UserRingKind& kind, const Char* username);

  ~User();

 public:
  NE_COPY_DEFAULT(User)

 public:
  bool operator==(const User& lhs);
  bool operator!=(const User& lhs);

 public:
  /// @brief Get software ring
  const UserRingKind& Ring() noexcept;

  /// @brief Get user name
  Char* Name() noexcept;

  /// @brief Is he a standard user?
  Bool IsStdUser() noexcept;

  /// @brief Is she a super user?
  Bool IsSuperUser() noexcept;

  /// @brief Saves a password from the public key.
  Bool Save(const UserPublicKey password) noexcept;

  /// @brief Checks if a password matches the **password**.
  /// @param password the password to check.
  Bool Login(const UserPublicKey password) noexcept;

 private:
  UserRingKind mUserRing{UserRingKind::kRingStdUser};
  Char         mUserName[kMaxUserNameLen] = {0};
  UInt64       mUserFNV{0UL};
};
}  // namespace Kernel

#endif /* ifndef INC_USER_H */
