/* -------------------------------------------

	Copyright ZKA Technologies.

	File: GUIDWizard.cxx
	Purpose: GUID helper code

	Revision History:

------------------------------------------- */

#include <CFKit/GUIDWizard.hxx>
#include <NewKit/Ref.hxx>

// begin of ascii 'readable' characters. (A, C, C, 1, 2)
#define kAsciiBegin 47
// @brief Size of UUID.
#define kUUIDSize 37

namespace Kernel::XRN::Version1
{
	auto cf_make_sequence(const ArrayList<UInt32>& uuidSeq) -> Ref<GUIDSequence*>
	{
		GUIDSequence* seq = new GUIDSequence();
		MUST_PASS(seq);

		Ref<GUIDSequence*> sequenceReference{seq, true};

		sequenceReference->fMs1	   = uuidSeq[0];
		sequenceReference->fMs2	   = uuidSeq[1];
		sequenceReference->fMs3	   = uuidSeq[2];
		sequenceReference->fMs4[0] = uuidSeq[3];
		sequenceReference->fMs4[1] = uuidSeq[4];
		sequenceReference->fMs4[2] = uuidSeq[5];
		sequenceReference->fMs4[3] = uuidSeq[6];
		sequenceReference->fMs4[4] = uuidSeq[7];
		sequenceReference->fMs4[5] = uuidSeq[8];
		sequenceReference->fMs4[6] = uuidSeq[9];
		sequenceReference->fMs4[7] = uuidSeq[10];

		return sequenceReference;
	}

	// @brief Tries to make a guid out of a string.
	// This function is not complete for now
	auto cf_try_guid_to_string(Ref<GUIDSequence*>& seq) -> ErrorOr<Ref<StringView>>
	{
		Char buf[kUUIDSize];

		for (SizeT index = 0; index < 16; ++index)
		{
			buf[index] = seq->u8[index] + kAsciiBegin;
		}

		for (SizeT index = 16; index < 24; ++index)
		{
			buf[index] = seq->u16[index] + kAsciiBegin;
		}

		for (SizeT index = 24; index < 28; ++index)
		{
			buf[index] = seq->u32[index] + kAsciiBegin;
		}

		auto view = StringBuilder::Construct(buf);

		if (view)
			return ErrorOr<Ref<StringView>>{view.Leak()};

		return ErrorOr<Ref<StringView>>{-1};
	}
} // namespace Kernel::XRN::Version1
