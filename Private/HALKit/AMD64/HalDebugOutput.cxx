/*
 *	========================================================
 *
 *	HCore
 * 	Copyright Mahrouss Logic, all rights reserved.
 *
 * 	========================================================
 */

#include <ArchKit/ArchKit.hpp>
#include <KernelKit/DebugOutput.hpp>
#include <NewKit/Utils.hpp>

namespace HCore {
enum CommStatus {
  kStateReady = 0xCF,
  kStateTransmit = 0xFC,
  kStateLimbo,
  kStateCnt = 3
};

namespace Detail {
constexpr short PORT = 0x3F8;

static int kState = kStateLimbo;

bool serial_init() {
#ifdef __DEBUG__
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
#endif

  return true;
}
}  // namespace Detail

void ke_io_print(const char* bytes) {
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
}

TerminalDevice TerminalDevice::Shared() noexcept {
  static TerminalDevice out(HCore::ke_io_print, nullptr);
  return out;
}

}  // namespace HCore
