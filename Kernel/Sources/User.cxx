/*
 *	========================================================
 *
 *	Kernel
 * 	Copyright ZKA Technologies, all rights reserved.
 *
 *	File: User.cxx
 * 	Purpose: User concept and management.
 *
 * 	========================================================
 */

#include <KernelKit/User.hxx>
#include <NewKit/KernelCheck.hxx>
#include <KernelKit/FileManager.hxx>
#include <KernelKit/ProcessScheduler.hxx>

#include <KernelKit/Heap.hxx>

/// BUGS: 0

namespace Kernel
{
	namespace Detail
	{
		/// \brief Constructs a token by hashing the password.
		/// \param password password to hash.
		/// \return the hashed password
		const Int32 cred_construct_token(Char* password, User* user)
		{
			if (!password || !user)
				return -1;

			for (Size i_pass = 0; i_pass < rt_string_len(password); ++i_pass)
			{
				Char cur_chr = password[i_pass];
				password[i_pass] = cur_chr + (user->IsStdUser() ? 0xCF : 0xEF);
			}


			return 0;
		}
	}

	User::User(const Int32& sel, const Char* userName)
		: fRing((RingKind)sel)
	{
		MUST_PASS(sel >= 0);

		auto view = StringBuilder::Construct(userName);
		this->fUserName += view.Leak().Leak();
	}

	User::User(const RingKind& ringKind, const Char* userName)
		: fRing(ringKind)
	{
		auto view = StringBuilder::Construct(userName);
		this->fUserName += view.Leak().Leak();
	}

	User::~User() = default;

	bool User::operator==(const User& lhs)
	{
		return lhs.fRing == this->fRing;
	}

	bool User::operator!=(const User& lhs)
	{
		return lhs.fRing != this->fRing;
	}

	StringView& User::Name() noexcept
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
		UserManager* view = nullptr;

		if (!view)
			view = new UserManager();

		return view;
	}

	Bool UserManager::TryLogIn(User* user, const Char* password) noexcept
	{
		if (!password ||
			!user)
		{
			ErrLocal() = kErrorInvalidData;

			kcout << "newoskrnl: Incorrect data given.\r";

			return false;
		}

		FileStreamUTF8 file(kUsersFile, "rb");

		// ------------------------------------------ //
		// Retrieve token from a specific file fork.
		// ------------------------------------------ //

		auto token = file.Read(user->fUserName.CData());

		if (!token)
		{
			ErrLocal() = kErrorInvalidCreds;

			kcout << "newoskrnl: Incorrect credentials.\r";
			return false;
		}
		else
		{
			Char generated_token[255] = { 0 };

			// ================================================== //
			// Provide password on token variable.
			// ================================================== //

			rt_copy_memory((VoidPtr)password, generated_token, rt_string_len(password));

			// ================================================== //
			// Construct token.
			// ================================================== //

			Detail::cred_construct_token(generated_token, user);

			// ================================================== //
			// Checks if it matches the current token we have.
			// ================================================== //

			if (rt_string_cmp((Char*)token, generated_token, rt_string_len(password)))
			{
				kcout << "newoskrnl: Incorrect credentials.\r";

				mm_delete_ke_heap(token);
				return false;
			}

			kcout << "newoskrnl: Correct credentials, moving on.\r";
		}

		// ------------------------------------------ //
		// This was successful, continue.
		// ------------------------------------------ //

		user->fUserToken = token;

		if (fCurrentUser)
		{
			if (!fLastLoggedOffUser)
			{
				fLastLoggedOffUser = fCurrentUser;
			}
			else
			{
				this->TryLogOff();
			}
		}

		fCurrentUser = user;
		Kernel::kcout << "newoskrnl: logged in as: " << fCurrentUser->Name().CData() << Kernel::endl;

		return true;
	}

	User* UserManager::GetCurrent() noexcept
	{
		return fCurrentUser;
	}

	Void UserManager::TryLogOff() noexcept
	{
		if (!fCurrentUser)
			return;

		// an illegal operation just occured, we can't risk more.
		if (fCurrentUser == fRootUser)
		{
			ke_stop(RUNTIME_CHECK_BOOTSTRAP);
		}

		if (fLastLoggedOffUser)
			delete fLastLoggedOffUser;

		fLastLoggedOffUser = nullptr;
		fLastLoggedOffUser = fCurrentUser;
	}
} // namespace Kernel
