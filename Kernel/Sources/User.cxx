/*
 *	========================================================
 *
 *	Kernel
 * 	Copyright ZKA Technologies, all rights reserved.
 *
 *	File: User.cpp
 * 	Purpose: Permission selectors.
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
		const Char* cred_construct_token(const Char* password)
		{
			return nullptr;
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

	Bool UserManager::LogIn(User* user, const Char* password) noexcept
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
			auto tok = Detail::cred_construct_token(password);

			if (rt_string_cmp((Char*)token, tok, rt_string_len(tok)))
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
				this->LogOff();
			}
		}

		fCurrentUser = user;
		Kernel::kcout << "newoskrnl: logged in as: " << fCurrentUser->Name().CData() << Kernel::endl;

		return true;
	}

	User* UserManager::Current() noexcept
	{
		return fCurrentUser;
	}

	Void UserManager::LogOff() noexcept
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
