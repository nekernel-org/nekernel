/* -------------------------------------------

 Copyright ZKA Technologies

------------------------------------------- */

#ifndef _INC_PERMISSION_SEL_HXX_
#define _INC_PERMISSION_SEL_HXX_

#include <CompilerKit/CompilerKit.hxx>
#include <NewKit/String.hpp>
#include <NewKit/Defines.hpp>

// user mode users.
#define kSuperUser "Admin"
#define kGuestUser "Guest"

#define kUsersDir "\\Users\\Store\\"

#define kMaxUserNameLen (255)

// hash 'password' -> base64+md5 encoded data
// use this data to then fetch specific data of the user..

namespace Kernel
{
	enum class RingKind
	{
		kRingStdUser   = 1,
		kRingSuperUser = 2,
		kRingGuestUser = 5,
		kRingCount	   = 5,
	};

	class User final
	{
	public:
		explicit User() = default;

		User(const Int32& sel, const Char* userName);
		User(const RingKind& kind, const Char* userName);

		~User();

	public:
		NEWOS_COPY_DEFAULT(User)

	public:
		bool operator==(const User& lhs);
		bool operator!=(const User& lhs);

	public:
		/// @brief Getters.
		const RingKind&	 Ring() noexcept;
		const StringView Name() noexcept;

		Bool IsStdUser() noexcept;
		Bool IsSuperUser() noexcept;

	private:
		RingKind   fRing{RingKind::kRingStdUser};
		StringView fUserName{kMaxUserNameLen};
	};

	class UserView final
	{
		UserView()	= default;
		~UserView() = default;

		User* fCurrentUser		 = nullptr;
		User* fLastLoggedOffUser = nullptr;

	public:
		User* fRootUser = nullptr;

	public:
		NEWOS_COPY_DELETE(UserView);

		STATIC UserView* The() noexcept
		{
			UserView* view = nullptr;

			if (!view)
				view = new UserView();

			return view;
		}

		Void LogIn(User* user) noexcept
		{
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
		}

		Void LogOff() noexcept
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
	};
} // namespace Kernel

#endif /* ifndef _INC_PERMISSION_SEL_HXX_ */
