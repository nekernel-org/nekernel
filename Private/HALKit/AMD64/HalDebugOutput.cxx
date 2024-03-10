/* -------------------------------------------

    Copyright Mahrouss Logic

------------------------------------------- */

#include <ArchKit/ArchKit.hpp>
#include <KernelKit/DebugOutput.hpp>
#include <NewKit/Utils.hpp>
#include <KernelKit/Framebuffer.hpp>

namespace HCore {
enum CommStatus {
  kStateInvalid,
  kStateReady = 0xCF,
  kStateTransmit = 0xFC,
  kStateCnt = 3
};

namespace Detail {
constexpr short PORT = 0x3F8;

static int kState = kStateInvalid;

/// @brief init COM1.
/// @return 
bool serial_init() noexcept {
#ifdef __DEBUG__
  if (kState == kStateReady ||
      kState == kStateTransmit)
    return true;

  HAL::Out8(PORT + 1, 0x00);  // Disable all interrupts
  HAL::Out8(PORT + 3, 0x80);  // Enable DLAB (set baud rate divisor)
  HAL::Out8(PORT + 0, 0x03);  // Set divisor to 3 (lo byte) 38400 baud
  HAL::Out8(PORT + 1, 0x00);  //                  (hi byte)
  HAL::Out8(PORT + 3, 0x03);  // 8 bits, no parity, one stop bit
  HAL::Out8(PORT + 2, 0xC7);  // Enable FIFO, clear them, with 14-byte threshold
  HAL::Out8(PORT + 4, 0x0B);  // IRQs enabled, RTS/DSR set
  HAL::Out8(PORT + 4, 0x1E);  // Set in loopback mode, test the serial chip
  HAL::Out8(PORT + 0, 0xAE);  // Test serial chip (send byte 0xAE and check if
                              // serial returns same byte)

  // Check if serial is faulty (i.e: not same byte as sent)
  if (HAL::In8(PORT) != 0xAE) {
    ke_stop(RUNTIME_CHECK_HANDSHAKE);
  }

  kState = kStateReady;

  // If serial is not faulty set it in normal operation mode
  // (not-loopback with IRQs enabled and OUT#1 and OUT#2 bits enabled)
  HAL::Out8(Detail::PORT + 4, 0x0F);
#endif // __DEBUG__

  return true;
}
}  // namespace Detail

EXTERN_C void ke_io_print(const char* bytes) {
#ifdef __DEBUG__
  Detail::serial_init();

  if (!bytes || Detail::kState != kStateReady) return;
  if (*bytes == 0) return;

  Detail::kState = kStateTransmit;

  SizeT index = 0;
  SizeT len = rt_string_len(bytes, 256);

  while (index < len) {
    HAL::Out8(Detail::PORT, bytes[index]);
    ++index;
  }

  Detail::kState = kStateReady;
#endif // __DEBUG__
}

EXTERN_C void ke_io_read(const char* bytes) {
#ifdef __DEBUG__
  Detail::serial_init();

  if (!bytes || Detail::kState != kStateReady) return;

  Detail::kState = kStateTransmit;

  SizeT index = 0;

  // send zero.
  HAL::Out8(Detail::PORT, 00);

  // get that zero.
  auto in = HAL::In8(Detail::PORT);

  while (in != '\n') {
    // if zero -> ignore.
    if (in == 0) {
      ++index;
      in = HAL::In8(Detail::PORT);
    
      continue;
    }

    ((char*)bytes)[index] = in;

    if (in == 0)
      break;
    
    ++index;
    in = HAL::In8(Detail::PORT);
  }

  ((char*)bytes)[index] = 0;

  Detail::kState = kStateReady;
#endif // __DEBUG__
}

TerminalDevice TerminalDevice::Shared() noexcept {
  TerminalDevice out(HCore::ke_io_print, HCore::ke_io_read);
  
  return out;
}

}  // namespace HCore
