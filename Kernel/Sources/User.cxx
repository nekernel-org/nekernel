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

/// bugs 0

namespace Kernel
{
	User::User(const Int32& sel, const Char* userName)
		: fRing((RingKind)sel)
	{
		MUST_PASS(sel >= 0);
		this->fUserName += userName;
	}

	User::User(const RingKind& ringKind, const Char* userName)
		: fRing(ringKind)
	{
		this->fUserName += userName;
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

	const StringView User::Name() noexcept
	{
		return this->fUserName;
	}

	const RingKind& User::Ring() noexcept
	{
		return this->fRing;
	}
} // namespace Kernel
