/* -------------------------------------------

    Copyright Mahrouss Logic

------------------------------------------- */

#include <FirmwareKit/EFI/EFI.hxx>
#include <FirmwareKit/Handover.hxx>
#include <BootKit/Vendor/Support.hxx>

/// @brief memset definition in C++.
/// @param dst destination pointer.
/// @param byte value to fill in.
/// @param len length of of src.
EXTERN_C VoidPtr memset(void *dst, int byte,
                          long long unsigned int len) {
  SetMem(dst, byte, len);
  return dst;
}

/// @brief memcpy definition in C++.
/// @param dst destination pointer.
/// @param  src source pointer.
/// @param len length of of src.
EXTERN_C VoidPtr memcpy(void *dst, const void *src,
                          long long unsigned int len) {
  CopyMem(dst, src, len);
  return dst;
}

/// @brief somthing specific to the microsoft ABI, regarding checking the stack.
EXTERN_C void ___chkstk_ms(void) {}
