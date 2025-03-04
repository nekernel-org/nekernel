/*
 *	========================================================
 *
 *  	NeKernel
 * 	Copyright (C) 2024-2025, Amlal EL Mahrouss, all rights reserved.
 *
 *	File: User.cc
 * 	Purpose: User class, used to provide authentication and security.
 *
 * 	========================================================
 */

#include <KernelKit/User.h>
#include <KernelKit/LPC.h>
#include <NewKit/KernelPanic.h>
#include <KernelKit/FileMgr.h>
#include <KernelKit/MemoryMgr.h>

#define kStdUserType   (0xEE)
#define kSuperUserType (0xEF)

/// @file User.cc
/// @brief Multi-user support.

namespace NeOS
{
	namespace Detail
	{
		////////////////////////////////////////////////////////////
		/// \brief Constructs a password by hashing the password.
		/// \param password password to hash.
		/// \return the hashed password
		////////////////////////////////////////////////////////////
		const Int32 cred_construct_token(Char* password, const Char* in_password, User* user, SizeT length)
		{
			if (!password || !user)
				return 1;

			kout << "cred_construct_token: Hashing user password...\r";

			for (SizeT i_pass = 0UL; i_pass < length; ++i_pass)
			{
				const Char cur_chr = in_password[i_pass];

				if (cur_chr == 0)
					break;

				password[i_pass] = cur_chr | (user->IsStdUser() ? kStdUserType : kSuperUserType);
			}

			kout << "cred_construct_token: Hashed user password.\r";

			return 0;
		}
	} // namespace Detail

	////////////////////////////////////////////////////////////
	/// @brief User ring constructor.
	////////////////////////////////////////////////////////////
	User::User(const Int32& sel, const Char* user_name)
		: mUserRing((UserRingKind)sel)
	{
		MUST_PASS(sel >= 0);
		rt_copy_memory((VoidPtr)user_name, this->mUserName, rt_string_len(user_name));
	}

	////////////////////////////////////////////////////////////
	/// @brief User ring constructor.
	////////////////////////////////////////////////////////////
	User::User(const UserRingKind& ring_kind, const Char* user_name)
		: mUserRing(ring_kind)
	{
		rt_copy_memory((VoidPtr)user_name, this->mUserName, rt_string_len(user_name));
	}

	////////////////////////////////////////////////////////////
	/// @brief User destructor class.
	////////////////////////////////////////////////////////////
	User::~User() = default;

	Bool User::Save(const UserPublicKey password_to_fill) noexcept
	{
		if (!password_to_fill ||
			*password_to_fill == 0)
			return No;

		SizeT len = rt_string_len(password_to_fill);

		UserPublicKey password = new UserPublicKeyType[len];

		MUST_PASS(password);

		rt_set_memory(password, 0, len);

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

		rt_copy_memory(password, this->mUserKey, rt_string_len(password_to_fill));

		delete[] password;
		password = nullptr;

		kout << "User::Save: Saved password successfully...\r";

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

		kout << "User::Matches: Validating hashed passwords...\r";

		// now check if the password matches.
		if (rt_string_cmp(password, this->mUserKey, rt_string_len(this->mUserKey)) == 0)
		{
			kout << "User::Matches: Password matches.\r";
			return Yes;
		}

		kout << "User::Matches: Password doesn't match.\r";
		return No;
	}

	Bool User::operator==(const User& lhs)
	{
		return lhs.mUserRing == this->mUserRing;
	}

	Bool User::operator!=(const User& lhs)
	{
		return lhs.mUserRing != this->mUserRing;
	}

	////////////////////////////////////////////////////////////
	/// @brief Returns the user's name.
	////////////////////////////////////////////////////////////

	Char* User::Name() noexcept
	{
		return this->mUserName;
	}

	////////////////////////////////////////////////////////////
	/// @brief Returns the user's ring.
	/// @return The king of ring the user is attached to.
	////////////////////////////////////////////////////////////

	const UserRingKind& User::Ring() noexcept
	{
		return this->mUserRing;
	}

	Bool User::IsStdUser() noexcept
	{
		return this->Ring() == UserRingKind::kRingStdUser;
	}

	Bool User::IsSuperUser() noexcept
	{
		return this->Ring() == UserRingKind::kRingSuperUser;
	}
} // namespace NeOS
