/* -------------------------------------------

 Copyright ZKA Technologies

------------------------------------------- */

#ifndef _INC_PERMISSION_SEL_HXX_
#define _INC_PERMISSION_SEL_HXX_

#include <CompilerKit/CompilerKit.hxx>
#include <KernelKit/LPC.hxx>
#include <NewKit/String.hxx>
#include <NewKit/Defines.hxx>

// user mode users.
#define kSuperUser "\\Local\\Super"
#define kGuestUser "\\Local\\Guest"

#define kUsersFile "\\Users\\$MANIFEST"

#define kMaxUserNameLen	 (255)
#define kMaxUserTokenLen (4096)

// hash 'password' -> base64+md5 encoded data
// use this data to then fetch specific data of the user..

namespace Kernel
{
	class UserManager;
	class User;

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
		explicit User() = delete;

		User(const Int32& sel, const Char* userName);
		User(const RingKind& kind, const Char* userName);

		~User();

	public:
		NEWOS_COPY_DEFAULT(User)

	public:
		bool operator==(const User& lhs);
		bool operator!=(const User& lhs);

	public:
		/// @brief Get software ring
		const RingKind& Ring() noexcept;

		/// @brief Get user name
		StringView& Name() noexcept;

		/// @brief Is he a standard user?
		Bool IsStdUser() noexcept;

		/// @brief Is she a super user?
		Bool IsSuperUser() noexcept;

		Bool TrySave(const Char* password) noexcept;

	private:
		RingKind   fRing{RingKind::kRingStdUser};
		StringView fUserName;
		VoidPtr	   fUserToken{nullptr};

		friend UserManager;
	};

	class UserManager final
	{
		UserManager()  = default;
		~UserManager() = default;

		User* fCurrentUser		 = nullptr;
		User* fLastLoggedOffUser = nullptr;

	public:
		User* fRootUser = nullptr;

	public:
		NEWOS_COPY_DELETE(UserManager);

		STATIC UserManager* The() noexcept;
		Bool				TryLogIn(User* user, const Char* password) noexcept;
		User*				GetCurrent() noexcept;
		Void				TryLogOff() noexcept;
	};
} // namespace Kernel

#endif /* ifndef _INC_PERMISSION_SEL_HXX_ */
