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
		kRingStdUser	 = 1,
		kRingSuperUser	 = 2,
		kRingGuestUser = 5,
		kRingCount = 5,
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
		const RingKind& Ring() noexcept;
		const StringView Name() noexcept;

	private:
		RingKind fRing{RingKind::kRingStdUser};
		StringView fUserName{kMaxUserNameLen};

	};

	inline User* cRootUser = nullptr;
} // namespace Kernel

#endif /* ifndef _INC_PERMISSION_SEL_HXX_ */
