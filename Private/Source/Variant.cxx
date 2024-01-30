/*
 *	========================================================
 *
 *	HCore
 * 	Copyright 2024 Mahrouss Logic, all rights reserved.
 *
 * 	========================================================
 */

#include <NewKit/Variant.hpp>

namespace HCore {
const Char* Variant::ToString() {
  if (m_Ptr == nullptr) {
    return ("Memory:{Nullptr}");
  }

  switch (m_Kind) {
    case VariantKind::kString:
      return ("Class:{String}");
    case VariantKind::kPointer:
      return ("Memory:{Pointer}");
    default:
      return ("Class:{Undefined}");
  }
}

}  // namespace HCore
