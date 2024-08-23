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

	User::~User()
	{
		mm_delete_ke_heap(fUserNodePtr);
	}

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
				ErrLocal() = kErrorDiskIsFull;
				return false;
			}

			kcout << "newoskrnl: Writing token...\r";

			NFS_FORK_STRUCT fork{0};

			fork.Kind	  = kNewFSDataForkKind;
			fork.DataSize = rt_string_len(password);

			rt_copy_memory((VoidPtr)this->fUserName, fork.ForkName, rt_string_len(this->fUserName));
			rt_copy_memory((VoidPtr)kUsersFile, fork.CatalogName, rt_string_len(kUsersFile));

			fork.DataSize = len;

			new_fs->GetParser()->CreateFork(node, fork);

			// writing the data fork now. False means a data fork.
			bool wrote = new_fs->GetParser()->WriteCatalog(node, false, reinterpret_cast<VoidPtr>(token), len, this->fUserName);

			this->fUserNodePtr = reinterpret_cast<VoidPtr>(node);
			rt_copy_memory(token, this->fUserToken, rt_string_len(token));

			delete[] token;
			token = nullptr;

			kcout << "newoskrnl: Wrote token?\r";
			return wrote;
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

	Bool UserManager::TryLogIn(User& user) noexcept
	{
		if (!user.fUserToken[0] ||
			!user.fUserNodePtr)
		{
			kcout << "newoskrnl: Incorrect data given.\r";

			ErrLocal() = kErrorInvalidData;

			return false;
		}

		kcout << "newoskrnl: Trying to log-in.\r";

		kcout << "newoskrnl: reading: " << reinterpret_cast<NFS_CATALOG_STRUCT*>(user.fUserNodePtr)->Name << endl;

		NewFilesystemManager* new_fs = (NewFilesystemManager*)NewFilesystemManager::GetMounted();

		if (!new_fs)
		{
			ErrLocal() = kErrorInvalidCreds;
			kcout << "newoskrnl: Incorrect filesystem.\r";

			return false;
		}

		NFS_CATALOG_STRUCT* node = new_fs->GetParser()->GetCatalog(kUsersFile);

		if (!node)
		{
			node = reinterpret_cast<NFS_CATALOG_STRUCT*>(user.fUserNodePtr);

			if (!node)
			{
				ErrLocal() = kErrorInvalidCreds;
				kcout << "newoskrnl: Incorrect catalog.\r";

				return false;
			}
		}

		auto token = new_fs->GetParser()->ReadCatalog(node, false, rt_string_len(user.fUserToken), user.fUserName);

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

			if (rt_string_cmp(reinterpret_cast<Char*>(token), user.fUserToken, rt_string_len(user.fUserToken)))
			{
				kcout << "newoskrnl: Incorrect credentials.\r";
				mm_delete_ke_heap(token);

				return false;
			}

			kcout << "newoskrnl: Credentials are correct, moving on.\r";
		}

		fCurrentUser = &user;
		Kernel::kcout << "newoskrnl: Logged in as: " << fCurrentUser->Name() << Kernel::endl;

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
