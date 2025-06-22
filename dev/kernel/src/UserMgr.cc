/*
 *	========================================================
 *
 *  	NeKernel
 * 		Copyright (C) 2024-2025, Amlal El Mahrouss, all rights reserved.
 *
 *		File: UserMgr.cc
 * 		Purpose: User Manager, used to provide authentication and security.
 *
 * 	========================================================
 */

#include <KernelKit/FileMgr.h>
#include <KernelKit/HeapMgr.h>
#include <KernelKit/KPC.h>
#include <KernelKit/UserMgr.h>
#include <NeKit/KernelPanic.h>
#include <NeKit/KString.h>
#include <KernelKit/ThreadLocalStorage.h>
#include <NeKit/Utils.h>

#define kStdUserType (0xEE)
#define kSuperUserType (0xEF)

/// @file User.cc
/// @brief Multi-user support.

namespace Kernel {
namespace Detail {
  ////////////////////////////////////////////////////////////
  /// \brief Constructs a password by hashing the password.
  /// \param password password to hash.
  /// \return the hashed password
  ////////////////////////////////////////////////////////////
  STATIC UInt64 user_fnv_generator(const Char* password, User* user) {
    if (!password || !user) return 1;
    if (*password == 0) return 1;

    kout << "user_fnv_generator: Hashing user password...\r";

    const UInt64 FNV_OFFSET_BASIS = 0xcbf29ce484222325ULL;
    const UInt64 FNV_PRIME        = 0x100000001b3ULL;

    UInt64 hash = FNV_OFFSET_BASIS;

    while (*password) {
      hash ^= (Utf8Char) (*password++);
      hash *= FNV_PRIME;
    }

    kout << "user_fnv_generator: Hashed user password.\r";

    return 0;
  }
}  // namespace Detail

////////////////////////////////////////////////////////////
/// @brief User ring constructor.
////////////////////////////////////////////////////////////
User::User(const Int32& sel, const Char* user_name) : mUserRing((UserRingKind) sel) {
  MUST_PASS(sel >= 0);
  rt_copy_memory_safe((VoidPtr) user_name, this->mUserName, rt_string_len(user_name), kMaxUserNameLen);
}

////////////////////////////////////////////////////////////
/// @brief User ring constructor.
////////////////////////////////////////////////////////////
User::User(const UserRingKind& ring_kind, const Char* user_name) : mUserRing(ring_kind) {
  rt_copy_memory_safe((VoidPtr) user_name, this->mUserName, rt_string_len(user_name), kMaxUserNameLen);
}

////////////////////////////////////////////////////////////
/// @brief User destructor class.
////////////////////////////////////////////////////////////
User::~User() = default;

Bool User::Save(const UserPublicKey password) noexcept {
  if (!password || *password == 0) return No;

  this->mUserFNV = Detail::user_fnv_generator(password, this);

  kout << "User::Save: Saved password successfully...\r";

  return Yes;
}

Bool User::Login(const UserPublicKey password) noexcept {
  if (!password || !*password) return No;

  // now check if the password matches.
  if (this->mUserFNV == Detail::user_fnv_generator(password, this)) {
    kout << "User::Login: Password matches.\r";
    return Yes;
  }

  kout << "User::Login: Password doesn't match.\r";
  return No;
}

Bool User::operator==(const User& lhs) {
  return lhs.mUserRing == this->mUserRing;
}

Bool User::operator!=(const User& lhs) {
  return lhs.mUserRing != this->mUserRing;
}

////////////////////////////////////////////////////////////
/// @brief Returns the user's name.
////////////////////////////////////////////////////////////

Char* User::Name() noexcept {
  return this->mUserName;
}

////////////////////////////////////////////////////////////
/// @brief Returns the user's ring.
/// @return The king of ring the user is attached to.
////////////////////////////////////////////////////////////

const UserRingKind& User::Ring() noexcept {
  return this->mUserRing;
}

Bool User::IsStdUser() noexcept {
  return this->Ring() == UserRingKind::kRingStdUser;
}

Bool User::IsSuperUser() noexcept {
  return this->Ring() == UserRingKind::kRingSuperUser;
}
}  // namespace Kernel
