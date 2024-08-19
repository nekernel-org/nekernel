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
				Char cur_chr	 = in_password[i_pass];
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
		SizeT len = rt_string_len(password);

		Char* token = new Char[len];

		MUST_PASS(token);

		kcout << "Trying to save password...\r";

		rt_copy_memory((VoidPtr)password, token, len);
		Detail::cred_construct_token(token, password, this, len);

		if (NewFilesystemManager::GetMounted())
		{
			NewFilesystemManager* new_fs = (NewFilesystemManager*)NewFilesystemManager::GetMounted();

			kcout << "newoskrnl: Opening catalog.\r";

			auto node = new_fs->GetParser()->GetCatalog(kUsersFile);

			if (!node)
			{
				node = new_fs->GetParser()->CreateCatalog(kUsersFile);
			}

			kcout << "newoskrnl: Writing token...\r";

			NFS_FORK_STRUCT fork{0};

			fork.Kind = kNewFSRsrcForkKind;
			fork.DataSize = rt_string_len(password);

			rt_copy_memory((VoidPtr)this->fUserName, fork.ForkName, rt_string_len(this->fUserName));
			rt_copy_memory((VoidPtr)kUsersFile, fork.CatalogName, rt_string_len(kUsersFile));

			fork.DataSize = len;

			new_fs->GetParser()->CreateFork(node, fork);

			new_fs->GetParser()->WriteCatalog(node, (fork.Kind == kNewFSRsrcForkKind), reinterpret_cast<VoidPtr>(token), len, this->fUserName);

			delete node;
			node = nullptr;

			delete[] token;
			token = nullptr;

			kcout << "newoskrnl: Wrote token...\r";
			return true;
		}

		kcout << "No filesystem mounted...\r";

		delete[] token;
		token = nullptr;

		return false;
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

	Bool UserManager::TryLogIn(User* user, const Char* password) noexcept
	{
		if (!password ||
			!user)
		{
			kcout << "newoskrnl: Incorrect data given.\r";

			ErrLocal() = kErrorInvalidData;

			return false;
		}

		kcout << "newoskrnl: Trying to log-in.\r";

		NewFilesystemManager* new_fs = (NewFilesystemManager*)NewFilesystemManager::GetMounted();

		// do not use if unmounted.

		if (!new_fs)
			return false;

		auto node = new_fs->GetParser()->GetCatalog(kUsersFile);

		// ------------------------------------------ //
		// Retrieve token from a specific file fork.
		// Fail on null.
		// ------------------------------------------ //

		if (!node)
			return false;

		auto token = new_fs->GetParser()->ReadCatalog(node, rt_string_len(password), user->fUserName);

		if (!token)
		{
			ErrLocal() = kErrorInvalidCreds;

			kcout << "newoskrnl: Incorrect credentials.\r";
			return false;
		}
		else
		{
			Char generated_token[kMaxUserTokenLen] = {0};

			// ================================================== //
			// Provide password on token variable.
			// ================================================== //

			rt_copy_memory((VoidPtr)password, generated_token, rt_string_len(password));

			// ================================================== //
			// Construct token.
			// ================================================== //

			Detail::cred_construct_token(generated_token, password, user, rt_string_len(password));

			// ================================================== //
			// Checks if it matches the current token we have.
			// ================================================== //

			if (rt_string_cmp((Char*)token, generated_token, rt_string_len(password)))
			{
				kcout << "newoskrnl: Incorrect credentials.\r";

				mm_delete_ke_heap(token);
				return false;
			}

			kcout << "newoskrnl: Credentials are correct, moving on.\r";
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
		Kernel::kcout << "newoskrnl: Logged in as: " << fCurrentUser->Name() << Kernel::endl;

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
