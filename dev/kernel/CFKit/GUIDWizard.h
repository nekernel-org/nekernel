/* -------------------------------------------

  Copyright (C) 2024-2025, Amlal El Mahrouss, all rights reserved.

------------------------------------------- */

#pragma once

#include <CFKit/GUIDWrapper.h>
#include <NeKit/Array.h>
#include <NeKit/ArrayList.h>
#include <NeKit/Defines.h>
#include <NeKit/ErrorOr.h>
#include <NeKit/KString.h>
#include <NeKit/Ref.h>
#include <NeKit/Stream.h>

namespace CF::XRN::Version1 {
using namespace Kernel;

Ref<GUIDSequence*>            cf_make_sequence(const ArrayList<UInt32>& seq);
ErrorOr<Ref<Kernel::KString>> cf_try_guid_to_string(Ref<GUIDSequence*>& guid);
}  // namespace CF::XRN::Version1
