/* -------------------------------------------

 Copyright (C) 2024-2025, Amlal EL Mahrouss, all rights reserved.

------------------------------------------- */

#ifndef INC_USER_H
#define INC_USER_H

/* -------------------------------------------

 Revision History:

 04/03/25: Set users directory as /user/ instead of /usr/

 ------------------------------------------- */

#include <CompilerKit/CompilerKit.h>
#include <KernelKit/LPC.h>
#include <NewKit/KString.h>
#include <NewKit/Defines.h>

///! We got the Super, Standard (%s format) and Guest user,
///! all are used to make authorization operations on the OS.
#define kSuperUser "OS AUTHORITY/SUPER/%s"
#define kGuestUser "OS AUTHORITY/GUEST/%s"
#define kStdUser   "OS AUTHORITY/STD/%s"

#define kUsersDir "/user/"

#define kMaxUserNameLen	 (256U)
#define kMaxUserTokenLen (256U)

namespace NeOS
{
	class User;

	enum class UserRingKind
	{
		kRingInvalid   = 0,
		kRingStdUser   = 1,
		kRingSuperUser = 2,
		kRingGuestUser = 5,
		kRingCount	   = 3,
	};

	typedef Char* UserPublicKey;
	typedef Char  UserPublicKeyType;

	/// @brief User class.
	class User final
	{
	public:
		User() = delete;

		User(const Int32& sel, const Char* username);
		User(const UserRingKind& kind, const Char* username);

		~User();

	public:
		NE_COPY_DEFAULT(User);

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
		Bool Matches(const UserPublicKey password) noexcept;

	private:
		UserRingKind mUserRing{UserRingKind::kRingStdUser};
		Char		 mUserName[kMaxUserNameLen] = {0};
		Char		 mUserKey[kMaxUserTokenLen] = {0};
	};
} // namespace NeOS

#endif /* ifndef INC_USER_H */
