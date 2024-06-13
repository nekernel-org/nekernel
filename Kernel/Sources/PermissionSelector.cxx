/*
 *	========================================================
 *
 *	NewOS
 * 	Copyright Zeta Electronics Corporation, all rights reserved.
 *
 *	File: PermissionSelector.cpp
 * 	Purpose: Permission selectors.
 *
 * 	========================================================
 */

#include <KernelKit/PermissionSelector.hxx>
#include <NewKit/KernelCheck.hpp>

/// bugs 0

namespace NewOS
{
	PermissionSelector::PermissionSelector(const Int32& sel)
		: fRing((RingKind)sel)
	{
		MUST_PASS(sel > 0);
	}

	PermissionSelector::PermissionSelector(const RingKind& ringKind)
		: fRing(ringKind)
	{
	}

	PermissionSelector::~PermissionSelector() = default;

	bool PermissionSelector::operator==(const PermissionSelector& lhs)
	{
		return lhs.fRing == this->fRing;
	}

	bool PermissionSelector::operator!=(const PermissionSelector& lhs)
	{
		return lhs.fRing != this->fRing;
	}

	const RingKind& PermissionSelector::Ring() noexcept
	{
		return this->fRing;
	}
} // namespace NewOS
