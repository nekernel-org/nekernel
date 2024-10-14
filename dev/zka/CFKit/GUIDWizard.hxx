/* -------------------------------------------

	Copyright ZKA Technologies.

------------------------------------------- */

#pragma once

#include <CFKit/GUIDWrapper.hxx>
#include <NewKit/Array.hxx>
#include <NewKit/ArrayList.hxx>
#include <NewKit/Defines.hxx>
#include <NewKit/ErrorOr.hxx>
#include <NewKit/Ref.hxx>
#include <NewKit/Stream.hxx>
#include <NewKit/String.hxx>

namespace Kernel::XRN::Version1
{
	Ref<GUIDSequence>				 cf_make_sequence(const ArrayList<UInt32>& seq);
	ErrorOr<Ref<Kernel::StringView>> cf_try_guid_to_string(Ref<GUIDSequence>& guid);
} // namespace Kernel::XRN::Version1
