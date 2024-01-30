/* -------------------------------------------

    Copyright Mahrouss Logic

    File: IO-Impl-AMD64.hpp
    Purpose: I/O for AMD64.

    Revision History:

    30/01/24: Added file (amlel)

------------------------------------------- */

namespace HCore {
template <SizeT Sz>
template <typename T>
T IOArray<Sz>::In(SizeT index) {
  switch (sizeof(T)) {
    case 4:
      return HAL::in32(m_Ports[index].Leak());
    case 2:
      return HAL::in16(m_Ports[index].Leak());
    case 1:
      return HAL::in8(m_Ports[index].Leak());
    default:
      return 0xFFFF;
  }
}

template <SizeT Sz>
template <typename T>
void IOArray<Sz>::Out(SizeT index, T value) {
  switch (sizeof(T)) {
#ifdef __x86_64__
    case 4:
      HAL::out32(m_Ports[index].Leak(), value);
    case 2:
      HAL::out16(m_Ports[index].Leak(), value);
    case 1:
      HAL::out8(m_Ports[index].Leak(), value);
#endif
    default:
      break;
  }
}
}  // namespace HCore
