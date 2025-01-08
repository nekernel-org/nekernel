/* -------------------------------------------

 Copyright (C) 2024, Theater Quality Corp, all rights reserved.

------------------------------------------- */

#ifndef INC_USER_H
#define INC_USER_H

#include <CompilerKit/CompilerKit.h>
#include <KernelKit/LPC.h>
#include <NewKit/KString.h>
#include <NewKit/Defines.h>

///! We got the Super, standard user (%s format) and guest user, all are used to make authorization operations on the OS.
#define kSuperUser "OS AUTHORITY/SUPER/%s"
#define kGuestUser "OS AUTHORITY/GUEST/%s"
#define kFmtUser "OS AUTHORITY/STD/%s"

#define kUsersDir "/usr/"

#define kMaxUserNameLen	 (255U)
#define kMaxUserTokenLen (255U)

namespace Kernel
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

	typedef Char* usr_public_key_kind;

	/// @brief User class.
	class User final
	{
	public:
		explicit User() = delete;

		User(const Int32& sel, const Char* username);
		User(const UserRingKind& kind, const Char* username);

		~User();

	public:
		ZKA_COPY_DEFAULT(User);

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
		Bool Save(const usr_public_key_kind password) noexcept;

		/// @brief Checks if a password matches the **password**.
		/// @param password the password to check.
		Bool Matches(const usr_public_key_kind password) noexcept;

	private:
		UserRingKind mUserRing{UserRingKind::kRingStdUser};
		Char		 mUserName[kMaxUserNameLen]	  = {0};
		Char		 mUserToken[kMaxUserTokenLen] = {0};
	};
} // namespace Kernel

#endif /* ifndef INC_USER_H */
