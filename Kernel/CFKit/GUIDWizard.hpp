/* -------------------------------------------

	Copyright SoftwareLabs

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

namespace NewOS::XRN::Version1
{
	Ref<GUIDSequence*>				make_sequence(const ArrayList<UShort>& seq);
	ErrorOr<Ref<NewOS::StringView>> try_guid_to_string(Ref<GUIDSequence*>& guid);
} // namespace NewOS::XRN::Version1
