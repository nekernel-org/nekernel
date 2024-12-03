/* -------------------------------------------

	Copyright (C) 2024, Amlal EL Mahrouss, all rights reserved.

	File: GUIDWizard.cc
	Purpose: GUID helper code

	Revision History:

------------------------------------------- */

#include <CFKit/GUIDWizard.h>
#include <NewKit/Ref.h>

// begin of ascii 'readable' characters. (A, C, C, 1, 2)
#define kUUIDAsciiBegin 47
// @brief Size of UUID.
#define kUUIDSize 37

namespace CFKit::XRN::Version1
{
	auto cf_make_sequence(const ArrayList<UInt32>& uuidSeq) -> Ref<GUIDSequence*>
	{
		GUIDSequence* seq = new GUIDSequence();
		MUST_PASS(seq);

		Ref<GUIDSequence*> seq_ref{seq};

		seq_ref.Leak()->fMs1	= uuidSeq[0];
		seq_ref.Leak()->fMs2	= uuidSeq[1];
		seq_ref.Leak()->fMs3	= uuidSeq[2];
		seq_ref.Leak()->fMs4[0] = uuidSeq[3];
		seq_ref.Leak()->fMs4[1] = uuidSeq[4];
		seq_ref.Leak()->fMs4[2] = uuidSeq[5];
		seq_ref.Leak()->fMs4[3] = uuidSeq[6];
		seq_ref.Leak()->fMs4[4] = uuidSeq[7];
		seq_ref.Leak()->fMs4[5] = uuidSeq[8];
		seq_ref.Leak()->fMs4[6] = uuidSeq[9];
		seq_ref.Leak()->fMs4[7] = uuidSeq[10];

		return seq_ref;
	}

	// @brief Tries to make a guid out of a string.
	// This function is not complete for now
	auto cf_try_guid_to_string(Ref<GUIDSequence*>& seq) -> ErrorOr<Ref<KString>>
	{
		Char buf[kUUIDSize];

		for (SizeT index = 0; index < 16; ++index)
		{
			buf[index] = seq.Leak()->u8[index] + kUUIDAsciiBegin;
		}

		for (SizeT index = 16; index < 24; ++index)
		{
			buf[index] = seq.Leak()->u16[index] + kUUIDAsciiBegin;
		}

		for (SizeT index = 24; index < 28; ++index)
		{
			buf[index] = seq.Leak()->u32[index] + kUUIDAsciiBegin;
		}

		auto view = StringBuilder::Construct(buf);

		if (view)
			return ErrorOr<Ref<KString>>{view.Leak()};

		return ErrorOr<Ref<KString>>{-1};
	}
} // namespace CFKit::XRN::Version1
