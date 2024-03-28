/* -------------------------------------------

    Copyright Mahrouss Logic

------------------------------------------- */

#include <NewKit/Variant.hpp>

namespace NewOS {
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
      return ("Memory:{Undefined}");
  }
}

}  // namespace NewOS
