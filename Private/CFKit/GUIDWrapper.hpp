/*
 *	========================================================
 *
 *	hCore
 * 	Copyright 2024 Mahrouss Logic, all rights reserved.
 *
 * 	========================================================
 */

#pragma once

#include <NewKit/Defines.hpp>
#include <NewKit/Ref.hpp>
#include <NewKit/Stream.hpp>

/* GUID for C++ Components */

#define NULL_GUID "XRN:{........-....-M...-N...-............}"

// eXtensible Resource Information
namespace hCore::XRN
{
    union GUIDSequence
    {
        alignas(8) UShort u8[16];
        alignas(8) UShort u16[8];
        alignas(8) UInt   u32[4];
        alignas(8) ULong  u64[2];

        struct 
        {
            alignas(8) UInt m_Ms1;
            UShort          m_Ms2;
            UShort          m_Ms3;
            UChar           m_Ms4[8];
        };
    };

    class GUID final
    {
    public:
        explicit GUID() = default;
        ~GUID() = default;

	public:
        GUID &operator=(const GUID &) = default;
        GUID(const GUID &) = default;

    public:
        GUIDSequence &operator->()
        {
            return m_UUID;
        }

    private:
        GUIDSequence m_UUID;
        
    };
} // namespace hCore::XRN
