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

#include <KernelKit/LPC.hxx>
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
		/// \brief Constructs a password by hashing the password.
		/// \param password password to hash.
		/// \return the hashed password
		const Int32 cred_construct_token(Char* password, const Char* in_password, User* user, SizeT length)
		{
			if (!password || !user)
				return -1;

			kcout << "Constructing password...\r";

			for (Size i_pass = 0; i_pass < length; ++i_pass)
			{
				Char cur_chr = in_password[i_pass];

				if (cur_chr == 0)
					break;

				password[i_pass] = cur_chr + (user->IsStdUser() ? cStdUser : cSuperUser);
			}

			kcout << "Done constructing password...\r";

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

	Bool User::TrySave(const Char* password_to_fill) noexcept
	{
		if (!password_to_fill ||
			*password_to_fill == 0)
			return false;

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

			return false;
		}

		// then store password.

		rt_copy_memory(password, this->fUserToken, rt_string_len(password_to_fill));

		delete[] password;
		password = nullptr;

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
} // namespace Kernel
