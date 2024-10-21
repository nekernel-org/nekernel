/* -------------------------------------------

	Copyright ZKA Web Services Co.

------------------------------------------- */

#pragma once

#include <CFKit/GUIDWrapper.h>
#include <NewKit/Array.h>
#include <NewKit/ArrayList.h>
#include <NewKit/Defines.h>
#include <NewKit/ErrorOr.h>
#include <NewKit/Ref.h>
#include <NewKit/Stream.h>
#include <NewKit/String.h>

namespace Kernel::XRN::Version1
{
	Ref<GUIDSequence>				 cf_make_sequence(const ArrayList<UInt32>& seq);
	ErrorOr<Ref<Kernel::StringView>> cf_try_guid_to_string(Ref<GUIDSequence>& guid);
} // namespace Kernel::XRN::Version1
