/* -------------------------------------------

	Copyright ZKA Technologies

------------------------------------------- */

#pragma once

#include <NewKit/Defines.hxx>
#include <NewKit/Ref.hxx>
#include <NewKit/Stream.hxx>

/* GUID for C++ Components */

#define kXRNNil "@{........-....-M...-N...-............}"

// eXtensible Resource Information
namespace Kernel::XRN
{
	union GUIDSequence {
		alignas(8) UShort u8[16];
		alignas(8) UShort u16[8];
		alignas(8) UInt u32[4];
		alignas(8) ULong u64[2];

		struct
		{
			alignas(8) UInt fMs1;
			UShort fMs2;
			UShort fMs3;
			UChar  fMs4[8];
		};
	};

	class GUID final
	{
	public:
		explicit GUID() = default;
		~GUID()			= default;

	public:
		GUID& operator=(const GUID&) = default;
		GUID(const GUID&)			 = default;

	public:
		GUIDSequence& operator->() noexcept
		{
			return fUUID;
		}
		GUIDSequence& Leak() noexcept
		{
			return fUUID;
		}

	private:
		GUIDSequence fUUID;
	};
} // namespace Kernel::XRN
