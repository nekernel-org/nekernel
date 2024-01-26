/*
 *	========================================================
 *
 *	hCore
 * 	Copyright 2024 Mahrouss Logic, all rights reserved.
 *
 * 	========================================================
 */

#pragma once

#include <CFKit/GUIDWrapper.hpp>
#include <NewKit/Array.hpp>
#include <NewKit/ArrayList.hpp>
#include <NewKit/ErrorOr.hpp>
#include <NewKit/Defines.hpp>
#include <NewKit/Ref.hpp>
#include <NewKit/Stream.hpp>
#include <NewKit/String.hpp>

namespace hCore::XRN::Version1
{
    Ref<GUIDSequence*> make_sequence(const ArrayList<UShort>& seq);
    ErrorOr<Ref<hCore::StringView>> try_guid_to_string(Ref<GUIDSequence*>& guid);
} // namespace hCore::XRN::Version1
