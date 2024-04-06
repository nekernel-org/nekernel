/* -------------------------------------------

    Copyright Mahrouss Logic

    File: GUIDWizard.cxx
    Purpose: GUID helper code

    Revision History:

------------------------------------------- */

#include <CFKit/GUIDWizard.hpp>
#include <NewKit/Ref.hpp>

// begin of ascii 'readable' characters. (A, C, C, 1, 2)
#define kAsciiBegin 47
// @brief Size of UUID.
#define kUUIDSize 37

namespace NewOS::XRN::Version1 {
auto make_sequence(const ArrayList<UShort>& uuidSeq) -> Ref<GUIDSequence*> {
  GUIDSequence* seq = new GUIDSequence();
  MUST_PASS(seq);

  Ref<GUIDSequence*> sequenceReference{seq, true};

  sequenceReference->fMs1 |= uuidSeq[0];
  sequenceReference->fMs2 |= uuidSeq[1];
  sequenceReference->fMs3 |= uuidSeq[2];
  sequenceReference->fMs3 |= uuidSeq[3];

  return sequenceReference;
}

// @brief Tries to make a guid out of a string.
// This function is not complete for now
auto try_guid_to_string(Ref<GUIDSequence*>& seq) -> ErrorOr<Ref<StringView>> {
  Char buf[kUUIDSize];

  for (SizeT index = 0; index < 16; ++index) {
    buf[index] = seq->u8[index] + kAsciiBegin;
  }

  for (SizeT index = 16; index < 24; ++index) {
    buf[index] = seq->u16[index] + kAsciiBegin;
  }

  for (SizeT index = 24; index < 28; ++index) {
    buf[index] = seq->u32[index] + kAsciiBegin;
  }

  auto view = StringBuilder::Construct(buf);

  if (view) return ErrorOr<Ref<StringView>>{view.Leak()};

  return ErrorOr<Ref<StringView>>{-1};
}
}  // namespace NewOS::XRN::Version1
