/* -------------------------------------------

  Copyright (C) 2024-2025, Amlal El Mahrouss, all rights reserved.

------------------------------------------- */

namespace Kernel {
template <class T>
T* DMAWrapper::operator->() {
  return this->fAddress;
}

template <class T>
T* DMAWrapper::Get(UIntPtr offset) {
  return reinterpret_cast<T*>((UIntPtr) this->fAddress + offset);
}
}  // namespace Kernel
