/* -------------------------------------------

 Copyright EL Mahrouss Logic.

------------------------------------------- */

#ifndef INC_USER_H
#define INC_USER_H

#include <CompilerKit/CompilerKit.h>
#include <KernelKit/LPC.h>
#include <NewKit/KString.h>
#include <NewKit/Defines.h>

// user mode users.
#define kSuperUser "OS AUTHORITY/SUPER"
#define kGuestUser "OS AUTHORITY/GUEST"

#define kUsersDir "/Users/"

#define kMaxUserNameLen	 (255)
#define kMaxUserTokenLen (4096)

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

	typedef Char* UserPublicKey;

	class User final
	{
	public:
		explicit User() = delete;

		User(const Int32& sel, const Char* userName);
		User(const UserRingKind& kind, const Char* userName);

		~User();

	public:
		ZKA_COPY_DEFAULT(User)

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
		UserRingKind fRing{UserRingKind::kRingStdUser};
		Char		 fUserName[kMaxUserNameLen]	  = {0};
		Char		 fUserToken[kMaxUserTokenLen] = {0};
	};
} // namespace Kernel

#endif /* ifndef INC_USER_H */
