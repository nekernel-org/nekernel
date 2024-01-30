/*
 *	========================================================
 *
 *	HCore
 * 	Copyright 2024 Mahrouss Logic, all rights reserved.
 *
 *	File: PermissionSelector.cpp
 * 	Purpose: Permission primitive type.
 *
 * 	========================================================
 */

#include <KernelKit/PermissionSelector.hxx>
#include <NewKit/RuntimeCheck.hpp>

namespace HCore {
PermissionSelector::PermissionSelector(const Int32 &sel)
    : fRing((RingKind)sel) {
  MUST_PASS(sel > 0);
}

PermissionSelector::PermissionSelector(const RingKind &ringKind)
    : fRing(ringKind) {}

PermissionSelector::~PermissionSelector() = default;

bool PermissionSelector::operator==(const PermissionSelector &lhs) {
  return lhs.fRing == this->fRing;
}

bool PermissionSelector::operator!=(const PermissionSelector &lhs) {
  return lhs.fRing != this->fRing;
}

const RingKind &PermissionSelector::Ring() noexcept { return this->fRing; }
}  // namespace HCore
