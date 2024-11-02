/*
 *	========================================================
 *
 *  ZKA
 * 	Copyright EL Mahrouss Logic., all rights reserved.
 *
 *	File: User.cc
 * 	Purpose: User class, used to provide authentication and security.
 *
 * 	========================================================
 */

#include <KernelKit/User.h>
#include <KernelKit/LPC.h>
#include <NewKit/Stop.h>
#include <KernelKit/FileMgr.h>
#include <KernelKit/UserProcessScheduler.h>
#include <KernelKit/Heap.h>

#define kStdUserType   (0xCE)
#define kSuperUserType (0xEC)

/// BUGS: 0

namespace Kernel
{
	namespace Detail
	{
		/// \brief Constructs a password by hashing the password.
		/// \param password password to hash.
		/// \return the hashed password
		const Int32 cred_construct_token(Char* password, const Char* in_password, User* user, SizeT length)
		{
			if (!password || !user)
				return 1;

			kcout << "Hashing password...\r";

			for (Size i_pass = 0; i_pass < length; ++i_pass)
			{
				const Char& cur_chr = in_password[i_pass];

				if (cur_chr == 0)
					break;

				password[i_pass] = cur_chr | (user->IsStdUser() ? kStdUserType : kSuperUserType);
			}

			kcout << "Done hashing password!\r";

			return 0;
		}
	} // namespace Detail

	User::User(const Int32& sel, const Char* userName)
		: fRing((UserRingKind)sel)
	{
		MUST_PASS(sel >= 0);
		rt_copy_memory((VoidPtr)userName, this->fUserName, rt_string_len(userName));
	}

	User::User(const UserRingKind& ringKind, const Char* userName)
		: fRing(ringKind)
	{
		rt_copy_memory((VoidPtr)userName, this->fUserName, rt_string_len(userName));
	}

	User::~User() = default;

	Bool User::Save(const UserPublicKey password_to_fill) noexcept
	{
		if (!password_to_fill ||
			*password_to_fill == 0)
			return No;

		SizeT len = rt_string_len(password_to_fill);

		Char* password = new Char[len];
		MUST_PASS(password);

		// fill data first, generate hash.
		// return false on error.

		rt_copy_memory((VoidPtr)password_to_fill, password, len);

		if (!Detail::cred_construct_token(password, password_to_fill, this, len))
		{
			delete[] password;
			password = nullptr;

			return No;
		}

		// then store password.

		rt_copy_memory(password, this->fUserToken, rt_string_len(password_to_fill));

		delete[] password;
		password = nullptr;

		kcout << "Saved password successfully...\r";

		return Yes;
	}

	Bool User::Matches(const UserPublicKey password_to_fill) noexcept
	{
		if (!password_to_fill ||
			*password_to_fill)
			return No;

		SizeT len = rt_string_len(password_to_fill);

		Char* password = new Char[len];
		MUST_PASS(password);

		// fill data first, generate hash.
		// return false on error.

		rt_copy_memory((VoidPtr)password_to_fill, password, len);

		if (!Detail::cred_construct_token(password, password_to_fill, this, len))
		{
			delete[] password;
			password = nullptr;

			return No;
		}

		kcout << "Validating hashed passwords...\r";

		// now check if the password matches.
		return rt_string_cmp(password, this->fUserToken, rt_string_len(this->fUserToken)) == 0;
	}

	Bool User::operator==(const User& lhs)
	{
		return lhs.fRing == this->fRing;
	}

	Bool User::operator!=(const User& lhs)
	{
		return lhs.fRing != this->fRing;
	}

	Char* User::Name() noexcept
	{
		return this->fUserName;
	}

	/// @brief Returns the user's ring.
	/// @return The king of ring the user is attached to.
	const UserRingKind& User::Ring() noexcept
	{
		return this->fRing;
	}

	Bool User::IsStdUser() noexcept
	{
		return this->Ring() == UserRingKind::kRingStdUser;
	}

	Bool User::IsSuperUser() noexcept
	{
		return this->Ring() == UserRingKind::kRingSuperUser;
	}
} // namespace Kernel
