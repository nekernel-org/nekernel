/*
 *	========================================================
 *
 *	HCore
 * 	Copyright 2024 Mahrouss Logic, all rights reserved.
 *
 * 	========================================================
 */

#include <CFKit/GUIDWizard.hpp>
#include <NewKit/Ref.hpp>

// begin of ascii 'readable' characters. (A, C, C, 1, 2)
#define kAsciiBegin 47
// @brief Size of UUID.
#define kUUIDSize 37

namespace HCore::XRN::Version1 {
auto make_sequence(const ArrayList<UShort>& uuidSeq) -> Ref<GUIDSequence*> {
  GUIDSequence* seq = new GUIDSequence();
  MUST_PASS(seq);

  Ref<GUIDSequence*> sequenceReference{seq, true};

  sequenceReference->m_Ms1 |= uuidSeq[0];
  sequenceReference->m_Ms2 |= uuidSeq[1];
  sequenceReference->m_Ms3 |= uuidSeq[2];
  sequenceReference->m_Ms3 |= uuidSeq[3];

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
}  // namespace HCore::XRN::Version1
