/*
 *	========================================================
 *
 *  ZKA
 * 	Copyright ZKA Technologies., all rights reserved.
 *
 *	File: User.cxx
 * 	Purpose: User concept and management.
 *
 * 	========================================================
 */

#include "KernelKit/LPC.hxx"
#include <KernelKit/User.hxx>
#include <NewKit/KernelCheck.hxx>
#include <KernelKit/FileManager.hxx>
#include <KernelKit/ProcessScheduler.hxx>

#include <KernelKit/Heap.hxx>

#define cStdUser   (0xCF)
#define cSuperUser (0xEF)

/// BUGS: 0

namespace Kernel
{
	namespace Detail
	{
		/// \brief Constructs a token by hashing the password.
		/// \param password password to hash.
		/// \return the hashed password
		const Int32 cred_construct_token(Char* password, const Char* in_password, User* user, SizeT length)
		{
			if (!password || !user)
				return -1;

			kcout << "Constructing token...\r";

			for (Size i_pass = 0; i_pass < length; ++i_pass)
			{
				Char cur_chr = in_password[i_pass];

				if (cur_chr == 0)
					break;

				password[i_pass] = cur_chr + (user->IsStdUser() ? cStdUser : cSuperUser);
			}

			kcout << "Done constructing token...\r";

			return 0;
		}
	} // namespace Detail

	User::User(const Int32& sel, const Char* userName)
		: fRing((RingKind)sel)
	{
		MUST_PASS(sel >= 0);
		rt_copy_memory((VoidPtr)userName, this->fUserName, rt_string_len(userName));
	}

	User::User(const RingKind& ringKind, const Char* userName)
		: fRing(ringKind)
	{
		rt_copy_memory((VoidPtr)userName, this->fUserName, rt_string_len(userName));
	}

	User::~User() = default;

	Bool User::TrySave(const Char* password) noexcept
	{
		if (!password ||
			*password == 0)
			return false;

		SizeT len = rt_string_len(password);

		Char* token = new Char[len];

		MUST_PASS(token);

		rt_copy_memory((VoidPtr)password, token, len);
		Detail::cred_construct_token(token, password, this, len);

		rt_copy_memory(token, this->fUserToken, rt_string_len(token));

		delete[] token;
		token = nullptr;

		kcout << "newoskrnl: Saved password...\r";

		return true;
	}

	bool User::operator==(const User& lhs)
	{
		return lhs.fRing == this->fRing;
	}

	bool User::operator!=(const User& lhs)
	{
		return lhs.fRing != this->fRing;
	}

	Char* User::Name() noexcept
	{
		return this->fUserName;
	}

	const RingKind& User::Ring() noexcept
	{
		return this->fRing;
	}

	Bool User::IsStdUser() noexcept
	{
		return this->Ring() == RingKind::kRingStdUser;
	}

	Bool User::IsSuperUser() noexcept
	{
		return this->Ring() == RingKind::kRingSuperUser;
	}

	UserManager* UserManager::The() noexcept
	{
		static UserManager* view = nullptr;

		if (!view)
			view = new UserManager();

		return view;
	}

	Bool UserManager::TryLogIn(User& user, const Char* password, const Char* right_password) noexcept
	{
		if (!password ||
			*password == 0)
			return false;

		rt_copy_memory(reinterpret_cast<VoidPtr>(const_cast<Char*>(password)), user.fUserToken, rt_string_len(password));
		Detail::cred_construct_token(user.fUserToken, password, &user, rt_string_len(password));

		return this->TryLogIn(user, right_password);
	}

	Bool UserManager::TryLogIn(User& user, const Char* token) noexcept
	{
		if (!user.fUserToken[0])
		{
			kcout << "newoskrnl: Incorrect data given.\r";

			ErrLocal() = kErrorInvalidData;

			return false;
		}

		kcout << "newoskrnl: Trying to log-in.\r";

		if (!token)
		{
			ErrLocal() = kErrorInvalidCreds;

			kcout << "newoskrnl: Incorrect token.\r";
			return false;
		}
		else
		{
			// ================================================== //
			// Checks if it matches the current token we have.
			// ================================================== //

			Char password[kMaxUserTokenLen] = {0};

			rt_copy_memory(reinterpret_cast<Char*>(const_cast<Char*>(token)), password, rt_string_len(token));
			Detail::cred_construct_token(password, token, &user, rt_string_len(password));

			if (rt_string_cmp(password, user.fUserToken, rt_string_len(token)))
			{
				kcout << "newoskrnl: Incorrect credentials.\r";
				return false;
			}

			kcout << "newoskrnl: Credentials are correct, moving on.\r";
		}

		fCurrentUser = &user;

		if (fCurrentUser->Name()[0])
			Kernel::kcout << "newoskrnl: Logged in as: " << fCurrentUser->Name() << Kernel::endl;
			else

			Kernel::kcout << "newoskrnl: Logged in as anon. " << Kernel::endl;
		return true;
	}

	User* UserManager::GetCurrent() noexcept
	{
		return fCurrentUser;
	}

	Void UserManager::TryLogOff() noexcept
	{
		fCurrentUser = nullptr;
	}
} // namespace Kernel
