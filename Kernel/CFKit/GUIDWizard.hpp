/* -------------------------------------------

	Copyright ZKA Technologies

------------------------------------------- */

#pragma once

#include <CFKit/GUIDWrapper.hpp>
#include <NewKit/Array.hpp>
#include <NewKit/ArrayList.hpp>
#include <NewKit/Defines.hpp>
#include <NewKit/ErrorOr.hpp>
#include <NewKit/Ref.hpp>
#include <NewKit/Stream.hpp>
#include <NewKit/String.hpp>

namespace Kernel::XRN::Version1
{
	Ref<GUIDSequence*>				 make_sequence(const ArrayList<UShort>& seq);
	ErrorOr<Ref<Kernel::StringView>> try_guid_to_string(Ref<GUIDSequence*>& guid);
} // namespace Kernel::XRN::Version1
