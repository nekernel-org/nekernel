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
#include <NewKit/KernelCheck.hpp>
#include <KernelKit/FileManager.hpp>
#include <KernelKit/ProcessScheduler.hxx>

/// bugs 0

namespace Kernel
{
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

	UserView* UserView::The() noexcept
	{
		UserView* view = nullptr;

		if (!view)
			view = new UserView();

		return view;
	}

	Void UserView::LogIn(User* user, const Char* password) noexcept
	{
		if (!password ||
			!user)
		{
			ErrLocal() = kErrorInvalidData;

			return;
		}

		FileStreamUTF8 file(kUsersFile, "rb");

		auto token = file.Read(password);

		if (!token)
		{
			ErrLocal() = kErrorInvalidCreds;

			kcout << "newoskrnl: Incorrect credentials.\r";
			return;
		}

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
	}

	Void UserView::LogOff() noexcept
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
